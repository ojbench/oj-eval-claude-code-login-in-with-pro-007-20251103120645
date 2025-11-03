/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <string>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"


/* Function prototypes */

void processLine(std::string line, Program &program, EvalState &state);

/* Main program */

int main() {
    EvalState state;
    Program program;
    //cout << "Stub implementation of BASIC" << endl;
    while (true) {
        try {
            std::string input;
            getline(std::cin, input);
            if (input.empty())
                continue;
            processLine(input, program, state);
        } catch (ErrorException &ex) {
            std::cout << ex.getMessage() << std::endl;
        }
    }
    return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version of
 * implementation, the program reads a line, parses it as an expression,
 * and then prints the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

// Helper function to check if a string is a valid integer
bool isInteger(const std::string &str) {
    if (str.empty()) return false;
    size_t start = 0;
    if (str[0] == '-' || str[0] == '+') {
        if (str.length() == 1) return false;
        start = 1;
    }
    for (size_t i = start; i < str.length(); i++) {
        if (!isdigit(str[i])) return false;
    }
    return true;
}

// Helper function to check if a string is a keyword
bool isKeyword(const std::string &str) {
    return str == "REM" || str == "LET" || str == "PRINT" ||
           str == "INPUT" || str == "END" || str == "GOTO" ||
           str == "IF" || str == "THEN" || str == "RUN" ||
           str == "LIST" || str == "CLEAR" || str == "QUIT" ||
           str == "HELP";
}

// Helper function to parse and execute a statement
Statement* parseStatement(TokenScanner &scanner, const std::string &cmd) {
    if (cmd == "REM") {
        return new RemStatement();
    } else if (cmd == "LET") {
        std::string varName = scanner.nextToken();
        if (isKeyword(varName)) {
            error("SYNTAX ERROR");
        }
        std::string equals = scanner.nextToken();
        if (equals != "=") {
            error("SYNTAX ERROR");
        }
        Expression *exp = parseExp(scanner);
        return new LetStatement(varName, exp);
    } else if (cmd == "PRINT") {
        Expression *exp = parseExp(scanner);
        return new PrintStatement(exp);
    } else if (cmd == "INPUT") {
        std::string varName = scanner.nextToken();
        if (isKeyword(varName)) {
            error("SYNTAX ERROR");
        }
        return new InputStatement(varName);
    } else if (cmd == "END") {
        return new EndStatement();
    } else if (cmd == "GOTO") {
        std::string lineStr = scanner.nextToken();
        int line = std::stoi(lineStr);
        return new GotoStatement(line);
    } else if (cmd == "IF") {
        // Parse the IF statement
        // We need to parse expressions on both sides of the comparison operator
        // Use precedence 1 to stop before comparison operators (which have precedence 1)
        Expression *lhs = nullptr;
        Expression *rhs = nullptr;
        std::string op;

        // Parse left expression (stop at comparison operators)
        lhs = readE(scanner, 1);

        // Next token should be a comparison operator
        op = scanner.nextToken();
        if (op != "=" && op != "<" && op != ">") {
            delete lhs;
            error("SYNTAX ERROR");
        }

        // Parse right expression (stop at THEN)
        rhs = readE(scanner, 1);

        // Next token should be THEN
        std::string then = scanner.nextToken();
        if (then != "THEN") {
            delete lhs;
            delete rhs;
            error("SYNTAX ERROR");
        }

        // Next token should be line number
        std::string lineStr = scanner.nextToken();
        int line = std::stoi(lineStr);
        return new IfStatement(lhs, op, rhs, line);
    }
    return nullptr;
}

// Function to run the program
void runProgram(Program &program, EvalState &state) {
    currentLine = program.getFirstLineNumber();
    shouldEnd = false;

    while (currentLine != -1 && !shouldEnd) {
        nextLine = program.getNextLineNumber(currentLine);
        Statement *stmt = program.getParsedStatement(currentLine);
        if (stmt != nullptr) {
            stmt->execute(state, program);
        }
        if (nextLine != program.getNextLineNumber(currentLine) && nextLine != -1) {
            // GOTO or IF changed the next line
            // Check if the target line exists
            if (program.getSourceLine(nextLine).empty()) {
                error("LINE NUMBER ERROR");
            }
            currentLine = nextLine;
        } else if (shouldEnd) {
            break;
        } else {
            currentLine = nextLine;
        }
    }
}

void processLine(std::string line, Program &program, EvalState &state) {
    TokenScanner scanner;
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    scanner.setInput(line);

    if (!scanner.hasMoreTokens()) {
        return;
    }

    std::string token = scanner.nextToken();

    // Check if this is a line number (numbered statement)
    if (isInteger(token)) {
        int lineNumber = std::stoi(token);

        // Check if there's anything after the line number
        if (!scanner.hasMoreTokens()) {
            // Just a line number, remove the line
            program.removeSourceLine(lineNumber);
            return;
        }

        // This is a numbered statement, store it
        program.addSourceLine(lineNumber, line);

        // Parse and store the statement
        std::string cmd = scanner.nextToken();
        Statement *stmt = parseStatement(scanner, cmd);
        if (stmt != nullptr) {
            program.setParsedStatement(lineNumber, stmt);
        }
    } else {
        // This is a direct command
        if (token == "QUIT") {
            exit(0);
        } else if (token == "LIST") {
            int lineNum = program.getFirstLineNumber();
            while (lineNum != -1) {
                std::cout << program.getSourceLine(lineNum) << std::endl;
                lineNum = program.getNextLineNumber(lineNum);
            }
        } else if (token == "CLEAR") {
            program.clear();
            state.Clear();
        } else if (token == "RUN") {
            runProgram(program, state);
        } else if (token == "HELP") {
            // Optional, not tested
        } else {
            // Direct execution of statement
            Statement *stmt = parseStatement(scanner, token);
            if (stmt != nullptr) {
                stmt->execute(state, program);
                delete stmt;
            }
        }
    }
}

