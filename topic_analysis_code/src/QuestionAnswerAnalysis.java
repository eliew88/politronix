import java.util.*;
import java.io.*;
import java.lang.Object;
/**
 * This class attempts to rate how well a speaker answers the question. It does this by creating word pairs
 * from words in the Q's and A's and looking for their correlations
 * in the file written by TextAnalysis.
 */
public class QuestionAnswerAnalysis
{
    ArrayList<Pair> correlatedPairs;
    // also store the correlations in a map for fast lookup times. Key: "word1, word2". Value: correlation.
    HashMap<String, Double> pairCorrelations = new HashMap<String, Double>();
    // a set of words that have a correlation with at least one other word of at least meaningfulThreshold.
    HashSet<String> meaningfulWords;
    // each word must have at least this correlation with some other word to be considered "meaningful"
    double meaningfulThreshold;
    // the question and answer pairs that we are going to analyze.
    ArrayList<QuestionAnswer> qAndAs = new ArrayList<QuestionAnswer>();
    TextUtils tu = new TextUtils();

    QuestionAnswerAnalysis(double meaningful)
    {
        meaningfulThreshold = meaningful;
    }

    //takes the correlations from fileName and stores them in correlatedPairs,
    // meaningfulWords(if corr is high enough), and pairCorrelations.
    public void addCorrelations( String fileName )
    {
        correlatedPairs = new ArrayList<Pair>();
        meaningfulWords= new HashSet<String>();
        Scanner scan = getFileScanner(fileName);
        while (scan.hasNext())
        {
            String word1 = scan.next();
            String word2 = scan.next();
            double correlation = scan.nextDouble();
            Pair newPair = new Pair(word1, word2, correlation);
            correlatedPairs.add(newPair);
            if (correlation >= meaningfulThreshold)
            {
                meaningfulWords.add(word1);
                meaningfulWords.add(word2);
            }
            String bothWords = word1 + ", " + word2;
            pairCorrelations.put(bothWords, correlation);
        }
        System.out.println("Read " + correlatedPairs.size() + " correlated word pairs from " + fileName + ".");
        System.out.println("This number of words is considered meaningful: " + meaningfulWords.size());
    }

    // analyzes the questions and answers from an input fleName
    public void analyzeQAndA( String fileName )
    {
        Scanner scan = getFileScanner(fileName);
        readQuestionsAndAnswers(scan);
        for (QuestionAnswer qa: qAndAs) {
            analyzeQA(qa);
        }
    }

    // prints the Q and A, and analyzes the correlation between the meaningful words in the question and the meaningful words in
    // the answer. If there is no found correlation, 0.0 is used. The method averages these correlations to provide an overall
    // rating of how well the question topic is related to the answer topic.
    public void analyzeQA( QuestionAnswer qa )
    {
        qa.print();
        ArrayList<String> questionWords = getImportantWords(qa.getQuestion());
        ArrayList<String> answerWords = getImportantWords(qa.getAnswer());
        double sum = 0;
        int count = 0;
        for (String qWord: questionWords)
        {
            for (String aWord: answerWords)
            {
                count++;
                sum += lookUpCorrelation(qWord, aWord);
            }
        }
        System.out.println("Question meaningful words: " + questionWords);
        System.out.println("Answer meaningful words: " + answerWords);
        double evaluation = sum / count;
        System.out.println(evaluation);
    }

    // looks up the correlation between word1 and word2. If no correlation is found, returns 0.0.
    public double lookUpCorrelation( String word1, String word2 )
    {
        String bothWords = word1 + ", " + word2;
        String bothWords2 = word2 + ", " + word1;
        if (word1.equals(word2))
            return 1.0;
        if (pairCorrelations.containsKey(bothWords))
            return pairCorrelations.get(bothWords);
        if (pairCorrelations.containsKey(bothWords2))
            return pairCorrelations.get(bothWords2);
        return 0.0;
    }

    // returns a list of the meaningful words in inWords
    public ArrayList<String> getImportantWords( ArrayList<String> inWords )
    {
        ArrayList<String> outWords = new ArrayList<String>();
        for (String word: inWords)
        {
            if (meaningfulWords.contains(word))
                outWords.add(word);
        }
        return outWords;
    }

    // Reads and questions and answers from the scanner. Only finds a question and answer in some cases.
    public void readQuestionsAndAnswers(Scanner text)
    {
        Set<String> questionStartWords = new HashSet<String>(Arrays.asList(
                "lehrer:", "question:", "crowley:", "schieffer:"));
        Set<String> answerStartWords = new HashSet<String>(Arrays.asList(
                "obama:", "romney:", "answer:"));
        final int NONE = 0;
        final int QUESTION = 1;
        final int ANSWER = 2;
        int state = NONE;
        QuestionAnswer qa = null;
        while(text.hasNext())
        {
            String word = text.next();
            String lower = word.toLowerCase();
            if (questionStartWords.contains(lower))
            {
                if (state == QUESTION)
                {
                    qa = new QuestionAnswer();
                }
                else
                {
                    if (qa != null && qa.getQuestion().size() > 10  && qa.getAnswer().size() > 10)
                    {
                        qAndAs.add(qa);
                    }
                    qa = new QuestionAnswer();
                }
                state = QUESTION;
            } else if (answerStartWords.contains(lower))
            {
                if (state == NONE || state == ANSWER)
                {
                    state = NONE;
                } else {
                    state = ANSWER;
                }
            } else {
                String cleanWord = tu.cleanWord(lower);
                if (!cleanWord.isEmpty())
                {
                    if (state == QUESTION) {
                        qa.getQuestion().add(cleanWord);
                    } else if (state == ANSWER) {
                        qa.getAnswer().add(cleanWord);
                    }
                }
            }
        }
        System.out.println("Read " + qAndAs.size() + " question and answer pairs.");
    }

    public static Scanner getFileScanner( String filename )
    {
        try {
            return new Scanner( new File(filename) );
        } catch (IOException ex)
        { throw new RuntimeException(ex); }
    }
}

