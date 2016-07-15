#include "restclient-cpp/connection.h"
#include "restclient-cpp/restclient.h"
#include <iostream>
#include "base64.h"
#include "json/json.h"

using namespace std;

string encode_oauth(string consumer_key, string consumer_secret) {
    string concat_key_secret = consumer_key + ":" + consumer_secret;
    return base64_encode(reinterpret_cast<const unsigned char*>(concat_key_secret.c_str()), concat_key_secret.length());
}

string get_bearer_token(string key, string secret) {
   
    RestClient::init();
    RestClient::Connection* conn = new RestClient::Connection("");

    // provide the AUTH api with the necessary headers
    RestClient::HeaderFields headers;
    string encoded_key = encode_oauth(key, secret); // special formatting required to encode our api key
    headers["Authorization"] = "Basic " + encoded_key;
    headers["Content-Type"] = "application/x-www-form-urlencoded;charset=UTF-8";
    conn->SetHeaders(headers);

    // issue POST and request AUTH token
    RestClient::Response r = conn->post("https://api.twitter.com/oauth2/token", "grant_type=client_credentials");
    delete conn;
    RestClient::disable();

    // parse returned AUTH token
    const string& body = r.body;
    Json::Reader json_reader; 
    Json::Value root;
    Json::FastWriter fastWriter;
    json_reader.parse(body, root, false);
    string access_token = fastWriter.write(root.get("access_token", NULL)); // TODO give it a non-null argument here
    return access_token;
}

void print_tweets(string search_query, string auth) {
    
    RestClient::init();
    RestClient::Connection *conn = new RestClient::Connection("https://api.twitter.com/1.1/search/tweets.json");

    // provide the REST api with our cleaned up Auth token
    RestClient::HeaderFields headers;
    headers["Authorization"] = "Bearer " + auth;
    conn->SetHeaders(headers);

    // issue the request to the REST api for our search term
    string request = "?q=" + search_query;
    RestClient::Response r = conn->get(request);
    delete conn;
    RestClient::disable();

    // parse the returned json with the linked Json library
    const string &body = r.body;
    Json::Reader json_reader;
    Json::Value root_json;
    json_reader.parse(body, root_json, false);

    cout << root_json << endl;
}


int main(int argc, char *argv[]) {
    string consumer_key    = "5d4rCYhsym7BbdKfmeD0uftca";
    string consumer_secret = "VR6dnqif2EioPxYAJjpanBhncZRA32fbLAHdVUHZYyMTG1dY4N";
    
    string auth = get_bearer_token(consumer_key, consumer_secret);
    string auth_clean = auth.substr(1,auth.length() - 3); // json fastWriter returns a extra chars at the end of the string


    
    cout << auth << endl;
    cout << auth_clean << endl;
    print_tweets("trump", auth_clean);
    
    cout << "program completed." << endl;
    return 0;
}
