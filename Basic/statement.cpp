/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include "statement.hpp"
#include <iostream>


/* Implementation of the Statement class */

int stringToInt(std::string str);

Statement::Statement() = default;

Statement::~Statement() = default;

// Global variables to manage control flow during program execution
int currentLine = -1;
int nextLine = -1;
bool shouldEnd = false;

void LetStatement::execute(EvalState &state, Program &program) {
    int value = exp->eval(state);
    state.setValue(varName, value);
}

void PrintStatement::execute(EvalState &state, Program &program) {
    int value = exp->eval(state);
    std::cout << value << std::endl;
}

void InputStatement::execute(EvalState &state, Program &program) {
    std::cout << " ? ";
    std::string input;
    std::getline(std::cin, input);

    // Try to parse the input as an integer
    try {
        int value = std::stoi(input);
        state.setValue(varName, value);
    } catch (...) {
        std::cout << "INVALID NUMBER" << std::endl;
        // Re-prompt for input
        execute(state, program);
    }
}

void EndStatement::execute(EvalState &state, Program &program) {
    shouldEnd = true;
}

void GotoStatement::execute(EvalState &state, Program &program) {
    nextLine = targetLine;
}

void IfStatement::execute(EvalState &state, Program &program) {
    int leftVal = lhs->eval(state);
    int rightVal = rhs->eval(state);
    bool condition = false;

    if (op == "=") {
        condition = (leftVal == rightVal);
    } else if (op == "<") {
        condition = (leftVal < rightVal);
    } else if (op == ">") {
        condition = (leftVal > rightVal);
    }

    if (condition) {
        nextLine = targetLine;
    }
}
