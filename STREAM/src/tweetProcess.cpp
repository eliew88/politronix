#include <math.h>
#include <stdlib.h>
#include <oauth.h>
#include <string> 
#include <vector> 
#include <iostream>
#include <fstream>
#include <map>
#include <queue>
#include <unordered_set>

#include "mysql_connection.h"
#include "mysql_driver.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/statement.h>
#include "json/json.h"

#include "tweetProcess.h"

#define SIG_WORD_THRESHOLD 0.4

using namespace std;
using namespace boost::posix_time;

/* Constructor: TopicStatus
 * -----------------------
 * Initializes the sample sets for the status
 */
TopicStatus::TopicStatus() {
    initialize_sample_sets();
}

/* Function: add_tweet
 * --------------------
 *  Adds a TweetScore struct to the TopicStatus for a given
 *  topic.
 */
void TopicStatus::add_tweet(double score) {
    ptime tweet_time = microsec_clock::universal_time();
    TweetScore ts = {score, tweet_time};
    tweet_scores.push(ts);
}

/* Function: should_write
 * ----------------------
 * Checks whether it is time for the topic to be written
 * to the database, based on data in TopicStatus
 */
bool TopicStatus::should_write(SampleSet &s) {
    ptime curr = microsec_clock::universal_time();
    ptime interval = curr - seconds(s.seconds_between_writes);
    return s.last_written_time < interval;
}

/* Function: update_time
 * ---------------------
 * Updates the last written time in TopicStatus sample set  to be the
 * current time (only called when writing to database).
 */
void TopicStatus::reset(SampleSet &s) {
    s.last_written_time = microsec_clock::universal_time();
}

/* Function: initialize_sample_sets
 * --------------------------------
 * Initializes the TopicStatus to have the correct vector of sample
 * sets.
 *      1. Writes to DB every minute, MA of ten mins, for 1 hour display
 *      2. Writes to DB every 5 mins, MA of 30 mins, for 5 hour display
 *      3. Writes to DB every 20 mins, MA of 1 hr, for 24 hour display
 *      4. Writes to DB every ~2 hrs, MA of 6 hrs, for 1 week display
 *      5. Writes to DB every ~8 hrs, MA of ~24 hrs, for 1 month display
 */
void TopicStatus::initialize_sample_sets() {
    ptime now = microsec_clock::universal_time();
    SampleSet one = {now, 60, 10, 0};
    sample_sets.push_back(one);
    SampleSet two = {now, 300, 30, 0};
    sample_sets.push_back(two);
    SampleSet three = {now, 1200, 60, 0};
    sample_sets.push_back(three);
    SampleSet four = {now, 8000, 360, 0};
    sample_sets.push_back(four);
    SampleSet five = {now, 32000, 1400, 0};
    sample_sets.push_back(five);
}

/* Function: get_average
 * ---------------------
 * Gets the average of all the tweet scores for a given sample set in the
 * TopicStatus. Discards all tweets that are older than 12 hours.
 */
void TopicStatus::update_average(SampleSet &s) {
    ptime now = microsec_clock::universal_time();
    ptime minimum = now - minutes(1400);
    TweetScore front = tweet_scores.front();
    while (front.time < minimum) {
        tweet_scores.pop();
        front = tweet_scores.front();
    }

    double total = 0;
    double count = 0;
    int size = tweet_scores.size();
    cout << "Queue size: " << size << endl;
    for (int i = 0; i < size; i++) {
        TweetScore ts = tweet_scores.front();
        tweet_scores.pop();
        if (ts.time >= now - minutes(s.minutes_to_average)) {
            total += ts.score;
            count++;
        }
        tweet_scores.push(ts);
    }
    s.curr_avg = total / count;
}

/*
 * Function: Constructor
 * ---------------------
 * Create new instance, initialize map
 */
TweetProcess::TweetProcess() {
    m_buffPlace = 0; 
    word_scores = create_map();
    word_to_topic = create_topic_map();
    initialize_statuses();
}

/* Function: Destructor
 * -------------------
 * Destructor to clean up TweetProcess object
 */
TweetProcess::~TweetProcess() {
    for (auto &key_value : topic_to_status) {
        delete key_value.second;
    }
}

/* Function: initialize_statuses
 * -----------------------------
 * Initializes the topic to status map to have empty statuses
 * for each topic. 
 */
void TweetProcess::initialize_statuses() {
    // one TopicStatus for each topic
    string statuses[] = {"clinton", "trump", "johnson", "stein", "pence", "kaine", "democrat", "republican"};
    for (string status : statuses) {
        topic_to_status[status] = new TopicStatus;
    }
}

/* Function: create_topic_map
 * -------------------------
 *  Creates the map that maps words to topics
 *  TODO: Create text file and read in map data from text file
 */
unordered_map<string, string> TweetProcess::create_topic_map() {
	unordered_map<string, string> map;
	map["hillary"] = "clinton";
	map["clinton"] = "clinton";
	map["#imwithher"] = "clinton";
	map["#crookedhillary"] = "clinton";
	map["@hillaryclinton"] = "clinton";
	map["#sheswithus"] = "clinton";
	map["#lockherup"] = "clinton";
	map["#lockher"] = "clinton";
	map["#hillaryforprison"] = "clinton";
	map["#hillary4prison"] = "clinton";
	map["#strongertogether"] = "clinton";
	map["#lovetrumpshate"] = "clinton";
	map["#hillarysarmy"] = "clinton";
	map["trump"] = "trump";
	map["donald"] = "trump";
	map["@realdonaldtrump"] = "trump";
	map["#makeamericagreatagain"] = "trump";
	map["#trump2016"] = "trump";
	map["#hillary2016"] = "clinton";
	map["#makedonalddrumpfagain"] = "trump";
	map["#nevertrump"] = "trump";
	map["#maga"] = "trump";
	map["#GOP"] = "republican";
	map["#hillyes"] = "clinton";
	map["#neverhillary"] = "clinton";
	map["#ohhillno"] = "clinton";
	map["#dumptrump"] = "trump";
	map["#racisttrump"] = "trump";
	map["#trumplies"] = "trump";
	map["#republicansforhillary"] = "clinton";
	map["#dropouthillary"] = "clinton";
	map["@govgaryjohnson"] = "johnson";
	map["#letgarydebate"] = "johnson";
	map["#johnsonweld2016"] = "johnson";
	map["#teamgov"] = "johnson";
	map["#youin"] = "johnson";
	map["@drjillstein"] = "stein";
	map["stein"] = "stein";
	map["pence"] = "pence";
	map["@mike_pence"] = "pence";
	map["#mikepence"] = "pence";
	map["#timkaine"] = "kaine";
	map["kaine"] = "kaine";
	map["@timkaine"] = "kaine";
	map["democrat"] = "democrat";
	map["republican"] = "republican";
	map["@thedemocrats"] = "democrat";
	map["#voteblue"] = "democrat";
	map["#votered"] = "republican";
	map["#leadright2016"] = "republican";
	map["#tcot"] = "republican";
	map["#uniteblue"] = "democrat";
	map["@gop"] = "republican";

	return map;
}

/*
 * Function: writeToBuffer
 * ---------------------
 * break input string into individual tweets, and call process. Returns the number of 
 * tweets processed. 
 */
int TweetProcess::write_to_buffer(string input, bool local) { 
    int tweet_count = 0;
    for (size_t i = 0; i < input.size(); i++) {
        if(input[i] != '\r') {
            m_buffer[m_buffPlace] = tolower(input[i]); 
            m_buffPlace++; 
        }
        else {
            process_tweet(local);
            tweet_count++; 
        }
    }
    return tweet_count;
}

/*
 * Function: processTweet
 * ---------------------
 * Read tweet, score it, and identify topics to be
 * updated and possibly written to database.
 */
void TweetProcess::process_tweet(bool local) {
    string s = string(m_buffer, m_buffPlace);
    m_buffPlace = 0; 
    char finalTime[20]; 
    int timeDiff;
    Json::Reader json_reader;
    Json::Value root_json;
    json_reader.parse(s, root_json, false);
    Json::Value status = root_json["text"];
    Json::Value created_at = root_json["created_at"];
    Json::Value lang = root_json["lang"]; 
    string stat = status.asString(); 
    string createdTime = created_at.asString(); 
    string language = lang.asString();	

    // score the tweet according to word map
    double score = score_tweet(stat, word_scores); 

    //time stuff 
    const char *timeChar = createdTime.c_str(); 
    struct tm result, * ptm, * timeinfo;
    time_t rawtime;
    time ( &rawtime );
    ptm = gmtime ( &rawtime );
    timeinfo = localtime (&rawtime);
    timeDiff = ptm->tm_hour - timeinfo->tm_hour; 

    if (strptime(timeChar, "%a %b %d %T %z %Y", &result) != NULL) {
        result.tm_hour += timeDiff; 
        strftime(finalTime,sizeof(finalTime), "%Y-%m-%d %T", &result);
    }

    if (language == "en") {
        //write_to_training_file(stat);

        // find the topics contained in the tweet, and update every topic
        unordered_set<string> topics = find_topics(stat);
        for (string topic : topics) {
            cout << "New tweet found for " << topic << ". Updating topic status..." << endl;
            update_topic(stat, finalTime, score, topic, local);
        }
    }
}

/* Function: update_topic
 * ----------------------
 * Updates the TopicStatus associated with a given topic, and 
 * writes to the database if necessary.
 */
void TweetProcess::update_topic(string tweet, string tweet_time, double score, string topic, bool local) {
    // if score is significantly positive or negative, write to separate table in database
    if (score > 0.5 || score < -0.5) {
        cout << "Found significantly positive or negative tweet!" << endl;
        //write_significant_tweet_to_database(tweet, topic, tweet_time, score, local);
    }

    // update the topic status, and write to main db table if needed
    TopicStatus *status = topic_to_status[topic];
    status->add_tweet(score);
    for (SampleSet &s : status->sample_sets) {
        if (status->should_write(s)) {
            // average scores for that topic, then write to database
            status->update_average(s);
            write_to_database(topic, tweet_time, s.curr_avg, local, s.seconds_between_writes);
            status->reset(s);
        }
    }
}


/* Function: find_topics
 * ---------------------
 * Takes in a tweet as input and returns a set of all the topics addressed
 * in the tweet.
 */
unordered_set<string> TweetProcess::find_topics(string tweet) {
    unordered_set<string> topics;
    for (auto it = word_to_topic.begin(); it != word_to_topic.end(); ++it) {
        size_t pos = tweet.find(it->first);
        if (pos != std::string::npos) {
            topics.insert(it->second);
        }
    }
    return topics;
}

/* Function: writeToTrainingFile
 * -----------------------------
 * Writes a tweet to a file storing a large collection of tweets, which
 * will be used for training correlation algorithm.
 */
void TweetProcess::write_to_training_file(string tweet) {
    ofstream training_file;
    training_file.open("training_file.txt", std::ios_base::app);
    training_file.seekp(0, ios::end);
    training_file << tweet << endl;
    training_file.close();
}

/* Function: clean_word
 * -------------------
 * Cleans a word, removing certain unnecessary characters.
 */
string TweetProcess::clean_word(string word) {
    unordered_set<char> ignore_chars = {'.', ',', '!', '"', 0x27, '~', '&', '*', '%', '\n'};
    string result = word;
    for (int i = 0; i < result.length(); i++) {
        bool erased = false;
        for (char ch : ignore_chars) {
            if (result[i] == ch) {
                result = result.erase(i, 1);
                erased = true;
            } else {
                result[i] = tolower(result[i]);
            }
        }
        if (erased) i--;
    }

    return result;
}

/* Function: find_positive_words
 * -----------------------------
 * Returns a set of all the positive words (words with a score 
 * higher than the threshold) in a given tweet). Also "cleans"
 * the tweet, getting rid of useless characters in the text
 * to avoid errors in entering to MySQL database.
 */
void TweetProcess::find_pos_and_neg_words(string& tweet, string& pos_words, string& neg_words) {
    string result_tweet;
    istringstream iss(tweet);
    string word;
    while (iss >> word) {
        double score = word_scores[word];
        if (score > SIG_WORD_THRESHOLD) {
            pos_words += word + " ";
        } else if (score < SIG_WORD_THRESHOLD * -1) {
            neg_words += word + " ";
        }
        result_tweet += clean_word(word) + " ";
    }
    tweet = result_tweet; //effectively returns result
}

/* Function: write_significant_tweet_to_database
 * ---------------------------------------------
 * Writes tweets with significantly positive or negative scores
 * to second table in AWS database, along with other information
 */
void TweetProcess::write_significant_tweet_to_database(string tweet, string topic, string time, double score, bool local) {
    string pos_words;
    string neg_words;
    find_pos_and_neg_words(tweet, pos_words, neg_words);
    sql::mysql::MySQL_Driver *driver;
    sql::Connection *sql_conn;
    sql::Statement *stmt;
    driver = sql::mysql::get_mysql_driver_instance();
    if (local) {
        sql_conn = driver->connect("localhost", "politronix", "sbs456Team");
        stmt = sql_conn->createStatement();
        stmt->execute("USE POLITRONIX");
    }   
    else { 
        sql_conn = driver->connect( "politronix-mysql-test1.cba6n6csw5eg.us-west-2.rds.amazonaws.   com:3306", "politronix", "sbs2016Team");
        stmt = sql_conn->createStatement();
        stmt->execute("USE politronix_database");
    }
    string score_str = to_string(score);
    string sql_statement =
        "INSERT INTO tweetfeed(tweet, topic, score, datetime, pos_words, neg_words) VALUES ('"
        + tweet + "', '"
        + topic + "', "
        + score_str + ", '"
        + time + "', '"
        + pos_words + "', '"
        + neg_words + "')";
    cout << sql_statement << endl;
    stmt->execute(sql_statement);
    delete stmt;
    delete sql_conn;

}

/*
 * Function: writeToDatabase
 * ---------------------
 * Writes the most recent smoothed average of tweets for a topic to the database. 
 */
void TweetProcess::write_to_database(string topic, string time, double average_score, bool local, int write_interval) {
    sql::mysql::MySQL_Driver *driver;
    sql::Connection *sql_conn;
    sql::Statement *stmt;
    driver = sql::mysql::get_mysql_driver_instance();
    if (local) {
        sql_conn = driver->connect("localhost", "politronix", "sbs456Team");
        stmt = sql_conn->createStatement();
        stmt->execute("USE POLITRONIX");
    }
    else {
        sql_conn = driver->connect( "politronix-mysql-test1.cba6n6csw5eg.us-west-2.rds.amazonaws.com:3306", "politronix", "sbs2016Team");
        stmt = sql_conn->createStatement();
        stmt->execute("USE politronix_database");
    }
    stmt = sql_conn->createStatement();
    string score_str = to_string(average_score);
    string interval_str = to_string(write_interval);
    string sql_statement = 
        "INSERT INTO data(topic, score, datetime, writeinterval) VALUES ('"
        + topic + "', " 
        + score_str + ", '"
        + time + "', "
        + interval_str + ")";
    cout << sql_statement << endl;
    stmt->execute(sql_statement);
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

/*
 * Function: create_map
 * ---------------------
 * create word to score map for sentiment
 * analysis
 */
map<string, double> TweetProcess::create_map() {
    map<string, double> scores;
    ifstream file("scores.txt");

    string entry;
    while (getline(file, entry)) {
        size_t space_index = entry.find_first_of(' ');
        string word = entry.substr(0, space_index);
        double score = stod(entry.substr(space_index + 1));
        scores[word] = score;
    }

    return scores;
}

/*
 * Function: trim_word
 * ---------------------
 * trim words to fit in database? 
 */
string TweetProcess::trim_word(string untrimmed) {
    return untrimmed.substr(0, untrimmed.length() - 2);
}

/*
 * Function: get_current_time
 * ---------------------
 * get the time to add tweets 
 * change changing tweet string time to 
 * proper format of current get_time 
 */
string TweetProcess::get_current_time() {
    ptime t = microsec_clock::universal_time(); 
    return to_iso_extended_string(t);
}

