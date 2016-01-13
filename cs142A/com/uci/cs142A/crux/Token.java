package crux;
import java.util.Set;
import java.util.HashSet;
public class Token {
	
	public static enum Kind {
		AND("and"),
		OR("or"),
		NOT("not"),
		LET("let"),
		VAR("var"),
		ARRAY("array"),
		FUNC("func"),
		IF("if"),
		ELSE("else"),
		WHILE("while"),
		TRUE("true"),
		FALSE("false"),
		RETURN("return"),
		OPEN_PAREN("("),
		CLOSE_PAREN(")"),
		OPEN_BRACE("{"),
		CLOSE_BRACE("}"),
		OPEN_BRACKET("["),
		CLOSE_BRACKET("["),
		ADD("+"),
		SUB("-"),
		MUL("*"),
		DIV("/"),
		GREATER_EQUAL(">="),
		LESSER_EQUAL("<="),
		NOT_EQUAL("!="),
		EQUAL("=="),
		GREATER_THAN(">"),
		LESS_THAN("<"),
		ASSIGN("="),
		COMMA(","),
		SEMICOLON(";"),
		COLON(":"),
		CALL("::"),
		IDENTIFIER(),
		INTEGER(),
		FLOAT(),
		ERROR(),
		EOF();

		private String default_lexeme;

		Kind()
		{
			default_lexeme = "";
		}
		
		Kind(String lexeme)
		{
			default_lexeme = lexeme;
		}
		
		public boolean hasStaticLexeme()
		{
			return default_lexeme != null;
		}

		public String getLexeme() {
			return default_lexeme;
		}

		public boolean matches(String lexeme) {
			return lexeme.equals(default_lexeme);
		}

		public boolean checkChar(char c, int i) {
			return i < default_lexeme.length() && default_lexeme.charAt(i) == c;
		}
	}
	
	private int lineNum;
	private int charPos;
	Kind kind;
	private String lexeme = "";

	private Token(int lineNum, int charPos)
	{
		this.lineNum = lineNum;
		this.charPos = charPos;
		
		// if we don't match anything, signal error
		this.kind = Kind.ERROR;
		this.lexeme = "No Lexeme Given";
	}
	
	public Token(String lexeme, int lineNum, int charPos)
	{
		this.lineNum = lineNum;
		this.charPos = charPos;
		this.kind = Kind.ERROR;
		this.lexeme = "Unrecognized lexeme: " + lexeme;

		// Determine lexeme, if there is one.
		for (Kind k : Kind.values()) {
			if (k.matches(lexeme)) {
				this.kind = k;
				this.lexeme = lexeme;
			}
		}
	}

	public Token(String lexeme, int lineNum, int charPos, Kind tokenType) {
		this.lexeme = lexeme;
		this.lineNum = lineNum;
		this.charPos = charPos;
		this.kind = tokenType;
	}
	
	public int lineNumber()
	{
		return lineNum;
	}
	
	public int charPosition()
	{
		return charPos;
	}
	
	// Return the lexeme representing or held by this token
	public String lexeme()
	{
		return this.kind.name();
	}
	
	public String toString()
	{
		return String.format("%s(lineNum: %d, charPos: %d)", this.lexeme(), this.lineNumber(), this.charPosition()); 
	}
	
	public boolean is(Token.Kind kind) {
		return this.kind == kind;
	}

	private static Token generateTokenFactory(int lineNum, int charPos, Kind kind) {
		Token token = new Token(lineNum, charPos);
		token.kind = kind;
		return token;
	}

	private static Token generateTokenFactory(String identifier, int lineNum, int charPos, Kind kind) {
		Token token = new Token(lineNum, charPos);
		token.kind = kind;
		token.lexeme = identifier;
		return token;
	}

	public static Token createIdentifierToken(String identifier, int lineNum, int charPos) {
		return generateTokenFactory(identifier, lineNum, charPos, Token.Kind.IDENTIFIER);
	}

	public static Token createIntegerToken(String identifier, int lineNum, int charPos) {
		return generateTokenFactory(identifier, lineNum, charPos, Kind.INTEGER);
	}

	public static Token createFloatToken(String identifier, int lineNum, int charPos) {
		return generateTokenFactory(identifier, lineNum, charPos, Kind.FLOAT);
	}

	public static Token createErrorToken(String identifier, int lineNum, int charPos) {
		return generateTokenFactory(identifier, lineNum, charPos, Kind.ERROR);	
	}

	public static Token createEOFToken(int lineNum, int charPos) {
		return generateTokenFactory(lineNum, charPos, Kind.EOF);
	}

	public static Set<Kind> possibleTransitions(Kind currentKind) {
		Set<Kind> states = new HashSet<>();
		if (currentKind == null) return states;
		for (Kind c : Kind.values())
			if (c != currentKind && c.getLexeme().startsWith(currentKind.getLexeme())) {
				states.add(c);
			}
		return states;
	}
}
