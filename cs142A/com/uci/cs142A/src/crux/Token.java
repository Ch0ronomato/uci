package crux;

import java.util.HashMap;

public class Token {
	public static final String VALID_CHARS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
	public static enum Kind {
		AND("and", true),
		OR("or", true),
		NOT("not", true),
		LET("let", true),
		VAR("var", true),
		ARRAY("array", true),
		FUNC("func", true),
		IF("if", true),
		ELSE("else", true),
		WHILE("while", true),
		TRUE("true", true),
		FALSE("false", true),
		RETURN("return", true),
		OPEN_PAREN("("),
		CLOSE_PAREN(")"),
		OPEN_BRACE("{"),
		CLOSE_BRACE("}"),
		OPEN_BRACKET("["),
		CLOSE_BRACKET("]"),
		ADD("+"),
		SUB("-"),
		MUL("*"),
		DIV("/"),
		COMMENT("//"),
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
		private boolean is_keyword;
		Kind()
		{
			default_lexeme = "";
			is_keyword = true;
		}
		
		Kind(String lexeme)
		{
			is_keyword = false;
			default_lexeme = lexeme;
		}

		Kind(String lexeme, boolean isKeyword)
		{
			default_lexeme = lexeme;
			this.is_keyword = isKeyword;
		}

		public boolean hasStaticLexeme()
		{
			return default_lexeme != null;
		}

		public boolean is_keyword() {
			return is_keyword;
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
	private static HashMap<String, Token.Kind> singleCharTokens = new HashMap<>();
	private static HashMap<String, Token.Kind> doubleCharTokens = new HashMap<>();
	private static HashMap<String, Token.Kind> multiCharTokens = new HashMap<>();
	private static HashMap<String, Token.Kind> keywords = new HashMap<>();

	public Token(int lineNum, int charPos)
	{
		this.lineNum = lineNum;
		this.charPos = charPos;
		
		// if we don't match anything, signal error
		this.kind = Kind.ERROR;
		this.lexeme = "No Lexeme Given";
	}

	public static Token generateToken(int lineNum, int charPos, String lexeme, Kind type) {
		Token token = new Token(lineNum, charPos);
		token.lexeme = lexeme;
		token.kind = type;
		return token;
	}

	public static Token generateToken(int lineNum, int charPos, String lexeme, HashMap<String, Kind> map) {
		Token token;
		if (map.containsKey(lexeme)) {
			token = new Token(lineNum, charPos);
			token.lexeme = lexeme;
			token.kind = map.get(lexeme);
		} else {
			token = generateIdentifierOrNumber(lexeme, lineNum, charPos);
		}
		return token;
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
		return String.format("%s(%s)(lineNum: %d, charPos: %d)", this.lexeme(), this.lexeme,
				this.lineNumber(), this.charPosition());
	}
	
	public boolean is(Token.Kind kind) {
		return this.kind == kind;
	}

	public static HashMap<String, Token.Kind> getSingleCharacterMap() {
		if (singleCharTokens.isEmpty()) {
			// fill map.
			for (Token.Kind k : Token.Kind.values())
				if (k.getLexeme().length() == 1)
					singleCharTokens.put(k.getLexeme(), k);
		}

		return singleCharTokens;
	}

	public static HashMap<String, Token.Kind> getDoubleCharTokens() {
		if (doubleCharTokens.isEmpty()){
			// fill
			for (Token.Kind k : Token.Kind.values())
				if (k.getLexeme().length() == 2)
					doubleCharTokens.put(k.getLexeme(), k);
		}

		return doubleCharTokens;
	}

	public static HashMap<String, Token.Kind> getMultiCharTokens() {
		if (multiCharTokens.isEmpty()){
			// fill
			for (Token.Kind k : Token.Kind.values())
				if (k.getLexeme().length() == 2 || k.getLexeme().length() == 3)
					multiCharTokens.put(k.getLexeme(), k);
		}

		return multiCharTokens;
	}

	public static HashMap<String, Token.Kind> getKeywords() {
		if (keywords.isEmpty()){
			// fill
			for (Token.Kind k : Token.Kind.values())
				if (k.is_keyword())
					keywords.put(k.getLexeme(), k);
		}

		return keywords;
	}

	public static Token generateIdentifierOrNumber(String lexeme, int lineNum, int charPos) {
		switch(isValidIdentifierNumberOrFloat(lexeme)) {
			case 1:
				return generateToken(lineNum, charPos, lexeme, Kind.FLOAT);
			case 2:
				return generateToken(lineNum, charPos, lexeme, Kind.INTEGER);
			case 3:
				return generateToken(lineNum, charPos, lexeme, Kind.IDENTIFIER);
			default:
				return null;
		}
	}

	private static boolean hasAnyNumbers(String lexeme) {
		boolean good = false;
		for (char c : lexeme.toCharArray())
			good |= (int)(c - '0') < 10 && ((int)(c - '0')) > -1;
		return good;
	}

	private static boolean hasNumbers(String lexeme) {
		boolean good = true;
		for (char c : lexeme.toCharArray()) {
			good &= (int)(c - '0') < 10 && ((int)(c - '0')) > -1;
		}
		return good;
	}

	private static boolean hasLetters(String lexeme) {
		boolean good = false;
		for (char c : lexeme.toCharArray()) {
			good |= VALID_CHARS.indexOf(c) > -1;
		}
		return good;
	}

	private static boolean hasSpecialChars(String lexeme) {
		boolean good = true;
		for (char c : lexeme.toCharArray())
			good &= !"(){}[]+-*/><=!,;.:".contains(String.valueOf(c));
		return good;
	}

	private static int isValidIdentifierNumberOrFloat(String lexeme) {
		if (!hasLetters(lexeme)) {
			if (lexeme.charAt(0) != '.' && lexeme.contains(".")
					&& lexeme.lastIndexOf('.') == lexeme.indexOf('.')) // ensure there is only one dot.
				return 1;
			else if (hasNumbers(lexeme)) return 2;
			else return 0;
		}
		else {
			if (hasSpecialChars(lexeme) && !hasAnyNumbers(lexeme)) return 3;
			else return 0;
		}
	}

	public static boolean isValidLexeme(HashMap<String, Token.Kind> mapping, String lexeme, int length) {
		if (mapping.containsKey(lexeme)) return true;
		if (isValidIdentifierNumberOrFloat(lexeme) > 0) return true;
		else return false;
	}
}
