package crux;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.io.PrintWriter;

public class AutoTester {
    public static String studentName = "Auto Tester"; // !Strictly! You should put these value in the rest of other classes
    // studentID is worth for 10 points!!!
    public static String studentID = "12345678"; // !Strictly! You should put these value in the rest of other classes
    public static String uciNetID = "autotester6";	// !Strictly! You should put these value in the rest of other classes
    
    public static final int PUBTESTS_NUM = 11;
    public static final int PRITESTS_NUM = 10;
    public static double PUB_WEIGHT = 5.0;
    public static double PRI_WEIGHT = 3.5;
    public static double BASE_SCORE = 10.0;
    public static final String ROSTER = "roster.csv";
    
    public static PrintWriter __out;  
    public static PrintWriter __outSummary;
    public static String __summary = "";
    public static String __comment = "\"Failed tests: ";
    public static double __score = 0.0;
    
    
    /*
     * NOTE: If your code crashes in the middle of the execution, this will be treated as a FAIL!
     */
    public static void main(String[] args)
    {
    	AutoTester.studentID = Parser.studentID;
    	AutoTester.studentName = Parser.studentName;
    	AutoTester.uciNetID = Parser.uciNetID;
		try {
			__out = new PrintWriter(new BufferedWriter(new FileWriter(ROSTER, true)));
			__outSummary = new PrintWriter("Output/"+studentID+".txt", "UTF-8");
		
		} catch (Exception e) {
			e.printStackTrace();
			System.exit(-2);
		}
		if (AutoTester.studentID == "" || AutoTester.studentID == null) {
			System.out.println("No student ID!");
			System.exit(-1);
		}
    	System.out.println("Public tests begin...");
    	// Put the results in the folder
    	for (int i=1; i<= PUBTESTS_NUM; ++i) {
    		String sourceFilename = String.format("tests/public/test%02d.crx", i);
    		fileCompile(sourceFilename);
    	}
//    	System.out.println("Private tests begin...");
//    	// Put the results in the folder
//    	for (int i=1; i<= PRITESTS_NUM; ++i) {
//    		String sourceFilename = String.format("tests/public/test%02d.crx", i);
//    		fileCompile(sourceFilename);
//    	}
    	
    	for (int i=1; i<= PUBTESTS_NUM; ++i) {
    		String sourceFilename = String.format("tests/public/test%02d.out", i);
    		compareAndGrade(sourceFilename,PUB_WEIGHT,i);
    	}
//    	for (int i=1; i<= PRITESTS_NUM; ++i) {
//    		String sourceFilename = String.format("tests/public/testP%02d.crx", i);
//    		compareAndGrade(sourceFilename,PRI_WEIGHT,i);
//    	}
    	__comment += "\"";
    	
    	__out.print(AutoTester.studentID+"\t\""+AutoTester.studentName+"\"\t\""+AutoTester.uciNetID+"\"\t");
    	AutoTester.__score += BASE_SCORE;
		__out.print(AutoTester.__score+"\t");
		__out.print(AutoTester.__comment+"\t");
		__out.println();
		__out.flush();
		__out.close();
		__outSummary.flush();
		__outSummary.close();
    }
           
    public static void compareAndGrade(String fileName, double weight, int testNum) {
    	System.out.print("Test " + testNum + ": ");
    	String outputFilename = fileName;
    	String expectedFilename= fileName.replace(".out", ".out_actual");
    	Process diff;
		try {
			diff = Runtime.getRuntime().exec("diff " + outputFilename + " " + expectedFilename);
			Thread.sleep(100);
			BufferedReader diffout = new BufferedReader(new InputStreamReader(diff.getInputStream()));
			String tmpRsl = "";
			while(diffout.ready()) {
				tmpRsl += (char)diffout.read();
			}
			if (tmpRsl.equals("")||tmpRsl == null) {
				System.out.println("Passed!");
				__score += weight;
			} else {
				System.out.println("Failed!");
				__comment += testNum+", ";
				__outSummary.println(tmpRsl);
			}
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    }
    public static void fileCompile(String sourceFilename)
    {
    	System.out.println("outputing for " + sourceFilename);
        
        Scanner s = null;
        try {
            s = new Scanner(new FileReader(sourceFilename));
        } catch (IOException e) {
            e.printStackTrace();
            System.err.println("Error accessing the source file: \"" + sourceFilename + "\"");
            System.exit(-2);
        }

        Parser p = new Parser(s);
        p.parse();

        String outFilename = sourceFilename.replace(".crx", ".out_actual");
        try {
            File outFile = new File(outFilename);
            PrintStream outStream = new PrintStream(outFile);
            if (p.hasError()) {
                outStream.println("Error parsing file.");
                outStream.println(p.errorReport());
                outStream.close();
                //System.exit(-3);
            }
            outStream.println("Crux program successfully parsed.");
            outStream.close();
        } catch (IOException e) {
            e.printStackTrace();
            System.err.println("Error creating output file: \"" + outFilename + "\"");
            System.exit(-6);
        }
    }
}
    
