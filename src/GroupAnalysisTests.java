/**
 * This class tests our group analysis and prints the results.
 *
 * @author Eli Echt-Wilson
 */
public class GroupAnalysisTests
{
    public static void main(String[] args) {
        testBasic();
    }

    public static void testBasic()
    {
        GroupAnalysis test02 = new GroupAnalysis("correlationsProximity0.2.txt");
        //GroupAnalysis test04 = new GroupAnalysis("correlationsProximity0.4.txt");
        //GroupAnalysis test06 = new GroupAnalysis("correlationsProximity0.6.txt");
        //GroupAnalysis test08 = new GroupAnalysis("correlationsProximity0.8.txt");
        test02.groupWords();
        System.out.println("Groups with correlation threshold of 0.2:");
        test02.printGroups();
//        System.out.println();
//        test04.groupWords();
//        System.out.println("Groups with correlation threshold of 0.4:");
//        test04.printGroups();
//        System.out.println();
//        test06.groupWords();
//        System.out.println("Groups with correlation threshold of 0.6:");
//        test06.printGroups();
//        System.out.println();
//        test08.groupWords();
//        System.out.println("Groups with correlation threshold of 0.8:");
//        test08.printGroups();
    }
}

