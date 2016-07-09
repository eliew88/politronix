import java.util.*;
/**
 * A class to represent a question and its answer.
 */
public class QuestionAnswer
{
    private ArrayList<String> question;
    private ArrayList<String> answer;

    public QuestionAnswer()
    {
        question = new ArrayList<String>();
        answer = new ArrayList<String>();
    }

    public ArrayList<String> getQuestion()
    {
        return question;
    }

    public ArrayList<String> getAnswer()
    {
        return answer;
    }

    public void print()
    {
        System.out.print("Question: ");
        for (String word : question)
            System.out.print(word + " ");
        System.out.println();
        System.out.print("Answer: ");
        for (String word: answer)
            System.out.print(word + " ");
        System.out.println();
    }

}
