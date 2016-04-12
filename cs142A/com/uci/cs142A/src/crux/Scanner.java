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
	private boolean on_comment;
	private Reader input;

	// munch states.
	private static final int EMPTY_STATE = 0;
	private static final int SINGLE_STATE = 1;
	private static final int DOUBLE_STATE = 2;

	
	Scanner(Reader reader)
	{
		this.input = reader;
		this.charPos = 1;
		this.lineNum = 1;
		this.nextChar = -1;
		this.lastChar = -1;
	}	
	
	private int readChar() {
		try {
			if (this.on_comment) {
				this.on_comment = false;
				// read until comment is over.
				int temp = this.input.read();
				while (temp != 10 && temp != -1) {
					temp = this.input.read();
				}

				temp = this.input.read();
				while (temp == 10) {
					this.lineNum++;
					temp = this.input.read();
					if (temp != 10)
						this.lastChar = temp;
				}
				if (temp != 10)
					this.lastChar = temp;
				this.lineNum++;
				this.charPos = 0;
			}
			if (this.lastChar == -1) {
				this.nextChar = this.input.read();
				this.charPos++;
				if (this.nextChar == 10) {
					this.lineNum++;
					this.charPos = 0;
				}
				if ((this.nextChar == 10) || ((char) this.nextChar == ' ')) return -1;
				else return this.nextChar == -1 ? -2 : this.nextChar;
			} else {
				this.nextChar = this.lastChar;
				this.lastChar = -1;
				return this.nextChar;
			}
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
		int munchSize = 0;
		StringBuilder buffer = new StringBuilder();

		// iterate until we either:
		//	1. Match a token in any state but MANY_STATE
		//	2. Whitespace, newline or EOF
		boolean was_good = false, can_continue = true;
		Token to_return = Token.generateToken(lineNum, charPos, "EOF", Token.Kind.EOF);

		// skip any initial white space.
		while (readChar() == -1) { }
		this.lastChar = this.nextChar;
		while (can_continue && readChar() > -1) {
			char next = (char)this.nextChar;
			int pos = (charPos - (buffer.length() + 1));
			switch (munchSize) {
				case EMPTY_STATE:
					// Search for all tokens of one token. If we match
					// store it, and attempt to switch states.
					if (Token.isValidLexeme(Token.getSingleCharacterMap(), String.valueOf(next), 0)) {
						// We can transition.
						buffer.append(next);
						to_return = Token.generateToken(lineNum, pos, buffer.toString(),
								Token.getSingleCharacterMap());
						munchSize++;
						was_good = true;
					} else {
						// error.
						was_good = false;
						buffer.append(next);
						munchSize++;
					}
					break;
				case SINGLE_STATE:
					// Search for all tokens of two tokens. If we match
					// store it, and attempt to switch states.
					if (Token.isValidLexeme(Token.getDoubleCharTokens(), buffer.toString() + next, 1)) {
						// we can transition
						buffer.append(next);
						to_return = Token.generateToken(lineNum, pos, buffer.toString(),
								Token.getDoubleCharTokens());
						munchSize++;
						was_good = true;

						// is this a comment?
						if (to_return.is(Token.Kind.COMMENT)) {
							// restart parse.
							this.on_comment = true;
							munchSize = 0;
							buffer.delete(0, buffer.length());
							was_good = false;
							can_continue = true;
							to_return = Token.generateToken(lineNum, pos, "EOF", Token.Kind.EOF);
						}
					} else {
						// if we had a previous good state, return, else error.
						can_continue = false;
						this.lastChar = this.nextChar;
						if (!was_good) {
							// error.
							was_good = false;
							to_return = Token.generateToken(lineNum, pos, "Unexpected token: " + buffer.toString(),
									Token.Kind.ERROR);
						}
					}
					break;
				case DOUBLE_STATE:
					// Search for multiple character states and keywords.
					if (Token.isValidLexeme(Token.getMultiCharTokens(), buffer.toString() + next, 2)) {
						// we can transition
						buffer.append(next);
						to_return = Token.generateToken(lineNum, pos, buffer.toString(),
								Token.getMultiCharTokens());
						munchSize++;
					} else {
						// if we had a previous good state, return, else error.
						can_continue = false;
						if (was_good) {
							this.lastChar = this.nextChar;
						} else {
							was_good = false;
							to_return = Token.generateToken(lineNum, pos, "Unexpected token: " + buffer.toString(),
									Token.Kind.ERROR);
						}
					}
					break;
				default: // MANY_STATE
					if (Token.isValidLexeme(Token.getKeywords(), buffer.toString() + next, munchSize)) {
						buffer.append(next);
						to_return = Token.generateToken(lineNum, pos, buffer.toString(), Token.getKeywords());
						munchSize++;
					} else {
						can_continue = false;
						if (was_good) {
							this.lastChar = this.nextChar;
						} else {
							was_good = false;
							to_return = Token.generateToken(lineNum, pos, "Unexpected token: " + buffer.toString(),
									Token.Kind.ERROR);
						}
					}
					break;
			}
		}

		return to_return;
	}

}
