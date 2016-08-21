#include <math.h>
#include <stdlib.h>
#include <oauth.h>
#include <unistd.h>
#include <string> 
#include <vector> 
#include <iostream>

#include "json/json.h"

#include "tweetProcess.h"

using namespace std;


TweetProcess::TweetProcess() {
	m_buffPlace = 0; 
}

//break input string into individual tweets, and call process
void TweetProcess::writeToBuffer(string input) { 

	for(int i = 0; i < input.size(); i++) {
		if(input[i] != '\r') {
			m_buffer[m_buffPlace] = input[i]; 
			m_buffPlace++; 
		}
		else {
			processTweet(); 
		}
	}

}

//put strings into vector 
void TweetProcess::processTweet() {
	string s = string(m_buffer, m_buffPlace);
	m_tweets.push_back(s); 
	m_buffPlace = 0; 
	//cout << s << endl << endl; 

	//const string &body = r.body;
    Json::Reader json_reader;
    Json::Value root_json;
    json_reader.parse(s, root_json, false);
    Json::Value statuses = root_json["text"];
    cout << statuses << endl << endl;
}

