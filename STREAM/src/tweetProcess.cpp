#include <math.h>
#include <stdlib.h>
#include <oauth.h>
#include <unistd.h>
#include <string> 
#include <vector> 
#include <iostream>
using namespace std;
//using namespace curl;

#include "tweetProcess.h"

/*
	class to process tweets into buffer 
	seperate each tweet from string based on new line, put into array

*/


TweetProcess::TweetProcess() {
	m_buffPlace = 0; 
}

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

void TweetProcess::processTweet() {
	string s = string(m_buffer, m_buffPlace);
	m_tweets.push_back(s); 
	m_buffPlace = 0; 
	cout << s << endl << endl; 
}

