#include "restclient-cpp/connection.h"
#include "restclient-cpp/restclient.h"
#include <iostream>
#include "base64.h"

using namespace std;

string encode_oauth(string consumer_key, string consumer_secret) {
    string concat_key_secret = consumer_key + ":" + consumer_secret;
    return base64_encode(reinterpret_cast<const unsigned char*>(concat_key_secret.c_str()), concat_key_secret.length());
}

string get_bearer_token(string key, string secret) {
   
    RestClient::init();
    RestClient::Connection* conn = new RestClient::Connection("");

    RestClient::HeaderFields headers;
    string encoded_key = encode_oauth(key, secret);
    headers["Authorization"] = "Basic " + encoded_key;
    headers["Content-Type"] = "application/x-www-form-urlencoded;charset=UTF-8";
    conn->SetHeaders(headers);

    RestClient::Response r = conn->post("https://api.twitter.com/oauth2/token", "grant_type=client_credentials");
    delete conn;

    string body = r.body;
    
    
    RestClient::disable();
    
    return r.body;
}

int main(int argc, char *argv[]) {
    string consumer_key    = "5d4rCYhsym7BbdKfmeD0uftca";
    string consumer_secret = "VR6dnqif2EioPxYAJjpanBhncZRA32fbLAHdVUHZYyMTG1dY4N";
    string twitter_url = "https://api.twitter.com/1.1/users/search.json";
    string query = "q=Twitter%20API&amp;page=1&amp;count=3"; 

    string test_query = "https://api.twitter.com/1.1/search/tweets.json?q=%40twitterapi";
    
    cout << get_bearer_token(consumer_key, consumer_secret) << endl;
    
    RestClient::init();

    RestClient::Connection* conn = new RestClient::Connection(twitter_url);

    conn->SetBasicAuth(consumer_key, consumer_secret);

    string base = "http://jsonplaceholder.typicode.com";

    RestClient::Response r = RestClient::get(test_query);
    cout << r.code << endl;
    cout << r.body << endl;
    cout << "program completed." << endl;
    return 0;
}
