/**
 * This class represents a pair of words, and stores their correlation.
 * Author: Eli Echt-Wilson
 */
public class Pair {

    public String word1;
    public String word2;
    public double correlation;
    /**
     * Constructor for objects of class Pair
     */
    public Pair(String one, String two, double corr) {
        word1 = one;
        word2 = two;
        correlation = corr;
    }

    public String getWordOne() {
        return word1;
    }

    public String getWordTwo() {
        return word2;
    }

    public double getCorrelation() {
        return correlation;
    }

    public boolean wordsAre(String wordOne, String wordTwo) {
        if ((wordOne.equals(word1) && wordTwo.equals(word2)) ||
                (wordTwo.equals(word1) && wordOne.equals(word2)))
            return true;
        return false;
    }

    public boolean startsWith(String word) {
        return (word.equals(word1));
    }

    public String getOtherWord(String word) {
        if (word.equals(word1))
            return word2;
        return word1;
    }
}

