package crux;

import java.util.ArrayList;
import java.util.List;
import java.util.Stack;

public class Parser {
    public static String studentName = "Ian Schweer";
    public static String studentID = "22514022";
    public static String uciNetID = "ischweer";
    // SymbolTable Management ==========================
    private SymbolTable symbolTable;

    private void initSymbolTable()
    {
        symbolTable = new SymbolTable();
        symbolTable.insert("readInt");
        symbolTable.insert("readFloat");
        symbolTable.insert("printBool");
        symbolTable.insert("printInt");
        symbolTable.insert("printFloat");
        symbolTable.insert("println");
    }

    private void enterScope()
    {
        symbolTable = new SymbolTable(symbolTable);
    }

    private void exitScope()
    {
        symbolTable = symbolTable.getParent();
    }

    private Symbol tryResolveSymbol(Token ident)
    {
        assert(ident.is(Token.Kind.IDENTIFIER));
        String name = ident.lexeme();
        try {
            return symbolTable.lookup(name);
        } catch (SymbolNotFoundError e) {
            String message = reportResolveSymbolError(name, ident.lineNumber(), ident.charPosition());
            return new ErrorSymbol(message);
        }
    }

    private String reportResolveSymbolError(String name, int lineNum, int charPos)
    {
        String message = "ResolveSymbolError(" + lineNum + "," + charPos + ")[Could not find " + name + ".]";
        errorBuffer.append(message + "\n");
        errorBuffer.append(symbolTable.toString() + "\n");
        return message;
    }

    private Symbol tryDeclareSymbol(Token ident)
    {
        assert(ident.is(Token.Kind.IDENTIFIER));
        String name = ident.lexeme();
        try {
            return symbolTable.insert(name);
        } catch (RedeclarationError re) {
            String message = reportDeclareSymbolError(name, ident.lineNumber(), ident.charPosition());
            return new ErrorSymbol(message);
        }
    }

    private String reportDeclareSymbolError(String name, int lineNum, int charPos)
    {
        String message = "DeclareSymbolError(" + lineNum + "," + charPos + ")[" + name + " already exists.]";
        errorBuffer.append(message + "\n");
        errorBuffer.append(symbolTable.toString() + "\n");
        return message;
    }

// Helper Methods ==========================================

    private Token expectRetrieve(Token.Kind kind)
    {
        Token tok = currentToken;
        if (accept(kind))
            return tok;
        String errorMessage = reportSyntaxError(kind);
        throw new QuitParseException(errorMessage);
        //return ErrorToken(errorMessage);
    }

    private Token expectRetrieve(NonTerminal nt)
    {
        Token tok = currentToken;
        if (accept(nt))
            return tok;
        String errorMessage = reportSyntaxError(nt);
        throw new QuitParseException(errorMessage);
        //return ErrorToken(errorMessage);
    }
    
// Grammar Rule Reporting ==========================================
    private StringBuffer parseTreeBuffer = new StringBuffer();
    public String parseTreeReport()
    {
        return parseTreeBuffer.toString();
    }

// Error Reporting ==========================================
    private StringBuffer errorBuffer = new StringBuffer();
    
    private String reportSyntaxError(NonTerminal nt)
    {
        String message = "SyntaxError(" + lineNumber() + "," + charPosition() + ")[Expected a token from " + nt.name() + " but got " + currentToken.kind() + ".]";
        errorBuffer.append(message + "\n");
        return message;
    }
     
    private String reportSyntaxError(Token.Kind kind)
    {
        String message = "SyntaxError(" + lineNumber() + "," + charPosition() + ")[Expected " + kind + " but got " + currentToken.kind() + ".]";
        errorBuffer.append(message + "\n");
        return message;
    }
    
    public String errorReport()
    {
        return errorBuffer.toString();
    }
    
    public boolean hasError()
    {
        return errorBuffer.length() != 0;
    }
    
    private class QuitParseException extends RuntimeException
    {
        private static final long serialVersionUID = 1L;
        public QuitParseException(String errorMessage) {
            super(errorMessage);
        }
    }
    
    private int lineNumber()
    {
        return currentToken.lineNumber();
    }
    
    private int charPosition()
    {
        return currentToken.charPosition();
    }
          
// Parser ==========================================
    private Scanner scanner;
    private Token currentToken;
    
    public Parser(Scanner _scanner)
    {
        scanner = _scanner;
    }
    
    public void parse()
    {
        try {
            // begin!
            currentToken = scanner.next();
            program();
        } catch (QuitParseException q) {
            errorBuffer.append("SyntaxError(" + lineNumber() + "," + charPosition() + ")");
            errorBuffer.append("[Could not complete parsing.]");
        }
    }
    
// Helper Methods ==========================================
    private boolean have(Token.Kind kind)
    {
        return currentToken.is(kind);
    }
    
    private boolean have(NonTerminal nt)
    {
        return nt.firstSet().contains(currentToken.kind());
    }

    private boolean accept(Token.Kind kind)
    {
        if (have(kind)) {
            currentToken = scanner.next();
            return true;
        }
        return false;
    }    
    
    private boolean accept(NonTerminal nt)
    {
        if (have(nt)) {
            currentToken = scanner.next();
            return true;
        }
        return false;
    }
   
    private boolean expect(Token.Kind kind)
    {
        if (accept(kind))
            return true;
        String errorMessage = reportSyntaxError(kind);
        throw new QuitParseException(errorMessage);
        //return false;
    }
        
    private boolean expect(NonTerminal nt)
    {
        if (accept(nt))
            return true;
        String errorMessage = reportSyntaxError(nt);
        throw new QuitParseException(errorMessage);
        //return false;
    }
   
// Grammar Rules =====================================================
    
    // literal := INTEGER | FLOAT | TRUE | FALSE .
    public void literal()
    {
        expect(NonTerminal.LITERAL);
    }

    // designator := IDENTIFIER { "[" expression0 "]" } .
    public void designator()
    {
        expect(Token.Kind.IDENTIFIER);
        while (accept(Token.Kind.OPEN_BRACKET)) {
            expression0();
            expect(Token.Kind.CLOSE_BRACKET);
        }
    }

    // type := IDENTIFIER
    public void type()
    {
        expect(Token.Kind.IDENTIFIER);
    }

    // op0 := >= | <= | != | == | < | >
    public void op0()
    {
        expect(NonTerminal.OP0);
    }

    // op1 : + | - | or
    public void op1()
    {
        expect(NonTerminal.OP1);
    }

    // op2 : * | / | and
    public void op2()
    {
        expect(NonTerminal.OP2);
    }

    // expression0 := expression1 [ op0 expression1 ]
    public void expression0()
    {
        expression1();
        while (have(NonTerminal.OP0)) {
            op0();
            expression1();
        }
    }

    public void expression1()
    {
        expression2();
        while (have(NonTerminal.OP1)) {
            op1();
            expression2();
        }
    }

    public void expression2()
    {
        expression3();
        while (have(NonTerminal.OP2)) {
            op2();
            expression3();
        }
    }

    public void expression3()
    {
        if (accept(Token.Kind.NOT)) {
            expression3();
        } else if (accept(Token.Kind.OPEN_PAREN)) {
            expression0();
            expect(Token.Kind.CLOSE_PAREN);
        } else if (have(NonTerminal.DESIGNATOR)) {
            designator();
        } else if (have(NonTerminal.CALL_EXPRESSION)) {
            call_expression();
        } else if (have(NonTerminal.LITERAL)) {
            literal();
        }
    }

    public void call_expression()
    {
        expect(NonTerminal.CALL_EXPRESSION);
        tryResolveSymbol(this.currentToken);
        expect(Token.Kind.IDENTIFIER);
        expect(Token.Kind.OPEN_PAREN);
        expression_list();
        expect(Token.Kind.CLOSE_PAREN);
    }

    public void expression_list()
    {
        if (have(NonTerminal.EXPRESSION_LIST)) {
            expression0();
            while (accept(Token.Kind.COMMA)) {
                expression0();
            }
        }
    }

    public void parameter() {
        this.tryDeclareSymbol(this.currentToken);
        expect(Token.Kind.IDENTIFIER);
        expect(Token.Kind.COLON);
        type();
    }

    public void parameter_list() {
        if (have(NonTerminal.PARAMETER)) {
            parameter();
            while (accept(Token.Kind.COMMA)) {
                parameter();
            }
        }
    }

    public void variable_declaration() {
        expect(Token.Kind.VAR);
        tryDeclareSymbol(this.currentToken);
        expect(Token.Kind.IDENTIFIER);
        expect(Token.Kind.COLON);
        type();
        expect(Token.Kind.SEMICOLON);
    }

    public void array_declaration() {
        expect(Token.Kind.ARRAY);
        tryDeclareSymbol(this.currentToken);
        expect(Token.Kind.IDENTIFIER);
        expect(Token.Kind.COLON);
        type();
        expect(Token.Kind.OPEN_BRACKET);
        expect(Token.Kind.INTEGER);
        expect(Token.Kind.CLOSE_BRACKET);
        while (accept(Token.Kind.OPEN_BRACKET)) {
            expect(Token.Kind.INTEGER);
            expect(Token.Kind.CLOSE_BRACKET);
        }
        expect(Token.Kind.SEMICOLON);
    }

    public void function_definition() {
        expect(Token.Kind.FUNC);
        Token k = this.currentToken;
        tryDeclareSymbol(this.currentToken);
        expect(Token.Kind.IDENTIFIER);
        // insert function into both scopes
        enterScope();
        expect(Token.Kind.OPEN_PAREN);
        parameter_list();
        expect(Token.Kind.CLOSE_PAREN);
        expect(Token.Kind.COLON);
        type();
        statement_block();
        exitScope();
    }

    public void declaration() {
        if (have(NonTerminal.VARIABLE_DECLARATION)) {
            variable_declaration();
        } else if (have(NonTerminal.ARRAY_DECLARATION)) {
            array_declaration();
        } else if (have(NonTerminal.FUNCTION_DEFINITION)) {
            function_definition();
        } else {
            expect(NonTerminal.DECLARATION);
        }
    }

    public void declaration_list() {
        while (have(NonTerminal.DECLARATION)) {
            declaration();
        }
    }

    public void assignment_statement() {
        expect(NonTerminal.ASSIGNMENT_STATEMENT);
        tryResolveSymbol(this.currentToken);
        designator();
        expect(Token.Kind.ASSIGN);
        expression0();
        expect(Token.Kind.SEMICOLON);
    }

    public void call_statement() {
        call_expression();
        expect(Token.Kind.SEMICOLON);
    }

    public void if_statement() {
        expect(NonTerminal.IF_STATEMENT);
        expression0();
        enterScope();
        statement_block();
        exitScope();
        if (accept(Token.Kind.ELSE)) {
            enterScope();
            statement_block();
            exitScope();
        }
    }

    public void while_statement() {
        expect(NonTerminal.WHILE_STATEMENT);
        enterScope();
        expression0();
        statement_block();
        exitScope();
    }

    public void return_statement() {
        expect(NonTerminal.RETURN_STATEMENT);
        expression0();
        expect(Token.Kind.SEMICOLON);
    }

    public void statement() {
        if (!have(NonTerminal.STATEMENT))
            expect(NonTerminal.STATEMENT);
        else if (have(NonTerminal.VARIABLE_DECLARATION)) {
            variable_declaration();
        } else if (have(NonTerminal.CALL_STATEMENT)) {
            call_statement();
        } else if (have(NonTerminal.ASSIGNMENT_STATEMENT)) {
            assignment_statement();
        } else if (have(NonTerminal.IF_STATEMENT)) {
            if_statement();
        } else if (have(NonTerminal.WHILE_STATEMENT)) {
            while_statement();
        } else if (have(NonTerminal.RETURN_STATEMENT)) {
            return_statement();
        }
    }

    public void statement_list() {
        if (have(NonTerminal.STATEMENT)) {
            statement();
            while (have(NonTerminal.STATEMENT))
                statement();
        }
    }

    public void statement_block() {
        expect(Token.Kind.OPEN_BRACE);
        statement_list();
        expect(Token.Kind.CLOSE_BRACE);
    }

    // program := declaration-list EOF .
    public void program()
    {
        initSymbolTable();
        declaration_list();
    }
}
