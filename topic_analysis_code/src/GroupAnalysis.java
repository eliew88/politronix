import java.util.*;
import java.io.*;
import java.lang.Object.*;
/**
 * This class groups words into "topics" by their correlations.
 *
 * @author Eli Echt-Wilson
 */
public class GroupAnalysis
{
    ArrayList<Pair> correlatedPairs;
    // all of the groups created from correlatedPairs.
    ArrayList<Set<String>> groups = new ArrayList<Set<String>>();

    /**
     * Constructor for objects of class GroupAnalysis
     */
    public GroupAnalysis(String inputFileName)
    {
        readWordPairs(inputFileName);
    }

    //reads the word pairs out of fileName and saves them in correlatedPairs.
    void readWordPairs(String fileName)
    {
        correlatedPairs = new ArrayList<Pair>();
        Scanner scan = getFileScanner(fileName);
        while (scan.hasNext())
        {
            String word1 = scan.next();
            String word2 = scan.next();
            double correlation = scan.nextDouble();
            Pair newPair = new Pair(word1, word2, correlation);
            correlatedPairs.add(newPair);
        }
        System.out.println("Read " + correlatedPairs.size() + " correlated word pairs from " + fileName + ".");

    }

    // prints the groups
    public void printGroups()
    {
        for (int i = 0; i < groups.size(); i++)
        {
            Object[] currentGroup = groups.get(i).toArray();
            System.out.print(currentGroup.length + ": ");
            for (int j = 0; j < currentGroup.length; j++)
            {
                System.out.print(currentGroup[j] + ", ");
            }
            System.out.println();
        }
    }

    public static Scanner getFileScanner( String filename )
    {
        try {
            return new Scanner( new File(filename) );
        } catch (IOException ex)
        { throw new RuntimeException(ex); }
    }

    // this does a simple grouping algorithm by choosing a word and adding all words that are highly correlated to it.
    // Then it chooses another word not in the group and repeats.
    public void groupWords()
    {
        while (correlatedPairs.size() > 0)
        {
            String firstWord = correlatedPairs.get(0).getWordOne();
            Set<String> cluster = getGroup(firstWord);
            groups.add(cluster);
        }
    }

    // This method gets the group for a single word.
    public Set<String> getGroup(String word)
    {
        TreeSet<String> group = new TreeSet<String>(); // the group we are building
        TreeSet<String> added = new TreeSet<String>(); // words that we need to add to group
        added.add(word);
        while (added.size() > 0) // while there are more words to add
        {
            String newWord = added.first();
            added.remove(newWord); //remove the word from added and put it in group
            group.add(newWord);
            while (getPairIndexOf(newWord) != -1) // while there are other words that are correlated to newWord
            {
                // Add the word that is highly correlated to newWord and remove the pair from correlatedPairs to avoid repeats.
                // This word will be added to group during one of the times around the main loop.
                added.add(correlatedPairs.remove(getPairIndexOf(newWord)).getOtherWord(newWord));
            }
        }
        return group;
    }

    // returns the index of a pair in correlatedPairs where one of the words matches word.
    public int getPairIndexOf(String word)
    {
        int count = 0;
        for (Pair a : correlatedPairs) //loop through pairs
        {
            if (a.startsWith(word) || a.getWordTwo().equals(word))    //until it finds the pair that starts with word
                return count;          //return index of first pair that starts with word
            count++;
        }
        return -1;                     //if no pairs start with word
    }
}
