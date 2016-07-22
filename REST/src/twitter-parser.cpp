#include "restclient-cpp/connection.h"
#include "restclient-cpp/restclient.h"
#include "mysql_connection.h"
#include "mysql_driver.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>

#include "base64.h"
#include "json/json.h"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <set>
#include <map>
#include <unistd.h>

using namespace std;

string encode_oauth(string consumer_key, string consumer_secret) {
    string concat_key_secret = consumer_key + ":" + consumer_secret;
    return base64_encode(reinterpret_cast<const unsigned char*>(concat_key_secret.c_str()), concat_key_secret.length());
}

string get_bearer_token(string key, string secret) {

    RestClient::init();
    RestClient::Connection* conn = new RestClient::Connection("");

    // provide the AUTH api with the necessary headers
    RestClient::HeaderFields headers;
    string encoded_key = encode_oauth(key, secret); // special formatting required to encode our api key
    headers["Authorization"] = "Basic " + encoded_key;
    headers["Content-Type"] = "application/x-www-form-urlencoded;charset=UTF-8";
    conn->SetHeaders(headers);

    // issue POST and request AUTH token
    RestClient::Response r = conn->post("https://api.twitter.com/oauth2/token", "grant_type=client_credentials");
    delete conn;
    RestClient::disable();

    // parse returned AUTH token
    const string& body = r.body;
    Json::Reader json_reader; 
    Json::Value root;
    Json::FastWriter fastWriter;
    json_reader.parse(body, root, false);
    string access_token = root["access_token"].asString();

    return access_token;
}

/* Function: score_tweet
 * ---------------------
 * Scores a tweet based on the numbers of positive and negative words in the tweet,
 * returning the calculated score.
 */
double score_tweet(string tweet, map<string, double>& word_scores) {
    double score = 0;
    istringstream iss(tweet);
    string word;
    // iterate over words in tweet, contributing each word to overall score
    while (iss >> word) {
        score += word_scores[word];
    }
    return score;
}

void print_tweets(string search_query, string auth, string n_tweets, map<string, double>& word_scores) {

    RestClient::init();
    RestClient::Connection *conn = new RestClient::Connection("https://api.twitter.com/1.1/search/tweets.json");

    // provide the REST api with our cleaned up Auth token
    RestClient::HeaderFields headers;
    headers["Authorization"] = "Bearer " + auth;
    conn->SetHeaders(headers);

    // issue the request to the REST api for our search term
    string request = "?q=" + search_query + "&count=" + n_tweets;
    RestClient::Response r = conn->get(request);
    delete conn;
    RestClient::disable();

    // parse the returned json with the linked Json library
    const string &body = r.body;
    Json::Reader json_reader;
    Json::Value root_json;
    json_reader.parse(body, root_json, false);
    Json::Value statuses = root_json["statuses"];

    double totalScore = 0;

    for (unsigned int i = 0; i < statuses.size(); i++) {
        string tweet = statuses[i]["text"].asString();
        double score = score_tweet(tweet, word_scores);
        totalScore += score;
        cout << "Tweet: " << tweet << "     Score: " << score << endl;
    }

    cout << "Total score for " << search_query << " is " << totalScore << endl; 

}

string trim_word(string untrimmed) {
    return untrimmed.substr(0, untrimmed.length() - 2);
}

//For testing purposes, prints map of sentiment scores
void print_map(map<string, double> map) {
    for (std::map<string,double>::iterator it=map.begin(); it!=map.end(); ++it) {
        cout << it->first << "=>" << it->second << endl;
    }
    cout << endl;
}

map<string, double> create_map() {
    map<string, double> word_scores;
    ifstream file("SentiWordNet_3.0.0_20130122.txt");
    string entry;

    for (int i = 0; i < 27; i++) {
        getline(file, entry);
    }

    while (file >> entry) {
        file >> entry;
        file >> entry;

        double score = 0;
        if (entry != "#" && entry != "") {
            score = stod(entry);
        }
        file >> entry;
        if (entry != "#" && entry != "") {
            score -= stod(entry);
        }

        file >> entry;

        string word = trim_word(entry);
        word_scores[word] = score;

        getline(file, entry);
    }
    // print_map(word_scores);
    return word_scores;
}



void continual_tweets(string search, string auth, map<string, double>& word_scores) {
    // init sql_connection
    
    sql::mysql::MySQL_Driver *driver;
    sql::Connection *sql_conn;
    sql::Statement *stmt;

    driver = sql::mysql::get_mysql_driver_instance();
    sql_conn = driver->connect("localhost", "george", "testpw");

    stmt = sql_conn->createStatement();

    stmt->execute("USE POLITRONIX");
   

    // Init REST Client
    RestClient::init();
    RestClient::Connection *conn = new RestClient::Connection("https://api.twitter.com/1.1/search/tweets.json");

    RestClient::HeaderFields headers;
    headers["Authorization"] = "Bearer " + auth;
    conn->SetHeaders(headers);
    
    while(true) {

        //issue the request to the REST api for our search term
        string request = "?q=" + search + "&count=" + "100"; //ten each time
        RestClient::Response r = conn->get(request);

        // parse the returned json with the linked Json library
        const string &body = r.body;
        Json::Reader json_reader;
        Json::Value root_json;
        json_reader.parse(body, root_json, false);
        Json::Value statuses = root_json["statuses"];
        
        double total_score = 0;
        for (unsigned int i = 0; i < statuses.size(); i++) {
            string tweet = statuses[i]["text"].asString();
            double tweet_score = score_tweet(tweet, word_scores);
            total_score += tweet_score;
        }
        
        string double_str = static_cast<ostringstream&>(ostringstream() << total_score).str();
        string sql_statement = "INSERT INTO data(topic, score) VALUES ('" + search + "', " + double_str + ")";
        cout << sql_statement << endl;
        stmt->execute(sql_statement);
        
        usleep(4000000);
    }
    
    delete conn;
    RestClient::disable();

    delete stmt;
    delete sql_conn;
}

int main(int argc, char *argv[]) {
    string consumer_key    = "5d4rCYhsym7BbdKfmeD0uftca";
    string consumer_secret = "VR6dnqif2EioPxYAJjpanBhncZRA32fbLAHdVUHZYyMTG1dY4N";

    map<string, double> word_scores = create_map();

    string auth = get_bearer_token(consumer_key, consumer_secret);
    
    
    if (argc == 3) { 
        print_tweets(argv[1], auth, argv[2], word_scores);
    } else if (argc == 2) {
        continual_tweets(argv[1], auth, word_scores);   
    } else {
        cout << "Invalid # of arguments. Arg1: search term, Arg2: number of tweets." << endl;
    } 

    return 0;
}
