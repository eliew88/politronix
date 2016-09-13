#include <string> 
#include <vector> 
#include <map> 
#include <unordered_map>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <queue>
#include <time.h>
#include <unistd.h>
#include <unordered_set>

#define NUM_TWEETS_SMOOTHING 100

typedef struct {
	boost::posix_time::ptime last_written_time; //last time written to database
	int seconds_between_writes;
	int minutes_to_average;
	double curr_avg;
} SampleSet;

class TopicStatus {

	typedef struct {
		double score;
		boost::posix_time::ptime time;
	} TweetScore;

	public:
		TopicStatus();
		void add_tweet(double score);
		bool should_write(SampleSet s);
		void reset(SampleSet s);
		void update_averages();
		void initialize_sample_sets();
		std::queue<TweetScore> tweet_scores; //all tweets since last write
		std::vector<SampleSet> sample_sets;
};

class TweetProcess { 

	public: 
		TweetProcess();
		void initialize_statuses();
		int writeToBuffer(std::string input, bool local);
		void processTweet(bool local); 
		double score_tweet(std::string tweet, std::map<std::string, double>& word_scores);
		std::map<std::string, double> create_map();
		std::unordered_map<std::string, std::string> create_topic_map();
		std::string trim_word(std::string untrimmed);
		std::unordered_set<std::string> find_topics(std::string tweet);
		void update_topic(std::string tweet, std::string tweet_time, double score, std::string topic, bool local);
		void writeToTrainingFile(std::string tweet);
		void writeToDatabase(std::string tweet, std::string tweetTime, double score, bool local); 
		std::string get_current_time(); 

	private: 
		char m_buffer[1000000]; 
		std::vector<std::string> m_tweets; 
		int m_buffPlace; 
		std::map<std::string, double> m_sentiWordScores;
		std::unordered_map<std::string, std::string> word_to_topic;
		std::unordered_map<std::string, TopicStatus*> topic_to_status;
};
