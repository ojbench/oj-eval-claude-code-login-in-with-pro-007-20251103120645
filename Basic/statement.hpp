/*
 * File: statement.h
 * -----------------
 * This file defines the Statement abstract type.  In
 * the finished version, this file will also specify subclasses
 * for each of the statement types.  As you design your own
 * version of this class, you should pay careful attention to
 * the exp.h interface, which is an excellent model for
 * the Statement class hierarchy.
 */

#ifndef _statement_h
#define _statement_h

#include <string>
#include <sstream>
#include "evalstate.hpp"
#include "exp.hpp"
#include "Utils/tokenScanner.hpp"
#include "program.hpp"
#include "parser.hpp"
#include "Utils/error.hpp"
#include "Utils/strlib.hpp"

class Program;

// Global variables for control flow management
extern int currentLine;
extern int nextLine;
extern bool shouldEnd;

/*
 * Class: Statement
 * ----------------
 * This class is used to represent a statement in a program.
 * The model for this class is Expression in the exp.h interface.
 * Like Expression, Statement is an abstract class with subclasses
 * for each of the statement and command types required for the
 * BASIC interpreter.
 */

class Statement {

public:

/*
 * Constructor: Statement
 * ----------------------
 * The base class constructor is empty.  Each subclass must provide
 * its own constructor.
 */

    Statement();

/*
 * Destructor: ~Statement
 * Usage: delete stmt;
 * -------------------
 * The destructor deallocates the storage for this expression.
 * It must be declared virtual to ensure that the correct subclass
 * destructor is called when deleting a statement.
 */

    virtual ~Statement();

/*
 * Method: execute
 * Usage: stmt->execute(state);
 * ----------------------------
 * This method executes a BASIC statement.  Each of the subclasses
 * defines its own execute method that implements the necessary
 * operations.  As was true for the expression evaluator, this
 * method takes an EvalState object for looking up variables or
 * controlling the operation of the interpreter.
 */

    virtual void execute(EvalState &state, Program &program) = 0;

};


/*
 * The remainder of this file must consists of subclass
 * definitions for the individual statement forms.  Each of
 * those subclasses must define a constructor that parses a
 * statement from a scanner and a method called execute,
 * which executes that statement.  If the private data for
 * a subclass includes data allocated on the heap (such as
 * an Expression object), the class implementation must also
 * specify its own destructor method to free that memory.
 */

// REM statement - comment (does nothing)
class RemStatement : public Statement {
public:
    RemStatement() {}
    virtual ~RemStatement() {}
    virtual void execute(EvalState &state, Program &program) override {}
};

// LET statement - variable assignment
class LetStatement : public Statement {
private:
    std::string varName;
    Expression *exp;
public:
    LetStatement(std::string var, Expression *e) : varName(var), exp(e) {}
    virtual ~LetStatement() { delete exp; }
    virtual void execute(EvalState &state, Program &program) override;
};

// PRINT statement - print expression value
class PrintStatement : public Statement {
private:
    Expression *exp;
public:
    PrintStatement(Expression *e) : exp(e) {}
    virtual ~PrintStatement() { delete exp; }
    virtual void execute(EvalState &state, Program &program) override;
};

// INPUT statement - read input to variable
class InputStatement : public Statement {
private:
    std::string varName;
public:
    InputStatement(std::string var) : varName(var) {}
    virtual ~InputStatement() {}
    virtual void execute(EvalState &state, Program &program) override;
};

// END statement - terminate program
class EndStatement : public Statement {
public:
    EndStatement() {}
    virtual ~EndStatement() {}
    virtual void execute(EvalState &state, Program &program) override;
};

// GOTO statement - jump to line number
class GotoStatement : public Statement {
private:
    int targetLine;
public:
    GotoStatement(int line) : targetLine(line) {}
    virtual ~GotoStatement() {}
    virtual void execute(EvalState &state, Program &program) override;
    int getTargetLine() const { return targetLine; }
};

// IF statement - conditional jump
class IfStatement : public Statement {
private:
    Expression *lhs;
    Expression *rhs;
    std::string op;
    int targetLine;
public:
    IfStatement(Expression *l, std::string operation, Expression *r, int line)
        : lhs(l), op(operation), rhs(r), targetLine(line) {}
    virtual ~IfStatement() { delete lhs; delete rhs; }
    virtual void execute(EvalState &state, Program &program) override;
    int getTargetLine() const { return targetLine; }
};

#endif
