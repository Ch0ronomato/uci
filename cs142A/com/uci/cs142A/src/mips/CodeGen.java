package mips;

import ast.*;
import types.*;

public class CodeGen implements ast.CommandVisitor {
    
    private StringBuffer errorBuffer = new StringBuffer();
    private TypeChecker tc;
    private Program program;
    private String returnlabel;
    private ActivationRecord currentFunction;

    public CodeGen(TypeChecker tc)
    {
        this.tc = tc;
        this.program = new Program();
    }
    
    public boolean hasError()
    {
        return errorBuffer.length() != 0;
    }
    
    public String errorReport()
    {
        return errorBuffer.toString();
    }

    private class CodeGenException extends RuntimeException
    {
        private static final long serialVersionUID = 1L;
        public CodeGenException(String errorMessage) {
            super(errorMessage);
        }
    }
    
    public boolean generate(Command ast)
    {
        try {
            currentFunction = ActivationRecord.newGlobalFrame();
            ast.accept(this);
            return !hasError();
        } catch (CodeGenException e) {
            return false;
        }
    }
    
    public Program getProgram()
    {
        return program;
    }

    @Override
    public void visit(ExpressionList node) {
        for (Expression expr : node) {
            expr.accept(this);
        }
    }

    @Override
    public void visit(DeclarationList node) {
        for (Declaration decl : node) {
            decl.accept(this);
        }
    }

    @Override
    public void visit(StatementList node) {
        for (Statement stat : node) {
            stat.accept(this);
            if (stat instanceof Call) {
                handleNonVoidReturns(tc.getType((Command) stat));
            }
        }
    }

    @Override
    public void visit(AddressOf node) {
        String register = "$t0";
        currentFunction.getAddress(program, register , node.symbol());
        program.pushInt(register);
    }

    @Override
    public void visit(LiteralBool node) {
        int nodeIntValue = node.value() == LiteralBool.Value.TRUE ? 1 : 0;
        program.appendInstruction("li $t0, " + nodeIntValue);
        program.pushInt("$t0");
    }

    @Override
    public void visit(LiteralFloat node) {
        program.appendInstruction("li.s $f0, " + node.value());
        program.pushFloat("$f0");
    }

    @Override
    public void visit(LiteralInt node) {
        program.appendInstruction("li $t0, " + node.value());
        program.pushInt("$t0");
    }

    @Override
    public void visit(VariableDeclaration node) {
        // just add the variable to the runtime.
        currentFunction.add(program, node);
    }

    @Override
    public void visit(ArrayDeclaration node) {
        currentFunction.add(program, node);
    }

    @Override
    public void visit(FunctionDefinition node) {
        currentFunction = new ActivationRecord(node, currentFunction);

        // book keeping and special work for the main method.
        program.debugComment("Got here with a function");
        String funcName = !node.symbol().name().equals("main") ? appendNamespace(node.function().name()) : node.symbol().name();
        returnlabel = program.newLabel();
        int startPos = program.appendInstruction(funcName + ":") + 1;

        // begin actual function call.
        node.body().accept(this);
        program.insertPrologue(startPos, currentFunction.stackSize());

        program.appendInstruction(returnlabel + ":");
        handleNonVoidReturns(((FuncType)tc.getType(node)).returnType());

        program.appendEpilogue(currentFunction.stackSize());
        currentFunction = currentFunction.parent();
    }

    @Override
    public void visit(Addition node) {
        node.leftSide().accept(this);
        node.rightSide().accept(this);
        handleArithmatic(this.tc.getType(node), "add", 1, 0, 2);
    }

    @Override
    public void visit(Subtraction node) {
        node.leftSide().accept(this);
        node.rightSide().accept(this);
        handleArithmatic(this.tc.getType(node), "sub", 1, 0, 2);
    }

    @Override
    public void visit(Multiplication node) {
        node.leftSide().accept(this);
        node.rightSide().accept(this);
        handleArithmatic(this.tc.getType(node), "mul", 1, 0, 2);
    }

    @Override
    public void visit(Division node) {
        node.leftSide().accept(this);
        node.rightSide().accept(this);
        handleArithmatic(this.tc.getType(node), "div", 1, 0, 2);
    }

    @Override
    public void visit(LogicalAnd node) {
        node.leftSide().accept(this);
        node.rightSide().accept(this);
        handleArithmatic(this.tc.getType(node), "and", 1, 0, 2);
    }

    @Override
    public void visit(LogicalOr node) {
        node.leftSide().accept(this);
        node.rightSide().accept(this);
        handleArithmatic(this.tc.getType(node), "or", 1, 0, 2);
    }
    
    @Override
    public void visit(LogicalNot node) {
        String falseLabl = program.newLabel();
        String pushLabel = program.newLabel();
        node.expression().accept(this);
        program.popInt("$t0");
        program.appendInstruction("beqz $t0, " + falseLabl);
        program.appendInstruction("li $t1, 0");
        program.appendInstruction("b " + pushLabel);
        program.appendInstruction(falseLabl + ":");
        program.appendInstruction("li $t1, 1");
        program.appendInstruction(pushLabel + ":");
        program.debugComment("Pushing and result.");
        program.pushInt("$t1");
    }

    @Override
    public void visit(Comparison node) {
        node.leftSide().accept(this);
        node.rightSide().accept(this);
        Type type = this.tc.getType((Command) node.leftSide());
        if (type.equivalent(new FloatType())) {
            program.popFloat("$f2");
            program.popFloat("$f0");
            switch (node.operation()) {
                case LT:
                    program.appendInstruction("c.lt.s $f0, $f2");
                    break;
                case GT:
                    program.appendInstruction("c.gt.s $f0, $f2");
                    break;
                case LE:
                    program.appendInstruction("c.le.s $f0, $f2");
                    break;
                case GE:
                    program.appendInstruction("c.ge.s $f0, $f2");
                    break;
                case EQ:
                    program.appendInstruction("c.eq.s $f0, $f2");
                    break;
                case NE:
                    program.appendInstruction("c.ne.s $f0, $f2");
                    break;
            }
            // Can I move the label decls?
            String elseLabel = program.newLabel();
            String joinLabel = program.newLabel();
            program.appendInstruction("bc1f " + elseLabel);
            program.appendInstruction("li $t0, 1");
            program.appendInstruction("b " + joinLabel);
            program.appendInstruction(elseLabel + ":");
            program.appendInstruction("li $t0, 0");
            program.appendInstruction(joinLabel + ":");
            program.pushInt("$t0");
        } else if (isInt(type)) {
            program.popInt("$t1");
            program.popInt("$t0");
            switch (node.operation()) {
                case LT:
                    program.appendInstruction("slt $t2, $t0, $t1");
                    break;
                case GT:
                    program.appendInstruction("sgt $t2, $t0, $t1");
                    break;
                case LE:
                    program.appendInstruction("sle $t2, $t0, $t1");
                    break;
                case GE:
                    program.appendInstruction("sge $t2, $t0, $t1");
                    break;
                case EQ:
                    program.appendInstruction("seq $t2, $t0, $t1");
                    break;
                case NE:
                    program.appendInstruction("sne $t2, $t0, $t1");
                    break;
            }
            program.pushInt("$t2");
        }
    }

    @Override
    public void visit(Dereference node) {
        Type type = this.tc.getType(node);
        node.expression().accept(this);
        program.popInt("$t0");
        if (type.equivalent(new FloatType())) {
            program.appendInstruction("lwc1 $f0, 0($t0)");
            program.pushFloat("$f0");
        } else if (isInt(type)) {
            program.appendInstruction("lw $t1, 0($t0)");
            program.pushInt("$t1");
        }
    }

    @Override
    public void visit(Index node) {
        Type type = this.tc.getType(node);
        node.base().accept(this);
        node.amount().accept(this);

        // build our output for index
        program.popInt("$t0");
        program.popInt("$t1");
        program.appendInstruction("li $t2, " + type.numBytes());
        program.appendInstruction("mul $t3, $t0, $t2");
        program.appendInstruction("add $t4, $t1, $t3");
        program.pushInt("$t4");
    }

    @Override
    public void visit(Assignment node) {
        node.destination().accept(this);
        node.source().accept(this);
        Type type = this.tc.getType(node);
        if (isInt(type)){
            program.popInt("$t0");
            program.popInt("$t1"); // dest addr
            program.appendInstruction("sw $t0, 0($t1)");
        } else {
            program.popFloat("$f0");
            program.popInt("$t0");
            program.appendInstruction("swc1 $f0, 0($t0)");
        }
    }

    @Override
    public void visit(Call node) {
        node.arguments().accept(this);
        String funcName =  node.function().name();
        if (!funcName.matches("print(Bool|Float|Int|ln)|read(Float|Int)")) {
            funcName = appendNamespace(funcName);
        } else {
            funcName = "func." + funcName;
        }
        program.appendInstruction("jal " + funcName);

        if (node.arguments().size() > 0) {
            int argSize = 0;
            for (Expression expr : node.arguments()) {
                Type type = this.tc.getType((Command) expr);
                argSize += type.numBytes();
            }
            program.appendInstruction("addi $sp, $sp, " + argSize);
        }

        FuncType func = (FuncType) node.function().type();
        if (!func.returnType().equivalent(new VoidType())) {
            program.appendInstruction("subu $sp, $sp, 4");
            program.appendInstruction("sw $v0, 0($sp)");
        }
    }

    @Override
    public void visit(IfElseBranch node) {
        String notif = program.newLabel();
        String join = program.newLabel();

        node.condition().accept(this);
        program.popInt("$t7");
        program.appendInstruction("beqz $t7, " + notif);
        node.thenBlock().accept(this);
        program.appendInstruction("b " + join);
        program.appendInstruction(notif + ":");
        node.elseBlock().accept(this);
        program.appendInstruction(join + ":");
    }

    @Override
    public void visit(WhileLoop node) {
        String condition = program.newLabel();
        String join = program.newLabel();
        program.appendInstruction(condition + ":");
        node.condition().accept(this);
        program.popInt("$t7");
        program.appendInstruction("beqz $t7, " + join);

        node.body().accept(this);
        program.appendInstruction("b " + condition);
        program.appendInstruction(join + ":");
    }

    @Override
    public void visit(Return node) {
        node.argument().accept(this);
        program.appendInstruction("b " + returnlabel);
    }

    @Override
    public void visit(ast.Error node) {
        String message = "CodeGen cannot compile a " + node.message();
        errorBuffer.append(message);
        throw new CodeGenException(message);
    }

    private boolean isInt(Type type) {
        return type.equivalent(new IntType()) || type.equivalent(new BoolType());
    }

    private boolean handleNonVoidReturns(Type retType) {
        if (!retType.equivalent(new VoidType())) {
            if (retType.equivalent(new FloatType())) {
                program.popFloat("$v0");
            } else if (isInt(retType)) {
                program.popInt("$v0");
            }
        }
        return isInt(retType);
    }

    private void handleArithmatic(Type t, String op, int leftReg, int rightReg, int resultReg) {
        if (isInt(t)) {
            program.popInt("$t" + leftReg);
            program.popInt("$t" + rightReg);
            program.appendInstruction(String.format("%s $%s, $%s, $%s", op, "t" + resultReg, "t" + rightReg, "t" + leftReg));
            program.pushInt("$t" + resultReg);
        } else {
            program.popFloat("$f" + 2 * leftReg);
            program.popFloat("$f" + 2 * rightReg);
            program.appendInstruction(String.format("%s.s $%s, $%s, $%s", op, "f"+(resultReg*2), "f"+(rightReg*2), "f"+(leftReg*2)));
            program.pushFloat("$f" + 2 * resultReg);
        }
    }
    /*
    private void handleComparison(Type t, String op, String prefix, int leftReg, int rightReg, int resultReg) {
        if (isInt(t)) {
            program.popInt(leftReg);
            program.popInt(rightReg);
            program.appendInstruction(String.format("%s $s, $s, $s", op, "t" + resultReg, "t" + leftReg, "t" + rightReg));
            program.pushInt(resultReg);
        } else {
            program.popFloat(leftReg);
            program.popFloat(rightReg);
            program.appendInstruction(String.format("%s.s $s, $s, $s", op, "f"+(resultReg*2), "f"+(leftReg*2), "f"+(rightReg*2));
            program.pushFloat(resultReg);
        }
    }*/

    private String appendNamespace(String name) {
        return "cruxfunc." + name;
    }
}
