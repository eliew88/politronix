#include <vector>
#include <map>
#include <string>
#include "textUtils.h"
#include <set>

class TwitterAnalysis {
	public:
		TwitterAnalysis();
		void count_words_in_tweets(std::string filename);
		void count_all_words();
		void loop_through_word_pairs(std::string outputfile, double threshold_corr);
		double calculate_correlation(std::string word1, std::string word2);
		double get_pearson_correlation(std::vector<double> one, std::vector<double> two);
	private:
		std::map<std::string, double> word_counts;
		std::vector<std::map<std::string, double> > bags;		
        TextUtils *tu;
        std::set<std::string> positive_seed_words;
        std::set<std::string> negative_seed_words;

};
