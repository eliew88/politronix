import java.util.*;
/**
 * Test for TextAnalysis class
 *
 * @author Eli Echt-Wilson
 */
public class TextAnalysisTests
{
    public static void main(String[] args) {
        testByProximity();
    }

    public static void testBySpeaker()
    {
        TextAnalysis test = new TextAnalysis(false); //creates an object to analyze the file by speaker
        test.addLearningWordsBySpeaker( "input/republican_debate_miami_2016.txt" ); //analyzes the file
        test.addLearningWordsBySpeaker( "input/republican_debate_milwaukee_2016.txt" );
        test.addLearningWordsBySpeaker( "input/republican_debate_simi_2016.txt" );
        test.addLearningWordsBySpeaker( "input/republican_debate_boulder_2016.txt" );
        test.addLearningWordsBySpeaker( "input/republican_debate_cleveland_2016.txt" );
        test.learnBySpeaker("correlationsSpeaker0.4.txt", 0.2);
        //test.loopThroughPairs("correlationsSpeaker0.4.txt", 0.4);
        test.loopThroughPairs("correlationsSpeaker0.6.txt", 0.6);
        test.loopThroughPairs("correlationsSpeaker0.8.txt", 0.8);
    }

    public static void testByProximity()
    {
        TextAnalysis test = new TextAnalysis(true); //creates an object to analyze the file by proximity
        test.addLearningWordsByProximity( "input/republican_debate_miami_2016.txt" ); //analyzes the file
        test.addLearningWordsByProximity( "input/republican_debate_milwaukee_2016.txt" );
        test.addLearningWordsByProximity( "input/republican_debate_simi_2016.txt" );
        test.addLearningWordsByProximity( "input/republican_debate_cleveland_2016.txt" );
        test.addLearningWordsByProximity( "input/republican_debate_boulder_2016.txt" );
        test.addLearningWordsByProximity( "input/republican_debate_charleston_2016.txt" );
        test.addLearningWordsByProximity( "input/republican_debate_desmoines_2016.txt" );
        test.addLearningWordsByProximity( "input/republican_debate_greenville_2016.txt" );
        test.addLearningWordsByProximity( "input/republican_debate_houston_2016.txt" );
        test.addLearningWordsByProximity( "input/republican_debate_lasvegas_2016.txt" );
        test.addLearningWordsByProximity( "input/republican_debate_manchester_2016.txt" );
        test.addLearningWordsByProximity( "input/republican_debate_detroit_2016.txt" );
        test.learnByProximity("correlationsProximity0.2.txt", 0.2);
        test.loopThroughPairs("correlationsProximity0.4.txt", 0.4);
        test.loopThroughPairs("correlationsProximity0.6.txt", 0.6);
        test.loopThroughPairs("correlationsProximity0.8.txt", 0.8);
    }

    public static void writeMapReduceInputSpeaker()
    {
        TextAnalysis test = new TextAnalysis(false); //creates an object to analyze the file by speaker
        test.addLearningWordsBySpeaker( "input/republican_debate_miami_2016.txt" ); //analyzes the file
        test.addLearningWordsBySpeaker( "input/debate2.txt" );
        test.addLearningWordsBySpeaker( "input/debate3.txt" );
        test.addLearningWordsBySpeaker( "input/VPdebate.txt" );
        test.countWordsInBags();
        test.writeBagsToFile( "input/mapReduceInput.txt" );
    }

    public static void writeMapReduceInputProximity()
    {
        TextAnalysis test = new TextAnalysis(true); //creates an object to analyze the file by proximity
        test.addLearningWordsByProximity( "input/republican_debate_miami_2016.txt" ); //analyzes the file
        test.addLearningWordsByProximity( "input/debate2.txt" );
        test.addLearningWordsByProximity( "input/debate3.txt" );
        test.addLearningWordsByProximity( "input/VPdebate.txt" );
        test.addLearningWordsByProximity( "input/obamasou2010.txt" );
        test.addLearningWordsByProximity( "input/obamasou2013.txt" );
        test.addLearningWordsByProximity( "input/bushsou2008.txt" );
        test.addLearningWordsByProximity( "input/obamainaug2013.txt" );
        test.addLearningWordsByProximity( "input/romneyforeignpolicy.txt" );
        test.addLearningWordsByProximity( "input/romneyacceptance2012.txt" );
        test.addLearningWordsByProximity( "input/romneyeconomy.txt" );
        test.addLearningWordsByProximity( "input/RepubsMesa2012.txt" );
        test.addLearningWordsByProximity( "input/RepubsJacksonville2012.txt" );
        test.addLearningWordsByProximity( "input/RepubsTampa2012.txt" );
        test.addLearningWordsByProximity( "input/RepubsCharleston2012.txt" );
        test.addLearningWordsByProximity( "input/RepubsMyrtleBeach2012.txt" );
        test.addLearningWordsByProximity( "input/RepubsConcord2012.txt" );
        test.addLearningWordsByProximity( "input/RepubsManchester2012.txt" );
        test.addLearningWordsByProximity( "input/RepubsSiouxCity2012.txt" );
        test.addLearningWordsByProximity( "input/RepubsDesMoines2012.txt" );
        test.addLearningWordsByProximity( "input/RepubsWashingtonDC2012.txt" );
        test.countWordsInProximity();
        test.writeBagsToFile( "input/mapReduceInput1.txt" );
        test.countAllWords();
        test.writeAllWordsToFile("input/allWords.txt");
    }

    public static void writeMulitpleInputFiles()
    {
        TextAnalysis test = new TextAnalysis(true); //creates an object to analyze the file by proximity
        test.addLearningWordsByProximity( "input/republican_debate_miami_2016.txt" ); //analyzes the file
        test.addLearningWordsByProximity( "input/debate2.txt" );
        test.addLearningWordsByProximity( "input/debate3.txt" );
        test.addLearningWordsByProximity( "input/VPdebate.txt" );
        test.addLearningWordsByProximity( "input/obamasou2010.txt" );
        test.addLearningWordsByProximity( "input/obamasou2013.txt" );
        test.addLearningWordsByProximity( "input/bushsou2008.txt" );
        test.addLearningWordsByProximity( "input/obamainaug2013.txt" );
        test.addLearningWordsByProximity( "input/romneyforeignpolicy.txt" );
        test.addLearningWordsByProximity( "input/romneyacceptance2012.txt" );
        test.addLearningWordsByProximity( "input/romneyeconomy.txt" );
        test.addLearningWordsByProximity( "input/RepubsMesa2012.txt" );
        test.addLearningWordsByProximity( "input/RepubsJacksonville2012.txt" );
        test.addLearningWordsByProximity( "input/RepubsTampa2012.txt" );
        test.addLearningWordsByProximity( "input/RepubsCharleston2012.txt" );
        test.addLearningWordsByProximity( "input/RepubsMyrtleBeach2012.txt" );
        test.addLearningWordsByProximity( "input/RepubsConcord2012.txt" );
        test.addLearningWordsByProximity( "input/RepubsManchester2012.txt" );
        test.addLearningWordsByProximity( "input/RepubsSiouxCity2012.txt" );
        test.addLearningWordsByProximity( "input/RepubsDesMoines2012.txt" );
        test.addLearningWordsByProximity( "input/RepubsWashingtonDC2012.txt" );
        test.countWordsInProximity();
        test.writeBagsToMultipleFiles( 5, "HadoopInput" );
        test.writeBagsToMultipleFiles( 20, "HadoopInput20" );
    }
}
