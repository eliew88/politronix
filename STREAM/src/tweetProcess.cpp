#include <math.h>
#include <stdlib.h>
#include <oauth.h>
#include <unistd.h>
#include <string> 
#include <vector> 
#include <iostream>
#include <fstream>
#include <map>
#include <unordered_set>

#include "mysql_connection.h"
#include "mysql_driver.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "json/json.h"

#include "tweetProcess.h"

using namespace std;

const char MYSQL_HOST[] = "localhost";
const char MYSQL_USER[] = "politronix";
const char MYSQL_PASSWORD[] = "sbs456Team"; 


TweetProcess::TweetProcess() {
	m_buffPlace = 0; 
	m_sentiWordScores = create_map(); 
	cout << "size map: "<< m_sentiWordScores.size() << endl; 


}

//break input string into individual tweets, and call process
void TweetProcess::writeToBuffer(string input) { 

	for(int i = 0; i < input.size(); i++) {
		if(input[i] != '\r') {
			m_buffer[m_buffPlace] = tolower(input[i]); 
			m_buffPlace++; 
		}
		else {
			processTweet(); 
		}
	}

}

//put strings into vector and generate score to be put in the database 
void TweetProcess::processTweet() {
	string s = string(m_buffer, m_buffPlace);
	m_tweets.push_back(s); 
	m_buffPlace = 0; 
	//cout << s << endl << endl; 

	//const string &body = r.body;
    Json::Reader json_reader;
    Json::Value root_json;
    json_reader.parse(s, root_json, false);
    Json::Value status = root_json["text"];
    Json::Value created_at = root_json["created_at"]; 
    string stat = status.asString(); 
    string createdTime = created_at.asString(); 

    double score = score_tweet(stat, m_sentiWordScores); 
    cout << stat << endl << score << endl << endl;
    cout << "Created at: " << createdTime << endl; 
    writeToDatabase(stat, createdTime, score); 
}

/*
* Function: writeToDatabase
 * ---------------------
 * parse the topic of a tweet, and put that information into the database 
 */
 
void TweetProcess::writeToDatabase(string tweet, string time, double score){

	sql::mysql::MySQL_Driver *driver;
    sql::Connection *sql_conn;
    sql::Statement *stmt;

    driver = sql::mysql::get_mysql_driver_instance();
    sql_conn = driver->connect(MYSQL_HOST, MYSQL_USER, MYSQL_PASSWORD);

    stmt = sql_conn->createStatement();

    stmt->execute("USE POLITRONIX");

	int topicSize = 11; 
	string topics[11] = {"clinton", "trump", "donald", "hillary", "democrat", "republican", "election", "gary", "gohnson", "jill", "stein"}; 
	size_t pos;
	for(int i = 0; i < topicSize; i++) {
		pos = tweet.find(topics[i]); 
		if (pos != std::string::npos){
			string score_str = to_string(score);
        	string sql_statement = 
            "INSERT INTO data(topic, score, datetime) VALUES ('"
            + topics[i] + "'," 
            + score_str + ", '"
            + get_current_time() + "')";
        cout << sql_statement << endl;
        stmt->execute(sql_statement);
		}
	}


	delete stmt;
    delete sql_conn;
}

/*
* Function: score_tweet
 * ---------------------
 * Scores a tweet based on the numbers of positive and negative words in the tweet,
 * returning the calculated score.
 */
double TweetProcess::score_tweet(string tweet, map<string, double>& word_scores) {
    double score = 0;
    istringstream iss(tweet);
    string word;
    static unordered_set<string> negation_words = {
        "not", "no", "never", "don't", "cannot", "ain't", "aren't", "can't", "couldn't",
        "didn't", "doesn't", "hadn't", "hasn't", "haven't", "mustn't", "needn't", 
        "shouldn't", "wasn't", "weren't", "won't", "wouldn't"};
    bool last_word_negative = false;
    // iterate over words in tweet, contributing each word to overall score
    while (iss >> word) {
        if (negation_words.find(word) == negation_words.end()) {
            if (last_word_negative) {
                score -= word_scores[word];
            } else {
                score += word_scores[word];
            }
        }
        last_word_negative = (negation_words.find(word) != negation_words.end());
    }
    return score;
}

map<string, double> TweetProcess::create_map() {
    map<string, double> word_scores;
    ifstream file("SentiWordNet_3.0.0_20130122.txt");

    string entry;

    //Get rid of the documentation (first 27 lines of the text file)
    for (int i = 0; i < 27; i++) {
        getline(file, entry);
    }

    //parse the rest of the file (each run of loop parses one line)
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

string TweetProcess::trim_word(string untrimmed) {
    return untrimmed.substr(0, untrimmed.length() - 2);
}

string TweetProcess::get_current_time() {
    using namespace boost::posix_time;
    ptime t = microsec_clock::universal_time(); 
    return to_iso_extended_string(t);
}

