#include "restclient-cpp/connection.h"
#include "restclient-cpp/restclient.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
    string consumer_key    = "5d4rCYhsym7BbdKfmeD0uftca";
    string consumer_secret = "VR6dnqif2EioPxYAJjpanBhncZRA32fbLAHdVUHZYyMTG1dY4N";
    string twitter_url = "https://api.twitter.com/1.1/users/search.json";
    string query = "q=Twitter%20API&amp;page=1&amp;count=3";

    RestClient::init();

    RestClient::Connection* conn = new RestClient::Connection(twitter_url);

    conn->SetBasicAuth(consumer_key, consumer_secret);

    string base = "http://jsonplaceholder.typicode.com";
    RestClient::Response r = RestClient::get(base + "/posts/1");
    cout << r.body << endl;
    cout << "program completed." << endl;
}
