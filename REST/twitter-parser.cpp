#include "restclient-cpp/connection.h"
#include "restclient-cpp/restclient.h"
#include "base64.h"
#include "json/json.h"
#include <iostream>
#include <string>
#include <sstream>
#include <set>

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
 *
 * TODO: Make pos_words and neg_words actually contain positive and negative words, 
 * maybe load them from a file?
 */
double score_tweet(string tweet) {
    set<string> pos_words;
    pos_words.insert("a"); // Just for simple testing
    set<string> neg_words;
    neg_words.insert("the"); //Just for simple testing
    double score = 0;
    istringstream iss(tweet);
    string word;
    // iterate over words in tweet, contributing each word to overall score
    while (iss >> word) {
        if (pos_words.find(word) != pos_words.end()) {
            score++;
        }
        if (neg_words.find(word) != neg_words.end()) {
            score--;
        }
    }
    return score;
}

void print_tweets(string search_query, string auth, string n_tweets) {
    
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
    
    for (unsigned int i = 0; i < statuses.size(); i++) {
        string tweet = statuses[i]["text"].asString();
        double score = score_tweet(tweet);
        cout << "Tweet: " << tweet << "     Score: " << score << endl;
    }

}


int main(int argc, char *argv[]) {
    string consumer_key    = "5d4rCYhsym7BbdKfmeD0uftca";
    string consumer_secret = "VR6dnqif2EioPxYAJjpanBhncZRA32fbLAHdVUHZYyMTG1dY4N";
    
    string auth = get_bearer_token(consumer_key, consumer_secret);
    if (argc == 3) { 
        print_tweets(argv[1], auth, argv[2]);
    } else {
        cout << "Invalid # of arguments. Arg1: search term, Arg2: number of tweets." << endl;
    }
    
    return 0;
}
