#include <vector>
#include <map>
#include <math.h>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
#include "twitterAnalysis.h"

#define MIN_WORDS 5

using namespace std;

TwitterAnalysis::TwitterAnalysis(){
	//tu = new TextUtils; TODO
}

void TwitterAnalysis::count_words_in_tweets(string filename) {
	ifstream file(filename);

	string tweet;

	while(getline(file, tweet)) {
		istringstream iss(tweet);
		string word;
		map<string, double> bag;
		while (iss >> word) {
			//word = tu.cleanWord(); TODO write TextUtils class that cleans words
			bag[word]++;
		}
		bags.push_back(bag);
	}
}

void TwitterAnalysis::count_all_words() {
	for (map<string, double> bag : bags) {
		for (const auto &key_value : bag) {
			word_counts[key_value.first] += key_value.second;
		}
	}
}


void TwitterAnalysis::loop_through_word_pairs(string outputfile, double threshold_corr) {
	int word_pairs = 0;
	int correlations_found = 0;
	ofstream output;
        output.open(outputfile, std::ios_base::app);



	for (const auto &key_value_1 : word_counts) {
		string word1 = key_value_1.first;
		for (const auto &key_value_2 : word_counts) {
			string word2 = key_value_2.first;
			if (word1 < word2) {
				double correlation = calculate_correlation(word1, word2);
				word_pairs++;
				if (word_pairs % 500000 == 0) {
					cout << "Currently on word pair #" << word_pairs << ": " << word1 << " and " << word2 << endl;
				}
				if (correlation > threshold_corr) {
					output.seekp(0, ios::end);
					output << word1 << " " << word2 << " " << correlation << endl;
					correlations_found++;
				}
			}
		}
	}

        output.close();
	cout << "Found " << correlations_found << " high correlations out of " << word_pairs << "word pairs." << endl;
} 

double TwitterAnalysis::calculate_correlation(string word1, string word2) {
	if (word_counts[word1] < MIN_WORDS || word_counts[word2] < MIN_WORDS) return 0;
	vector<double> counts_one;
	vector<double> counts_two;
	counts_one.push_back(0.0);
	counts_two.push_back(0.0);
	for (map<string, double> curr_bag : bags) {
		double count_one = curr_bag[word1];
		double count_two = curr_bag[word2];
		if (count_one > 0 || count_two > 0) {
			counts_one.push_back(count_one);
			counts_two.push_back(count_two);
		}
	}
	return get_pearson_correlation(counts_one, counts_two);
}

double TwitterAnalysis::get_pearson_correlation(vector<double> one, vector<double> two) {
	double sum_sq_x = 0;
	double sum_sq_y = 0;
	double sum_coproduct = 0;
	double mean_x = one[0];
	double mean_y = two[0];
	for (size_t i = 2; i < one.size() + 1; i++) {
		double sweep = ((double) (i-1)) / i;
		double delta_x = one[i-1] - mean_x;
		double delta_y = two[i-1] - mean_y;
		sum_sq_x += delta_x * delta_x * sweep;
		sum_sq_y += delta_y * delta_y * sweep;
		sum_coproduct += delta_x * delta_y * sweep;
		mean_x += delta_x / i;
		mean_y += delta_y / i;
	}
	double pop_sd_x = (double) sqrt(sum_sq_x/one.size());
	double pop_sd_y = (double) sqrt(sum_sq_y/one.size());
	double cov_x_y = sum_coproduct / one.size();
	return cov_x_y / (pop_sd_x * pop_sd_y);
}
