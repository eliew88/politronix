import java.util.*;
/**
 * Tests for question and answer analysis.
 */
public class QuestionAnswerTests
{
    public static void testBasic()
    {
        String textFile1 = "_debate1.txt"; //uses the text file found in project folder
        String textFile2 = "_debate2.txt";
        String textFile3 = "_debate3.txt";
        QuestionAnswerAnalysis test = new QuestionAnswerAnalysis(0.5); //creates an object to analyze the file
        test.addCorrelations("correlationsSpeaker0.2.txt");
        test.analyzeQAndA("_debate3.txt");
    }

    public static void testSurveyLearnedBySpeaker()
    {
        String surveyFile = "_surveyFile.txt";
        QuestionAnswerAnalysis test = new QuestionAnswerAnalysis(0.5); //creates an object to analyze the file
        test.addCorrelations("correlationsSpeaker0.2.txt");
        test.analyzeQAndA(surveyFile);
    }

    public static void testSurveyLearnedByProximity()
    {
        String surveyFile = "_surveyFile.txt";
        QuestionAnswerAnalysis test = new QuestionAnswerAnalysis(0.2); //creates an object to analyze the file
        test.addCorrelations("correlationsProximity0.2.txt");
        test.analyzeQAndA(surveyFile);
    }
}

