#include <string>
#include <stdio.h>
#include <ctype.h>
#include <set>

class TextUtils {
    public: 
        TextUtils();
        std::string clean_word(std::string input);
    private:
        std::set<std::string> ignore_words;
        std::set<char> ignore_chars;
};
