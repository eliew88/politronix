#include <math.h>
#include <stdlib.h>
#include <oauth.h>
#include <unistd.h>
#include <string>
#include <curl/curl.h>
#include <stdio.h>

#include "tweetProcess.h"

using namespace std;

#define STATS_UPDATE_INTERVAL 5000

// Access parameters for the POLITRONIX twitter account
const char *consumer_key = "5d4rCYhsym7BbdKfmeD0uftca";
const char *consumer_secret = "VR6dnqif2EioPxYAJjpanBhncZRA32fbLAHdVUHZYyMTG1dY4N";
const char *access_token = "753622605253320704-gljOrQ7CG5kX9ngOW8cWA9pmEeJhgPy";
const char *access_token_secret = "EPyeeT8wPaFJWQpUOPB7unOVN8UKDIPCvnHogzp9GtADm";

bool local;
int num_disconnects;
int num_tweets;
//number of unsuccessful attempts to connect to twitter before getting data
int num_attempts;

/* Function: handle_disconnect
 * Usage: handle_disconnect(curl_disconnect_error_code, n_disconnects)
 * ----------------------------------------------------
 * This function handles disconnections from the twitter api according to their documentation.
 * The error code is passed, alongside the number of disconnects that have occurred with the 
 * same error code.
 *
 * - TCP/IP disconnections and HTTP 500 errors are handled with a linear backoff, 
 *   starting at 0.25s and increasing to 16s in 0.25s increments.
 * - HTTP errors greater than 500 are handled with an exponential backoff,
 *   starting at 5s and doubling up to 5mins.
 * - Rate limiting errors (HTTP 420 - 429) are handled with an exponential backoff,
 *   starting at 1min and doubling indefinitely.
 * - All other errors cause the function to return false, and the program to exit. 
 *
 */

void sleepmil(int n_millisecs) {
	usleep(n_millisecs * 1000);
}

bool handle_disconnect(int error_code, int n) {
	printf("SLEEPING FOR HTTP CODE: %d \n", error_code);
	if (error_code == 500) {
		int sleep_time = 250 * (n + 1);
		sleepmil(min(sleep_time, 16*1000));
	} else if (error_code > 500) {
		int sleep_time = (5*1000) * pow(2, n);
		sleepmil(min(sleep_time, 5*60*1000));
	} else if (error_code >= 420 && error_code <= 499) {
		int sleep_time = (1*1000) * pow(2, n);
		sleepmil(sleep_time);
	} else if (error_code == 200) {
		sleepmil(10000);
	} else {
		return false;
	}

	return true;
}

//called everytime new input is recieved, calls member function to parse input into tweets
size_t tweet_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
	num_attempts = 0;
	size_t total = size * nmemb; 
	string str = string(ptr, total); 
	TweetProcess *tweets = reinterpret_cast<TweetProcess*>(userdata); 

	num_tweets += tweets->writeToBuffer(str, local); 
	if (num_tweets % STATS_UPDATE_INTERVAL == 0) { //print STATS update periodically
		printf("STATS UPDATE\nNumber of tweets processed: %i\n", num_tweets);
		printf("Number of disconnects: %i\n", num_disconnects);
	}

	return total; 
}


int main(int argc, char *argv[]) {
	// Sets local flag if command line argument "local" is included so that local MySQL database is used
	if (argv[1] != NULL) {    
		string localflag(argv[1]);
		local = (localflag == "local");
	} else {
		local = false;
	}
	TweetProcess *tweets = new TweetProcess; 
	const char *url = "https://stream.twitter.com/1.1/statuses/sample.json";

	curl_global_init(CURL_GLOBAL_ALL);
	CURL *curl = curl_easy_init();

	// User agent we're going to use, fill this in appropriately
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "Politronix/0.1"); 

	// libcurl will now fail on an HTTP error (>=400)
	curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);

	// prepare for errors
	char error_buffer[1000];
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buffer);

	// In this case, we're not specifying a callback function for
	// handling received data, so libcURL will use the default, which
	// is to write to the file specified in WRITEDATA
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, tweets);

	//when data comes it, this calls our write_callback function 
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tweet_callback);	

	while (true) {
		char *signedurl = oauth_sign_url2(url, NULL, OA_HMAC, "GET", consumer_key, consumer_secret, access_token, access_token_secret);
		curl_easy_setopt(curl, CURLOPT_URL, signedurl);
		// Execute the request!
		int curlstatus = curl_easy_perform(curl);
		printf("curl_easy_perform terminated with status code %d\n", curlstatus);
		num_disconnects++;
		printf("Disconnect #%i: %s \n", num_disconnects, error_buffer);

		//get the error code
		long http_code = 0;
		curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);

		//Disconnected from streaming: handle disconnect and exit if needed, otherwise continue streaming
		if (!handle_disconnect(http_code, num_attempts)) {
			// Disconnect error code could not be handled: terminate program
			printf("curl error could not be handled: Error Code: %ld, Curl Status: %i \n ", http_code, curlstatus);
			break;
		}
		num_attempts++;
	}

	curl_easy_cleanup(curl);
	curl_global_cleanup();

	return 0;
}
