//#include <curl/curl.h>

#include <math.h>
#include <stdlib.h>
#include <oauth.h>
#include <unistd.h>
#include <string> 

#include "../curlcpp/include/curl_easy.h"
#include "../curlcpp/include/curl_ios.h"
#include "../curlcpp/include/curl_exception.h"

#include "tweetProcess.h"

using namespace std;
//using namespace curl;

// Access parameters for the POLITRONIX twitter account
const char *consumer_key = "5d4rCYhsym7BbdKfmeD0uftca";
const char *consumer_secret = "VR6dnqif2EioPxYAJjpanBhncZRA32fbLAHdVUHZYyMTG1dY4N";
const char *access_token = "753622605253320704-gljOrQ7CG5kX9ngOW8cWA9pmEeJhgPy";
const char *access_token_secret = "EPyeeT8wPaFJWQpUOPB7unOVN8UKDIPCvnHogzp9GtADm";


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

bool handle_disconnect(int error_code, int n_disconnects) {
    if(error_code == 500) {
        int sleep_time = 250 * (n_disconnects + 1);
        sleepmil(min(sleep_time, 16*1000));
        return true;
    } else if(error_code > 500) {
        int sleep_time = (5*1000) * pow(2, n_disconnects);
        sleepmil(min(sleep_time, 5*60*1000));
        return true;
    } else if(error_code >= 420 && error_code <= 429) {
        int sleep_time = (1*1000) * pow(2, n_disconnects);
        sleepmil(sleep_time);
        return true;
    } else {
        return false;
    }
}

// change char * to void* ??  
size_t tweet_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    
    size_t total = size * nmemb; //find total size of data that came in 
    string str = string(ptr, total); //turn input char * into c++ string 
    TweetProcess *tweets = reinterpret_cast<TweetProcess*>(userdata); 
    tweets->writeToBuffer(str); 
    //cout << str; 
    return total; 
}


int main(int argc, char *argv[]) {

    TweetProcess *tweets = new TweetProcess; 
    const char *url = "https://stream.twitter.com/1.1/statuses/sample.json";
    char *signedurl = oauth_sign_url2(url, NULL, OA_HMAC, "GET", consumer_key, consumer_secret, access_token, access_token_secret);

    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();

    // URL we're connecting to
    curl_easy_setopt(curl, CURLOPT_URL, signedurl);
    
    // User agent we're going to use, fill this in appropriately
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Politronix/0.1"); //change 0.1 to 0.2 if issues with twitter?? 
    
    // libcurl will now fail on an HTTP error (>=400)
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);
    
    // In this case, we're not specifying a callback function for
    // handling received data, so libcURL will use the default, which
    // is to write to the file specified in WRITEDATA
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, tweets);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tweet_callback); //when data comes it, this calls our write_callback function 


    
    // Execute the request!
    int curlstatus = curl_easy_perform(curl);
    printf("curl_easy_perform terminated with status code %d\n", curlstatus);
    
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    
    return 0;
//////////////// 
/*
    ofstream myfile;
    myfile.open("stream.txt");

    curl_ios<ostream> writer(myfile);
    curl_easy easy(writer);

    easy.add<CURLOPT_URL>(signedurl);
    easy.add<CURLOPT_USERAGENT>("Politronix/0.1");

    easy.perform();
    myfile.close();
    return 0;*/
}
