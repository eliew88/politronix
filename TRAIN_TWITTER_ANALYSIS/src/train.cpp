#include <stdio.h>
#include <iostream>
#include "twitterAnalysis.h"

using namespace std;


int main() {
	cout << "Welcome to Twitter Analyzer!" << endl;
	TwitterAnalysis *analyzer = new TwitterAnalysis;
	cout << "Counting words in tweets in training file... " << endl;
	analyzer->count_words_in_tweets("training_file.txt");
	cout << "Counting all words... " << endl;
	analyzer->count_all_words();
	cout << "Calculating correlations and writing to output file... " << endl;
	analyzer->loop_through_word_pairs("correlations.txt", 0.1);
	return 0;
}
