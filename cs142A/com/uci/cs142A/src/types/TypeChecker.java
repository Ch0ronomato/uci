package types;

import java.util.HashMap;
import ast.*;
import ast.Error;
import com.sun.org.apache.xpath.internal.operations.Bool;

public class TypeChecker implements CommandVisitor {
    
    private HashMap<Command, Type> typeMap;
    private StringBuffer errorBuffer;
    private Type innerMostReturnType;
    private boolean returning;
    private String funcname;
    /* Useful error strings:
     *
     * "Function " + func.name() + " has a void argument in position " + pos + "."
     * "Function " + func.name() + " has an error in argument in position " + pos + ": " + error.getMessage()
     *
     * "Function main has invalid signature."
     *
     * "Not all paths in function " + currentFunctionName + " have a return."
     *
     * "IfElseBranch requires bool condition not " + condType + "."
     * "WhileLoop requires bool condition not " + condType + "."
     *
     * "Function " + currentFunctionName + " returns " + currentReturnType + " not " + retType + "."
     *
     * "Variable " + varName + " has invalid type " + varType + "."
     * "Array " + arrayName + " has invalid base type " + baseType + "."
     */

    public TypeChecker()
    {
        typeMap = new HashMap<Command, Type>();
        errorBuffer = new StringBuffer();
    }

    private void reportError(int lineNum, int charPos, String message)
    {
        errorBuffer.append("TypeError(" + lineNum + "," + charPos + ")");
        errorBuffer.append("[" + message + "]" + "\n");
    }

    private void put(Command node, Type type)
    {
        if (type instanceof ErrorType) {
            reportError(node.lineNumber(), node.charPosition(), ((ErrorType)type).getMessage());
        }
        typeMap.put(node, type);
    }
    
    public Type getType(Command node)
    {
        return typeMap.get(node);
    }
    
    public boolean check(Command ast)
    {
        ast.accept(this);
        return !hasError();
    }
    
    public boolean hasError()
    {
        return errorBuffer.length() != 0;
    }
    
    public String errorReport()
    {
        return errorBuffer.toString();
    }

    @Override
    public void visit(ExpressionList node) {
        TypeList list = new TypeList();
        for (Expression expr : node) {
            expr.accept(this);
            list.append(getType((Command) expr));
        }
        put(node, list);
    }

    @Override
    public void visit(DeclarationList node) {
        for (Declaration decl : node) {
            decl.accept(this);
        }
    }

    @Override
    public void visit(StatementList node) {
        returning = false;
        for (Statement s : node) {
            s.accept(this);
        }
    }

    @Override
    public void visit(AddressOf node) {
        put(node, new types.AddressType(node.symbol().type()));
    }

    @Override
    public void visit(LiteralBool node) {
        put(node, Type.getBaseType("bool"));
    }

    @Override
    public void visit(LiteralFloat node) {
        put(node, Type.getBaseType("float"));
    }

    @Override
    public void visit(LiteralInt node) {
        put(node, Type.getBaseType("int"));
    }

    @Override
    public void visit(VariableDeclaration node) {
        Type voidT = new VoidType();
        if (voidT.equivalent(node.symbol().type())) {
            String msg = "Variable " + node.symbol().name() + " has invalid type " + node.symbol().type() + ".";
            put (node, new ErrorType(msg));
        }
        put(node, node.symbol().type());
    }

    @Override
    public void visit(ArrayDeclaration node) {
        ArrayType baseType = (ArrayType)node.symbol().type();
        if (baseType.base() instanceof types.VoidType) {
            String msg = "Array " + node.symbol().name() + " has invalid base type " + baseType.base() + ".";
            put(node, new ErrorType(msg));
        }
        put(node, baseType);
    }

    @Override
    public void visit(FunctionDefinition node) {
        returning = false;
        innerMostReturnType = ((FuncType)node.function().type()).returnType();
        funcname = node.function().name();
        Type voidT = Type.getBaseType("void");
        if (node.ismain()) {
            // ensure the type is void.
            if (!innerMostReturnType.equivalent(Type.getBaseType("void"))) {
                this.reportError(node.lineNumber(), node.charPosition(), "Function main has invalid signature.");
                return;
            }
        }

        // arguments
        int i = 0;
        for (crux.Symbol s : node.arguments()) {
            if (s.type().equivalent(voidT)) {
                this.reportError(node.lineNumber(), node.charPosition(), "Function " + funcname + " has a void argument in position " + i + ".");
                return;
            }

            if (s.type() instanceof ErrorType) {
                String msg = "Function " + funcname + " has an error in argument in position " + i + ": " + ((ErrorType) s.type()).getMessage();
                this.reportError(node.lineNumber(), node.charPosition(), msg);
                return;
            }
            i++;
        }
        node.body().accept(this);
        if (!innerMostReturnType.equivalent(voidT) && !returning) {
            String msg = "Not all paths in function " + funcname + " have a return.";
            this.reportError(node.lineNumber(), node.charPosition(), msg);
            return;
        }

        this.put(node, node.function().type());
    }

    @Override
    public void visit(Comparison node) {
        node.leftSide().accept(this);
        node.rightSide().accept(this);
        put(node, getType((Command)node.leftSide()).compare(getType((Command)node.rightSide())));

    }
    
    @Override
    public void visit(Addition node) {
        node.leftSide().accept(this);
        node.rightSide().accept(this);
        put(node, getType((Command)node.leftSide()).add(getType((Command) node.rightSide())));
    }
    
    @Override
    public void visit(Subtraction node) {
        node.leftSide().accept(this);
        node.rightSide().accept(this);
        put(node, getType((Command)node.leftSide()).sub(getType((Command) node.rightSide())));
    }
    
    @Override
    public void visit(Multiplication node) {
        node.leftSide().accept(this);
        node.rightSide().accept(this);
        put(node, getType((Command)node.leftSide()).mul(getType((Command) node.rightSide())));
    }
    
    @Override
    public void visit(Division node) {
        node.leftSide().accept(this);
        node.rightSide().accept(this);
        put(node, getType((Command)node.leftSide()).div(getType((Command) node.rightSide())));
    }
    
    @Override
    public void visit(LogicalAnd node) {
        node.leftSide().accept(this);
        node.rightSide().accept(this);
        put(node, getType((Command)node.leftSide()).and(getType((Command) node.rightSide())));
    }

    @Override
    public void visit(LogicalOr node) {
        node.leftSide().accept(this);
        node.rightSide().accept(this);
        put(node, getType((Command)node.leftSide()).or(getType((Command) node.rightSide())));
    }

    @Override
    public void visit(LogicalNot node) {
        node.expression().accept(this);
        put(node, getType((Command) node.expression()).not());
    }
    
    @Override
    public void visit(Dereference node) {
        node.expression().accept(this);
        put(node, getType((Command) node.expression()).deref());
    }

    @Override
    public void visit(Index node) {
        node.base().accept(this);
        node.amount().accept(this);
        put(node, getType((Command)node.base()).index(getType((Command) node.amount())));
    }

    @Override
    public void visit(Assignment node) {
        node.source().accept(this);
        node.destination().accept(this);
        put(node, getType((Command)node.destination()).assign(getType((Command)node.source())));
    }

    @Override
    public void visit(Call node) {
        node.arguments().accept(this);
        TypeList list = (TypeList)getType(node.arguments());
        put(node, ((FuncType) node.function().type()).call(list));
    }

    @Override
    public void visit(IfElseBranch node) {
        node.condition().accept(this);
        Type conditionalType = getType((Command)node.condition());
        if (!(conditionalType instanceof BoolType)) {
            String msg = "IfElseBranch requires bool condition not " + conditionalType + ".";
            reportError(node.lineNumber(), node.charPosition(), msg);
            return;
        }
        boolean current_returning = true;
        node.thenBlock().accept(this);
        current_returning &= returning;
        node.elseBlock().accept(this);
        current_returning &= returning;
        if (!(innerMostReturnType instanceof VoidType)) {
            returning = current_returning;
        }
    }

    @Override
    public void visit(WhileLoop node) {
        node.condition().accept(this);
        Type condType = getType((Command) node.condition());
        if (!(condType instanceof BoolType)) {
            String msg = "WhileLoop requires bool condition not " + condType + ".";
            this.reportError(node.lineNumber(), node.charPosition(), msg);
            return;
        }
        node.body().accept(this);
        if (!(innerMostReturnType instanceof VoidType)) {
            returning = false;
        }
    }

    @Override
    public void visit(Return node) {
        returning = true;
        node.argument().accept(this);
        Type returnType = getType((Command) node.argument());
        if (!innerMostReturnType.equivalent(returnType)) {
            String msg = "Function " + funcname + " returns " + innerMostReturnType + " not " + returnType + ".";
            this.reportError(node.lineNumber(), node.charPosition(), msg);
            return;
        }
        put(node, returnType);
    }

    @Override
    public void visit(ast.Error node) {
        put(node, new ErrorType(node.message()));
    }
}
