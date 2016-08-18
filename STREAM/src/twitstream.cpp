#include <iostream>
#include "curl_easy.h"
#include "curl_ios.h"
#include "curl_exception.h"

#include <math.h>
#include <unistd.h>



using namespace std;

string default_api_url = "https://stream.twitter.com/1.1/statuses/sample.json";

class TwitStream {
    public:
        TwitStream(ofstream&, string);
        ~TwitStream();
        void set_consumer_key(string);
        void set_consumer_secret(string);
        void set_access_token(string);
        void set_access_token_secret(string);
        bool handle_disconnect(int error_code, int n_disconnects);
        void connect(void);
    private:
        string consumer_key;
        string consumer_secret;
        string access_token;
        string access_token_secret;
        string api_url;
};

TwitStream::TwitStream(ofstream &, string url=default_api_url) {
    api_url = url;
}

TwitStream::~TwitStream(void) {
    // nothing in deconstructor yet
}

// Setters for the OAUTH 
void TwitStream::set_consumer_key(string c_key) {
    consumer_key = c_key;
}

void TwitStream::set_consumer_secret(string c_secret) {
    consumer_secret = c_secret;
}

void TwitStream::set_access_token(string a_token) {
    access_token = a_token;
}

void TwitStream::set_access_token_secret(string a_secret) {
    access_token_secret = a_secret;
}

bool TwitStream::handle_disconnect(int error_code, int n_disconnects) {
    int m = 1e6;
    if(error_code == 500) {
        int sleep_usec = 250 * (n_disconnects + 1);
        usleep(min(sleep_usec, 16*m));
        return true;
    } else if(error_code > 500) {
        int sleep_usec = (5*m) * pow(2, n_disconnects);
        usleep(min(sleep_usec, 5*60*m));
        return true;
    } else if(error_code >= 420 && error_code <= 429) {
        int sleep_usec = (1*m) * pow(2, n_disconnects);
        usleep(sleep_usec);
        return true;
    } else {
        return false;
    }
}

void TwitStream::connect(void) {

}
