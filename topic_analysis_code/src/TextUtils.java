
import java.util.*;
/**
 * Utilities to support processing text.
 *
 * @author Eli Echt-Wilson
 */
public class TextUtils  {
    // A list of words we will ignore in the input.
    ArrayList<String> ignoreWords = new ArrayList<String>(
            Arrays.asList("the", "i", "me", "in", "at", "my", "thank", "of", "im", "and",
                    "you", "on", "for", "be", "if", "a", "an", "by", "to", "--", "ive", "it",
                    "ill", "do", "or", "get", "from", "mr", "he", "she", "oh", "we", "go", "weve", "got", "as", "your",
                    "were", "what", "will", "with", "would", "then", "they", "theyre", "cheer", "but", "now", "that", "when",
                    "any", "make", "made", "instead", "isnt", "put", "this", "thi", "wont", "are", "also", "mr", "have",
                    "been", "our", "not", "want", "just", "only", "going", "applause", "laughter", "about", "all",
                    "because", "can", "each", "every", "let", "more", "must", "one", "out", "said", "say", "them",
                    "there", "their", "these", "those", "well", "who", "thing", "stuff", "sure", "than",
                    "many", "both", "crosstalk"));
    ArrayList<String> ignoreChars = new ArrayList<String>(
            Arrays.asList(";", "\"", ",", ".", "'", "’", "?", "-", "(", ")", "[", "]", ":", "#", "@")); //list of chars we want to ignore

    /**
     * Constructor for objects of class textUtils
     */
    public TextUtils() {
        //empty
    }

    /* Makes words into a cleaner version by removing unwanted characters
     * and removing s at the end of the word.
     */
    public String cleanWord(String inputWord) {
        String word = inputWord.toLowerCase();
        for (int i = 0; i < word.length(); i++) //cycles through the letters of the word
        {
            for (int j = 0; j < ignoreChars.size(); j++) //cycles through the characters we wish to ignore
            {
                word = word.replace(ignoreChars.get(j), ""); //if the char exists in the word, replace it with an empty string, removing it
            }
        }

        if (word.length() != 0)  //if the word still has characters in it
        {
            if ( word.charAt(word.length()-1) == 's' ) //if the last letter is s
            {
                word = word.substring(0, word.length()-1); //remove the s
            }
        }

        return word;
    }

    //returns whether we should ignore a "bag" of words or not
    public boolean shouldNotIgnoreBag( ArrayList bag ) {
        boolean decision = true; //set default to true
        if (bag.size() <= 10) //if the bag has less than 10 words
        {
            decision = false; //ignore it
        }
        return decision;
    }

    //decide whether an entire word should be ignored
    public boolean shouldNotIgnoreWord(String word) {
        boolean decision = true; //set default to true
        for (int i = 0; i < ignoreWords.size(); i++) //loop through the list of words we want to ignore
        {
            if(word.toLowerCase().equals(ignoreWords.get(i))) //if the given word is the same as a word we want to ignore
            {
                decision = false; //ignore it
            }
            else if (word.length() <= 2) //if the word is too short(<=2 characters)
            {
                decision = false; //ignore it
            }

        }

        return decision;
    }
}

