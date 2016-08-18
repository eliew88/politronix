#include "../curlcpp/include/curl_easy.h"
#include "../curlcpp/include/curl_ios.h"
#include "../curlcpp/include/curl_exception.h"

#include <math.h>
#include <oauth.h>
#include <unistd.h>

using namespace std;
using namespace curl;

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

int main(int argc, char *argv[]) {

    const char *url = "https://stream.twitter.com/1.1/statuses/sample.json";
    char *signedurl = oauth_sign_url2(url, NULL, OA_HMAC, "GET", consumer_key, consumer_secret, access_token, access_token_secret);

    ofstream myfile;
    myfile.open("stream.txt");

    curl_ios<ostream> writer(myfile);
    curl_easy easy(writer);

    easy.add<CURLOPT_URL>(signedurl);
    easy.add<CURLOPT_USERAGENT>("Politronix/0.1");

    easy.perform();
    myfile.close();
    return 0;
}
