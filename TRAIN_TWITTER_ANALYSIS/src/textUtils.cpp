#include <string>
#include <stdio.h>
#include <ctype.h>

#include "textUtils.h"

using namespace std;

TextUtils::TextUtils(){
    ignore_chars = {'.', ',', '!', '"', '~', '&', '*', '%', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    ignore_words = {"the", "a", "an", "but", "this", "is"};
}

string TextUtils::clean_word(string input) {
    string result = input;
    for (int i = 0; i < result.length(); i++) {
        bool erased = false;
        for (char ch : ignore_chars) {
            if (result[i] == ch) {
                result = result.erase(i, 1);
                erased = true;
            } else {
                result[i] = tolower(result[i]);
            }
        }
        if (erased) i--;
    }

    if (ignore_words.find(result) != ignore_words.end()) {
        result = "";
    }
    return result;
}
