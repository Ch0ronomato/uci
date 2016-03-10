package crux;

import ast.*;
import ast.Error;
import sun.jvm.hotspot.debugger.cdbg.Sym;
import types.*;

import java.util.*;

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
        symbolTable.lookup("readInt").setType(new FuncType(new TypeList(), Type.getBaseType("int")));
        symbolTable.insert("readFloat");
        symbolTable.lookup("readFloat").setType(new FuncType(new TypeList(), Type.getBaseType("float")));
        symbolTable.insert("printBool");
        TypeList printBool = new TypeList();
        printBool.append(new BoolType());
        symbolTable.lookup("printBool").setType(new FuncType(printBool, Type.getBaseType("void")));
        symbolTable.insert("printInt");
        TypeList printInt = new TypeList();
        printInt.append(new types.IntType());
        symbolTable.lookup("printInt").setType(new FuncType(printInt, Type.getBaseType("void")));
        symbolTable.insert("printFloat");
        TypeList printFloat = new TypeList();
        printFloat.append(new types.FloatType());
        symbolTable.lookup("printFloat").setType(new FuncType(printFloat, Type.getBaseType("void")));
        symbolTable.insert("println");
        symbolTable.lookup("println").setType(new FuncType(new TypeList(), Type.getBaseType("void")));
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
    
// Grammar Rule Reporting ==========================================
    private int parseTreeRecursionDepth = 0;
    private StringBuffer parseTreeBuffer = new StringBuffer();

    public void enterRule(NonTerminal nonTerminal) {
        String lineData = new String();
        for(int i = 0; i < parseTreeRecursionDepth; i++)
        {
            lineData += "  ";
        }
        lineData += nonTerminal.name();
        //System.out.println("descending " + lineData);
        parseTreeBuffer.append(lineData + "\n");
        parseTreeRecursionDepth++;
    }

    private void exitRule(NonTerminal nonTerminal)
    {
        parseTreeRecursionDepth--;
    }

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
    
// Parser ==========================================
   
    public ast.Command parse()
    {
        initSymbolTable();
        try {
            currentToken = scanner.next();
            return program();
        } catch (QuitParseException q) {
            return new ast.Error(lineNumber(), charPosition(), "Could not complete parsing.");
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
        String errormessage = reportSyntaxError(kind);
        throw new QuitParseException(errormessage);
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

    private int expectInteger (Token.Kind kind) {
        Token tok = currentToken;
        if (accept(kind))
            return Integer.parseInt(tok.lexeme());
        String errorMessage = reportSyntaxError(kind);
        throw new QuitParseException(errorMessage);
    }

    private int expectInteger (NonTerminal nt) {
        Token tok = currentToken;
        if (accept(nt))
            return Integer.parseInt(tok.lexeme());
        String errorMessage = reportSyntaxError(nt);
        throw new QuitParseException(errorMessage);
    }
   
// Grammar Rules =====================================================
    
    // literal := INTEGER | FLOAT | TRUE | FALSE .
    public ast.Expression literal()
    {
        enterRule(NonTerminal.LITERAL);
        Token tok = expectRetrieve(NonTerminal.LITERAL);
        ast.Expression exp = Command.newLiteral(tok);
        exitRule(NonTerminal.LITERAL);
        return exp;
    }

    // designator := IDENTIFIER { "[" expression0 "]" } .
    public Expression designator()
    {
        enterRule(NonTerminal.DESIGNATOR);
        Expression to_return;
        Token tok = expectRetrieve(Token.Kind.IDENTIFIER);

        // we already know the designator is declared in scope.
        Symbol s = symbolTable.lookup(tok.lexeme());
        Expression addr = new AddressOf(tok.lineNumber(), tok.charPosition(), s);
        to_return = addr;
        while (accept(Token.Kind.OPEN_BRACKET)) {
            to_return = new Index(currentToken.lineNumber(), currentToken.charPosition(), to_return, expression0());
            expect(Token.Kind.CLOSE_BRACKET);
        }
        exitRule(NonTerminal.DESIGNATOR);
        return to_return;
    }

    // type := IDENTIFIER
    // @todo: return type.
    public types.Type type()
    {
        enterRule(NonTerminal.TYPE);
        Token k = expectRetrieve(Token.Kind.IDENTIFIER);
        exitRule(NonTerminal.TYPE);
        return types.Type.getBaseType(k.lexeme());
    }

    // op0 := >= | <= | != | == | < | >
    public Token op0()
    {
        return expectRetrieve(NonTerminal.OP0);
    }

    // op1 : + | - | or
    public Token op1()
    {
        return expectRetrieve(NonTerminal.OP1);
    }

    // op2 : * | / | and
    public Token op2()
    {
        return expectRetrieve(NonTerminal.OP2);
    }

    // expression0 := expression1 [ op0 expression1 ]
    public Expression expression0()
    {
        Expression expr = expression1();
        while (have(NonTerminal.OP0)) {
            Token op = op0();
            expr = Command.newExpression(expr, op, expression1());
        }
        return expr;
    }

    public Expression expression1()
    {
        Expression expr = expression2();
        while (have(NonTerminal.OP1)) {
            Token op = op1();
            expr = Command.newExpression(expr, op, expression2());
        }
        return expr;
    }

    public Expression expression2()
    {
        Expression expr = expression3();
        while (have(NonTerminal.OP2)) {
            Token op = op2();
            expr = Command.newExpression(expr, op, expression3());
        }
        return expr;
    }

    public Expression expression3()
    {
        Token tok = this.currentToken;
        Expression expr = new Error(currentToken.lineNumber(), currentToken.charPosition(), "Unknown expression");
        if (accept(Token.Kind.NOT)) {
            expr = new LogicalNot(tok.lineNumber(), tok.charPosition(), expression3());
        } else if (accept(Token.Kind.OPEN_PAREN)) {
            expr = expression0();
            expect(Token.Kind.CLOSE_PAREN);
        } else if (have(NonTerminal.DESIGNATOR)) {
            expr = designator();
            expr = new Dereference(tok.lineNumber(), tok.charPosition(), expr);
        } else if (have(NonTerminal.CALL_EXPRESSION)) {
            expr = call_expression();
        } else if (have(NonTerminal.LITERAL)) {
            expr = literal();
        }
        return expr;
    }

    public Call call_expression()
    {
        Token k = currentToken;
        expect(NonTerminal.CALL_EXPRESSION);
        Token tok = expectRetrieve(Token.Kind.IDENTIFIER);
        Symbol s = tryResolveSymbol(tok);
        expect(Token.Kind.OPEN_PAREN);
        ExpressionList list = expression_list();
        expect(Token.Kind.CLOSE_PAREN);
        return new Call(k.lineNumber(), k.charPosition(), s, list);
    }

    public ExpressionList expression_list()
    {
        ExpressionList expression_list = new ExpressionList(currentToken.lineNumber(), currentToken.charPosition());
        if (have(NonTerminal.EXPRESSION_LIST)) {
            expression_list.add(expression0());
            while (accept(Token.Kind.COMMA)) {
                expression_list.add(expression0());
            }
        }
        return expression_list;
    }

    public Symbol parameter() {
        this.tryDeclareSymbol(this.currentToken);
        Token tok = expectRetrieve(Token.Kind.IDENTIFIER);
        expect(Token.Kind.COLON);
        types.Type t = type();
        Symbol symbol = new Symbol(tok.lexeme());
        symbol.setType(t);
        symbolTable.lookup(tok.lexeme()).setType(t);
        return symbol;
    }

    // @todo: return TypeList
    public List<Symbol> parameter_list() {
        List<Symbol> params = new ArrayList<Symbol>();
        if (have(NonTerminal.PARAMETER)) {
            params.add(parameter());
            while (accept(Token.Kind.COMMA)) {
                params.add(parameter());
            }
        }
        return params;
    }

    // @todo: for all declerations, run TypeChecker on it
    // @todo: and add it to the hashmap
    public ast.VariableDeclaration variable_declaration() {
        Token k = expectRetrieve(Token.Kind.VAR);
        Symbol s = tryDeclareSymbol(this.currentToken);
        Token tok = expectRetrieve(Token.Kind.IDENTIFIER);
        ast.VariableDeclaration decl = new VariableDeclaration(k.lineNumber(), k.charPosition(), s);
        expect(Token.Kind.COLON);
        types.Type t = type();
        decl.symbol().setType(t);
        symbolTable.lookup(tok.lexeme()).setType(t);
        expect(Token.Kind.SEMICOLON);
        return decl;
    }

    // @todo: For all declerations, run TypeChecker on this
    // @todo: It's an array type, so ensure the inner type
    public ast.ArrayDeclaration array_declaration() {
        List<Integer> numbers = new ArrayList<>();
        Token tok = expectRetrieve(Token.Kind.ARRAY);
        Symbol symbol = tryDeclareSymbol(this.currentToken);
        Token name = expectRetrieve(Token.Kind.IDENTIFIER);
        expect(Token.Kind.COLON);
        types.Type t = type();
        expect(Token.Kind.OPEN_BRACKET);
        int extent = expectInteger(Token.Kind.INTEGER);
        expect(Token.Kind.CLOSE_BRACKET);
        types.ArrayType final_type;
        numbers.add(extent);
        while (accept(Token.Kind.OPEN_BRACKET)) {
            // nest the array type.
            int inner_extent = expectInteger(Token.Kind.INTEGER);
            expect(Token.Kind.CLOSE_BRACKET);
            numbers.add(inner_extent);
        }
        expect(Token.Kind.SEMICOLON);

        // build backwards
        final_type = new types.ArrayType(numbers.get(numbers.size() - 1), t);
        for (int i = numbers.size() - 2; i > -1; i--) {
            final_type = new types.ArrayType(numbers.get(i), final_type);
        }
        symbol.setType(final_type);
        symbolTable.lookup(name.lexeme()).setType(final_type);
        return new ArrayDeclaration(tok.lineNumber(), tok.charPosition(), symbol);
    }

    // @todo: Add function to the type checker reference
    // @todo: For all invocations, ensure the return matches the expected.
    public ast.FunctionDefinition function_definition() {
        Token k = this.currentToken;
        expect(Token.Kind.FUNC);
        Symbol symbol = tryDeclareSymbol(this.currentToken);
        Token name = expectRetrieve(Token.Kind.IDENTIFIER);
        // insert function into both scopes
        enterScope();
        expect(Token.Kind.OPEN_PAREN);
        List<Symbol> args = parameter_list();
        expect(Token.Kind.CLOSE_PAREN);
        expect(Token.Kind.COLON);
        types.Type return_type = type();
        ast.StatementList sl = statement_block();
        exitScope();
        TypeList arg_types = new TypeList();
        for (Symbol s : args) {
            arg_types.append(s.type());
        }
        Type func = new FuncType(arg_types, return_type);
        symbol.setType(func);
        symbolTable.lookup(name.lexeme()).setType(func);
        return new FunctionDefinition(k.lineNumber(), k.charPosition(), symbol, args, sl);
    }

    public ast.Declaration declaration() {
        Declaration decl = new Error(currentToken.lineNumber(), currentToken.charPosition(), "Unknown decleration");
        if (have(NonTerminal.VARIABLE_DECLARATION)) {
            decl = variable_declaration();
        } else if (have(NonTerminal.ARRAY_DECLARATION)) {
            decl = array_declaration();
        } else if (have(NonTerminal.FUNCTION_DEFINITION)) {
            decl = function_definition();
        } else {
            expect(NonTerminal.DECLARATION);
        }
        return decl;
    }

    public DeclarationList declaration_list() {
        DeclarationList dl = new DeclarationList(this.lineNumber(), this.charPosition());;
        while (have(NonTerminal.DECLARATION)) {
            dl.add(declaration());
        }
        return dl;
    }

    public Assignment assignment_statement() {
        Token let = this.currentToken;
        expect(NonTerminal.ASSIGNMENT_STATEMENT);
        tryResolveSymbol(this.currentToken);
        Expression left = designator();
        expectRetrieve(Token.Kind.ASSIGN);
        Expression right = expression0();
        expect(Token.Kind.SEMICOLON);
        return new Assignment(let.lineNumber(), let.charPosition(), left, right);
    }

    public Call call_statement() {
        Call c = call_expression();
        expect(Token.Kind.SEMICOLON);
        return c;
    }

    public IfElseBranch if_statement() {
        Token begin = this.currentToken;
        expect(NonTerminal.IF_STATEMENT);
        Expression predict = expression0();
        enterScope();
        StatementList sl = statement_block();
        StatementList else_block = new StatementList(currentToken.lineNumber(), currentToken.charPosition());
        exitScope();
        if (accept(Token.Kind.ELSE)) {
            enterScope();
            else_block = statement_block();
            exitScope();
        }
        return new IfElseBranch(begin.lineNumber(), begin.charPosition(), predict, sl, else_block);
    }

    public WhileLoop while_statement() {
        Token tok = this.currentToken;
        expect(NonTerminal.WHILE_STATEMENT);
        enterScope();
        Expression predicate = expression0();
        StatementList sl = statement_block();
        exitScope();
        return new WhileLoop(tok.lineNumber(), tok.charPosition(), predicate, sl);
    }

    public Return return_statement() {
        Token tok = this.currentToken;
        expect(NonTerminal.RETURN_STATEMENT);
        Expression value = expression0();
        expect(Token.Kind.SEMICOLON);
        return new Return(tok.lineNumber(), tok.charPosition(), value);
    }

    public Statement statement() {
        if (!have(NonTerminal.STATEMENT))
            expect(NonTerminal.STATEMENT);
        else if (have(NonTerminal.VARIABLE_DECLARATION)) {
            return variable_declaration();
        } else if (have(NonTerminal.CALL_STATEMENT)) {
            return call_statement();
        } else if (have(NonTerminal.ASSIGNMENT_STATEMENT)) {
            return assignment_statement();
        } else if (have(NonTerminal.IF_STATEMENT)) {
            return if_statement();
        } else if (have(NonTerminal.WHILE_STATEMENT)) {
            return while_statement();
        } else if (have(NonTerminal.RETURN_STATEMENT)) {
            return return_statement();
        }
        return new ast.Error(this.currentToken.lineNumber(), this.currentToken.charPosition(), "Error");
    }

    public StatementList statement_list() {
        StatementList sl = new StatementList(this.currentToken.lineNumber(), this.currentToken.charPosition());
        if (have(NonTerminal.STATEMENT)) {
            sl.add(statement());
            while (have(NonTerminal.STATEMENT))
                sl.add(statement());
        }
        return sl;
    }

    public StatementList statement_block() {
        expect(Token.Kind.OPEN_BRACE);
        StatementList sl = statement_list();
        expect(Token.Kind.CLOSE_BRACE);
        return sl;
    }

    // program := declaration-list EOF .
    public Command program()
    {
        initSymbolTable();
        return declaration_list();
    }
}
