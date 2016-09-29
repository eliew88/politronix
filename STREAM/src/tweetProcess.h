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

// Struct that stores info for a given sample set of tweets, such as the 
// last time it was written to the database, how often to write, the interval
// of the moving average, and the current moving average
typedef struct {
	boost::posix_time::ptime last_written_time; //last time written to database
	int seconds_between_writes;
	int minutes_to_average;
	double curr_avg;
} SampleSet;

// Stores the current status of a topic
class TopicStatus {

    // Struct to store a tweet score as well as the time received
	typedef struct {
		double score;
		boost::posix_time::ptime time;
	} TweetScore;

	public:
		TopicStatus();
		void add_tweet(double score);
		bool should_write(SampleSet &s);
		void reset(SampleSet &s);
		void update_average(SampleSet &s);
		void initialize_sample_sets();
		std::queue<TweetScore> tweet_scores; //all tweets since last write
		std::vector<SampleSet> sample_sets; // different sample sets keeping track of
};

// class to process tweets - we make one TweetProcess * and use it continually
class TweetProcess { 

	public: 
		TweetProcess();
		~TweetProcess();
		void initialize_statuses();
		int write_to_buffer(std::string input, bool local);
		void find_pos_and_neg_words(std::string& tweet, std::string& pos_words, std::string& neg_words);
        void process_tweet(bool local); 
        std::string clean_word(std::string word);
        double score_tweet(std::string tweet, std::map<std::string, double>& word_scores);
		std::map<std::string, double> create_map();
		std::unordered_map<std::string, std::string> create_topic_map();
		std::string trim_word(std::string untrimmed);
		std::unordered_set<std::string> find_topics(std::string tweet);
		void update_topic(std::string tweet, std::string tweet_time, double score, std::string topic, bool local);
		void write_to_training_file(std::string tweet);
        void write_significant_tweet_to_database(std::string tweet, std::string topic, std::string time, double score, bool local); 
		void write_to_database(std::string tweet, std::string tweetTime, double score, bool local, int write_interval); 
		std::string get_current_time(); 

	private: 
		char m_buffer[1000000]; 
		int m_buffPlace; 
		std::map<std::string, double> word_scores;
		std::unordered_map<std::string, std::string> word_to_topic;
		std::unordered_map<std::string, TopicStatus*> topic_to_status;
};
