package crux;
import java.io.IOException;
import java.io.Reader;
import java.util.Iterator;
import java.io.StringWriter;
import java.util.Stack;
import java.util.Set;

public class Scanner implements Iterable<Token> {
	public static String studentName = "Ian Schweer";
	public static String studentID = "ischweer";
	public static String uciNetID = "22514022";
	
	private int lineNum;  // current line count
	private int charPos;  // character offset for current line
	private int nextChar; // contains next char -1 == EOF
	private int lastChar;
	private Reader input;
	
	
	Scanner(Reader reader)
	{
		this.input = reader;
		this.charPos = 0;
		this.lineNum = 0;
		this.nextChar = -1;
	}	
	
	private int readChar() {
		try {
			this.nextChar = this.input.read();
			this.charPos++;
			if (this.nextChar == 10) {
				this.lineNum++;
			}
			if ((this.nextChar == 10) || ((char)this.nextChar == ' ')) return -1;
			else return this.nextChar;
		} catch (IOException e) {
			System.err.println("Failed to initialize reader");
			System.exit(-2);
			return -1;
		}
	}

	@Override
	public Iterator<Token> iterator() {
		return null;
	}
	/* Invariants:
	 *  1. call assumes that nextChar is already holding an unread character
	 *  2. return leaves nextChar containing an untokenized character
	 */
	public Token next()
	{
		Token.Kind currentState = null;
		Stack<Token.Kind> tokens = new Stack<>();
		StringWriter lexeme = new StringWriter();
		int pos = 0;
		final int lineNumber = this.lineNum;
		for (Token.Kind c : Token.Kind.values()) {
			tokens.push(c);
		}

		while (tokens.size() > 0) {
			// ------ Begin scan
			if (readChar() == -1) break;
			lexeme.append((char)this.nextChar);
			Stack<Token.Kind> answerSet = new Stack<>();
			while (tokens.empty() == false) {
				Token.Kind k = tokens.pop();
				if (k.checkChar((char)this.nextChar, pos)) {
					answerSet.push(k);
				}

				if (currentState == null && k.matches(lexeme.toString())) {
					currentState = k;
				}
			}
			if (currentState == null) {
				currentState = Token.Kind.ERROR;
				this.lastChar = this.nextChar;
				break;
			}
			// trim the answer set based off our current state.
			Set<Token.Kind> nextStates = Token.possibleTransitions(currentState);
			answerSet.retainAll(nextStates);
			tokens = answerSet;
			currentState = null;
			pos++;
		}

		return new Token(lexeme.toString(), lineNumber, this.charPos);
	}
}
