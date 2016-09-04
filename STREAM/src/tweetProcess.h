#include <string> 
#include <vector> 
#include <map> 



class TweetProcess { 
	
	public: 
		TweetProcess();
		int writeToBuffer(std::string input, bool local);
		void processTweet(bool local); 
		double score_tweet(std::string tweet, std::map<std::string, double>& word_scores);
		std::map<std::string, double> create_map();
		std::string trim_word(std::string untrimmed);
		void writeToTrainingFile(std::string tweet);
		void writeToDatabase(std::string tweet, std::string tweetTime, double score, bool local); 
		std::string get_current_time(); 

	private: 
		char m_buffer[1000000]; 
		std::vector<std::string> m_tweets; 
		int m_buffPlace; 
		std::map<std::string, double> m_sentiWordScores; 
}; 
