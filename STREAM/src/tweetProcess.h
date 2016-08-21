#include <string> 
#include <vector> 



class TweetProcess { 
	
	public: 
		TweetProcess();
		void writeToBuffer(std::string input);
		void processTweet(); 

	private: 
		char m_buffer[1000000]; 
		std::vector<std::string> m_tweets; 
		int m_buffPlace; 
}; 
