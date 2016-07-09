import java.util.*;
import java.io.*;
import java.lang.Object;
/**
 * This class takes a large set of words and learns the correlations
 * of word pairs by breaking them into bags of related words, counts the instances of words in bags,
 * and calculates a Pearson's correlation between them.
 * @author Eli Echt-Wilson
 */
public class TextAnalysis
{
    // when splitting by proximity, how many words are in each bag
    static final int proximityWordsPerBag = 150;
    // bags separates the initial text into bags of related words
    ArrayList<ArrayList<String>> bags = new ArrayList<ArrayList<String>>();
    // for each bag in bags, the corresponding map in bagMaps counts how often each word appears
    ArrayList<TreeMap<String, Double>> bagMaps = new ArrayList<TreeMap<String, Double>>();
    TextUtils tu = new TextUtils();
    // a count of all words in the text
    TreeMap<String, Double> wordCounts = new TreeMap<String, Double>();
    //a count of the times that a pair has counts (0,0)
    int countZeros = 0;
    //a count of word pairs that we use for correlation
    int countWordPairs = 0;
    // the final list of pairs of words and their correlations
    public ArrayList<Pair> correlatedWordPairs = new ArrayList<Pair>();
    // whether this class is for learning by proximity
    boolean analyzeByProximity = false;

    TextAnalysis(boolean byProximity)
    {
        analyzeByProximity = byProximity;
    }

    // this function adds all the words from inputFileName to our bags of words, splitting by speaker
    public void addLearningWordsBySpeaker( String inputFileName )
    {
        Scanner text = getFileScanner(inputFileName);
        System.out.println("Adding words in " + inputFileName + ". Separating bags by speaker.");
        separateToBagsBySpeaker(text); //separates the text into related bags of words, and "cleans" them using TextUtils class
    }

    // this function adds all the word from inputFileName to our bags of words, separating by proximity
    public void addLearningWordsByProximity( String inputFileName )
    {
        Scanner text = getFileScanner(inputFileName);
        System.out.println("Adding words in " + inputFileName + ". Separating bags by proximity.");
        separateToBagsByProximity(text); //separates the text into related bags of words, and "cleans" them using TextUtils class
    }

    // writes all the bags to a file, one per line, in the format word:count
    public void writeBagsToFile(String outputFileName)
    {
        try {
            PrintWriter out = new PrintWriter(new FileWriter(outputFileName));
            for (TreeMap<String, Double> bagMap : bagMaps) {
                for (Map.Entry<String, Double> entry : bagMap.entrySet()) {
                    out.print(entry.getKey() + ":" + entry.getValue() + " ");
                }
                out.println("");
                //System.out.println("Wrote one bag."); (test)
            }
            out.close();
            System.out.println("Done. Wrote bags to file " + outputFileName + ".");
        } catch (IOException e){
            e.printStackTrace();
        }
    }

    public void writeBagsToMultipleFiles( int numBags, String outputDirectory )
    {
        try {
            int fileNum = 0;
            PrintWriter out = null;

            for (int i = 0; i < bagMaps.size(); i++) {
                if (i % numBags == 0) {
                    if (out != null)
                        out.close();
                    fileNum++;
                    String fileName = outputDirectory + "/input" + fileNum + ".txt";
                    out = new PrintWriter(new FileWriter(fileName));
                }
                TreeMap<String, Double> bagMap = bagMaps.get(i);
                for (Map.Entry<String, Double> entry : bagMap.entrySet()) {
                    out.print(entry.getKey() + ":" + entry.getValue() + " ");
                }
                out.println("");
            }
            out.close();
            System.out.println("Done. Wrote bags to " + fileNum + " files in directory " + outputDirectory + ".");
        } catch (IOException e){
            e.printStackTrace();
        }
    }

    // returns the list of correlated word pairs with their correlations
    public ArrayList<Pair> getCorrelatedWordPairs()
    {
        return correlatedWordPairs;
    }

    // after all the words have been entered, this function goes through all the bags and finds
    // correlations(must be greater than correlation) between words and writes them to output file.
    public void learnBySpeaker(String outputFileName, double correlation)
    {
        System.out.println("Counting words in bags");
        countWordsInBags(); //counts the number of each word in each bag using a TreeMap
        System.out.println("Counting all of the words");
        countAllWords(); //countsAllWords
        System.out.println("Computing correlations");
        loopThroughPairs(outputFileName, correlation); //loops through the pairs in a bag, and saves the correlations in a file
        System.out.println("Done: wrote output to " + outputFileName);
        printStatistics();
    }

    // after all the words have been entered, this function goes through all the bags and finds
    // correlations(must be greater than correlation) between words and writes them to output file.
    // This version works by using bags that are close to eachother.
    public void learnByProximity(String outputFileName, double correlation)
    {
        System.out.println("Counting words in bags");
        countWordsInProximity(); //counts the number of each word in each bag using a TreeMap, and adds smaller values for close bags
        System.out.println("Counting all of the words");
        countAllWords(); //countsAllWords
        System.out.println("Computing correlations");
        loopThroughPairs(outputFileName, correlation); //loops through the pairs in a bag, and saves the correlations in a file
        System.out.println("Done: wrote output to " + outputFileName);
        printStatistics();
    }

    //finds pearson correlation between two arrays
    public static double getPearsonCorrelation(double[] scores1,double[] scores2){
        double result = 0;
        double sum_sq_x = 0;
        double sum_sq_y = 0;
        double sum_coproduct = 0;
        double mean_x = scores1[0];
        double mean_y = scores2[0];
        for(int i=2;i<scores1.length+1;i+=1){
            double sweep =Double.valueOf(i-1)/i;
            double delta_x = scores1[i-1]-mean_x;
            double delta_y = scores2[i-1]-mean_y;
            sum_sq_x += delta_x * delta_x * sweep;
            sum_sq_y += delta_y * delta_y * sweep;
            sum_coproduct += delta_x * delta_y * sweep;
            mean_x += delta_x / i;
            mean_y += delta_y / i;
        }
        double pop_sd_x = (double) Math.sqrt(sum_sq_x/scores1.length);
        double pop_sd_y = (double) Math.sqrt(sum_sq_y/scores1.length);
        double cov_x_y = sum_coproduct / scores1.length;
        result = cov_x_y / (pop_sd_x*pop_sd_y);
        return result;
    }

    //prints out various useful statistics about the computation we have run
    public void printStatistics() {
        System.out.println("Number of bags: " + bags.size());
        int totalWords = 0;
        for (String key = wordCounts.firstKey(); key != null; key = wordCounts.higherKey(key))
        {
            totalWords += wordCounts.get(key);
        }
        System.out.println("Number of words: " + totalWords);
        System.out.println("Number of distinct words: " + wordCounts.size());
        System.out.println("Number of (0, 0) instances in correlation computation: " + countZeros);
        System.out.println("Number of word pairs that we actually correlate: " + countWordPairs);
    }

    // counts the number of each word in each bag and stores it in bagMaps
    public void countWordsInBags()
    {
        for (int i = 0; i < bags.size(); i++) //loops through all of the bags
        {
            TreeMap<String, Double> map = new TreeMap<String, Double>(); //creates a new TreeMap for each bag
            bagMaps.add(map); //adds the TreeMap to ArrayList of Treemaps
            for (int j = 0; j < bags.get(i).size(); j++) //loops through the current bag
            {
                String word = bags.get(i).get(j); //current word
                if (map.get(word) != null) //if the word already exists in the map, meaning it has already occurred
                {
                    map.put(word, map.get(word) + 1.0); //increase the integer value that corresponds to the word by 1
                }
                else
                {
                    map.put(word, 1.0); //if the word has not yet occurred, set it to have occurred once
                }
            }
        }

//        for (int k = 0; k < bagMaps.size(); k++) //used to print out the counts of words in maps
//        {
//            TreeMap<String, Double> thisMap = bagMaps.get(k);
//            if (thisMap.size() > 0)
//            {
//                for (String key = thisMap.firstKey(); key != null; key = thisMap.higherKey(key))
//                {
//
//                }
//                //System.out.println();
//            }
//        }

    }

    // counts the words in each of the bags, but also adds an additional 0.5 for instances of words in
    //bags that are just before and after the current bag.
    public void countWordsInProximity()
    {
        for (int i = 0; i < bags.size(); i++) //loops through all of the bags
        {
            TreeMap<String, Double> map = new TreeMap<String, Double>(); //creates a new TreeMap for each bag
            bagMaps.add(map); //adds the TreeMap to ArrayList of Treemaps
            for (int j = 0; j < bags.get(i).size(); j++) //loops through the current bag
            {
                String word = bags.get(i).get(j); //current word
                if (map.get(word) != null) //if the word already exists in the map, meaning it has already occurred
                {
                    map.put(word, map.get(word) + 1.0); //increase the integer value that corresponds to the word by 1
                }
                else
                {
                    map.put(word, 1.0); //if the word has not yet occurred, set it to have occurred once
                }
            }

            if (i != 0)
            {
                // bag immediately before this one
                for (int k = 0; k < bags.get(i-1).size(); k++)
                {
                    String word = bags.get(i-1).get(k); //current word
                    if (map.get(word) != null) //if the word already exists in the map, meaning it has already occurred
                    {
                        map.put(word, map.get(word) + 0.5); //increase the double value that corresponds to the word by 0.5
                    }
                    else
                    {
                        map.put(word, 0.5); //if the word has not yet occurred, set it to have occurred once
                    }
                }
            }
            if (i != bags.size() - 1)
            {
                // bag immediately after this one
                for (int k = 0; k < bags.get(i+1).size(); k++)
                {
                    String word = bags.get(i+1).get(k); //current word
                    if (map.get(word) != null) //if the word already exists in the map, meaning it has already occurred
                    {
                        map.put(word, map.get(word) + 0.5); //increase the double value that corresponds to the word by 0.5
                    }
                    else
                    {
                        map.put(word, 0.5); //if the word has not yet occurred, set it to have occurred once
                    }
                }
            }
        }

    }
    // count all the words
    public void countAllWords()
    {
        for (TreeMap<String, Double> thisMap : bagMaps)
        {
            for (String key : thisMap.keySet())
            {
                if (wordCounts.get(key) == null)
                {
                    wordCounts.put(key, 0.0);
                }
                wordCounts.put(key, wordCounts.get(key) + thisMap.get(key));
            }
        }
    }

    public void writeAllWordsToFile( String outputFileName )
    {
        try {
            PrintWriter out = new PrintWriter(new FileWriter(outputFileName));
            for (String word : wordCounts.keySet()) {
                out.println(word);
                //System.out.println("Wrote one bag."); (test)
            }
            out.close();
            System.out.println("Done. Wrote all words to file " + outputFileName + ".");
        } catch (IOException e){
            e.printStackTrace();
        }
    }

    // separates the text into related bags of words, and "cleans" them using TextUtils class
    public void separateToBagsBySpeaker(Scanner text)
    {
        String word = "";
        Set<String> splitWords = new HashSet<String>(Arrays.asList(
                "obama:", "romney:", "lehrer:", "question:", "crowley:", "schieffer:", "biden:", "ryan:", "raddatz:"));
        ArrayList<String> words = new ArrayList<String>();
        bags.add(words);
        while(text.hasNext())
        {
            word = text.next();
            String lower = word.toLowerCase();
            if (splitWords.contains(lower))
            {
                words = new ArrayList<String>();
                bags.add(words);
            }
            else
            {
                String cleanWord = tu.cleanWord(word);
                if (tu.shouldNotIgnoreWord(cleanWord))
                    words.add(cleanWord);
            }
        }

    }

    // reads all words from the scanner, and separates them into related bags of words using proximity
    public void separateToBagsByProximity(Scanner text)
    {
        ArrayList<String> words = new ArrayList<String>();
        bags.add(words);
        int currentWord = 0;
        while(text.hasNext())
        {
            String word = text.next();
            currentWord++;
            String lower = word.toLowerCase();
            if (currentWord >= proximityWordsPerBag)
            {
                words = new ArrayList<String>();
                bags.add(words);
                currentWord = 0;
            }
            String cleanWord = tu.cleanWord(word);
            if (tu.shouldNotIgnoreWord(cleanWord))
                words.add(cleanWord);
        }
    }

    // calculates the correlation between word1 and word2 across all of the bagMaps
    public double calculateCorrelation( String word1, String word2 ) {
        int minWords = 7;
        if (analyzeByProximity)
            minWords = 14;  // because proximity double-counts the words
        if (wordCounts.get(word1) < minWords || wordCounts.get(word2) < minWords)
            return 0;
        ArrayList<Double> wordCounts1 = new ArrayList<Double>();
        ArrayList<Double> wordCounts2 = new ArrayList<Double>();
        if (analyzeByProximity)
        {
            wordCounts1.add(0.0);  // there are always bags that don't contain either word.
            wordCounts2.add(0.0);
        }
        int numNonZero = 0;
        for (int i = 0; i < bagMaps.size(); i++)
        {
            TreeMap<String, Double> map = bagMaps.get(i);
            double word1Count = 0;
            if (map.containsKey(word1))
            {
                word1Count = map.get(word1);
            }
            double word2Count = 0;
            if (map.containsKey(word2))
            {
                word2Count = map.get(word2);
            }
            if (word1Count > 0 || word2Count > 0)
            {
                wordCounts1.add(word1Count);
                wordCounts2.add(word2Count);
                numNonZero++;
            }
            else if (!analyzeByProximity)
            {
                wordCounts1.add(0.0);
                wordCounts2.add(0.0);
            }
            countWordPairs++;
            if (word1Count == 0.0 && word2Count == 0.0)
            {
                countZeros++;
            }
        }
        double[] wordCounts1Array = new double[wordCounts1.size()];
        double[] wordCounts2Array = new double[wordCounts2.size()];
        for (int i = 0; i < wordCounts1.size(); i++)
        {
            wordCounts1Array[i] = wordCounts1.get(i);
            wordCounts2Array[i] = wordCounts2.get(i);
        }
        double correlation = getPearsonCorrelation(wordCounts1Array, wordCounts2Array);

        // sample code for scatter plot data (uncomment in order to get results):
        //         if (correlation > 0.8 && correlation < 1.1) // && numNonZero > 10)
        //         {
        //             System.out.println("Found correlation " + correlation + ", words are: " + word1 + ", " + word2);
        //             for (int i = 0; i < wordCounts1.size(); i++)
        //             {
        //                 if (wordCounts1.get(i) != 0.0 || wordCounts2.get(i) != 0.0)
        //                 {
        //                     System.out.println(wordCounts1.get(i) + ", " + wordCounts2.get(i));
        //                 }
        //             }
        //         }

        return correlation;
    }

    public static Scanner getFileScanner( String filename )
    {
        try {
            return new Scanner( new File(filename) );
        } catch (IOException ex)
        { throw new RuntimeException(ex); }
    }

    //loops through all of the pairs of distinct words, and saves the correlations in a file
    public void loopThroughPairs(String outputFileName, double thresholdCorrelation)
    {
        try {
            int wordPairs = 0;
            int correlationsFound = 0;
            PrintWriter out = new PrintWriter(new FileWriter(outputFileName));
            TreeMap<String, Double> map = wordCounts;
            if (map.size() > 0)
            {
                for (String word1 = map.firstKey(); (word1 != null); word1 = map.higherKey(word1))
                {
                    for (String word2 = map.higherKey(word1); (word2 != null); word2 = map.higherKey(word2))
                    {
                        double correlation = calculateCorrelation(word1, word2);
                        wordPairs++;
                        if (wordPairs % 500000 == 0)
                            System.out.println("Currently on word pair #" + wordPairs + ": " + word1);
                        if (correlation > thresholdCorrelation)
                        {
                            Pair correlatedWords = new Pair(word1, word2, correlation);
                            correlatedWordPairs.add(correlatedWords);
                            out.print(word1 + " " + word2 + " ");
                            out.println(correlation);
                            correlationsFound++;
                        }
                    }
                }
            }
            out.close();
            System.out.println("Found " + correlationsFound + " high correlations out of " + wordPairs + " word pairs");
        } catch (IOException e){
            e.printStackTrace();
        }
    }
}

