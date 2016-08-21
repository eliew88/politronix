#include <string> 
#include <vector> 
#include <map> 



class TweetProcess { 
	
	public: 
		TweetProcess();
		void writeToBuffer(std::string input);
		void processTweet(); 
		double score_tweet(std::string tweet, std::map<std::string, double>& word_scores);
		std::map<std::string, double> create_map();
		std::string trim_word(std::string untrimmed);
		void writeToDatabase(std::string tweet, std::string time, double score); 
		std::string get_current_time(); 

	private: 
		char m_buffer[1000000]; 
		std::vector<std::string> m_tweets; 
		int m_buffPlace; 
		std::map<std::string, double> m_sentiWordScores; 
}; 
