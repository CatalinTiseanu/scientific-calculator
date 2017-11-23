/*
    This file contains:
    (1) The Token struct which is used by the Tokenizer to parse a given expression into individual atomic parts
    (2) The Node class which represents and object in Reverse Polish Notation. It can be either:
        (a) a Scalar, which is represented as Polynomial of degree at most 2
        (b) A Function, which operates on Scalars

    AbstractNode is the base class for Scalar and Function.
    Function is the abstract base classes form which all functions are derived.

    In order to add another function:

    (1) Define a new FunctioncFUNC class derived from Function
    (2) Add the relevant code in the FunctionFactory class
*/

#include <cstdio>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <cassert>

#include <cmath>
#include <queue>
#include <stack>

#include <memory>
#include <string>
#include <vector>

#include "Polynomial.h"

using namespace std;

enum TokenType {TOKEN_WHITESPACE, TOKEN_COMMA, TOKEN_NUMBER, TOKEN_OPERATOR, TOKEN_FUNCTION, TOKEN_LEFT_PARANTHESES,
                TOKEN_RIGHT_PARANTHESES, TOKEN_VARIABLE, TOKEN_EQUAL_SIGN};

enum NodeType {NODE_SCALAR, NODE_FUNCTION};

#define EPS 1e-6

typedef Polynomial scalar;

struct Token {
    string identifier;
    double value;
    TokenType token_type;
    Token() {;}
    Token (const string & _indentifier, const TokenType _token_type) {
        identifier = _indentifier, token_type = _token_type;    
    }
};

//////////////////////////////////////////
//  Node abstract base class
//////////////////////////////////////////

class AbstractNode {
protected:
    NodeType type;
    int precedence;
    string identifier;
public:
    virtual ~AbstractNode () {;}
    NodeType get_type() {
        return type;
    }
    int get_precedence() {
        return precedence;
    }
    string get_identifier() {
        return identifier;
    }
};

//////////////////////////////////////////
//  Functions base class
//////////////////////////////////////////

class Function : public AbstractNode {    
protected:
    int arity;
    void check_arity(int num_scalars) const {
        if (num_scalars != arity) {
            throw string("Invalid number of parameters for " + identifier);
        }
    }
    void check_constants(const vector<scalar> & scalars) const {
        for (const auto & value : scalars) {
            if (!value.is_constant()) {
                throw string("Can't use " + identifier + " on polynomials of degree >= 2");    
            }
        }
    }
public:
    virtual scalar apply(const vector<scalar> & scalars) const = 0;

    Function () {
        type = NODE_FUNCTION;
    }

    int get_arity() {
        return arity;
    }
};

//////////////////////////////////////////
//  Mathematical operators
//////////////////////////////////////////

class FunctionAdd : public Function {
public:
    FunctionAdd () {
        arity = 2;
        precedence = 1;
        identifier = "+";
    }
    scalar apply(const vector<scalar> & scalars) const {
        check_arity (scalars.size());
        return scalars[0] + scalars[1];
    }
};

class FunctionSubstract: public Function {
public:
    FunctionSubstract () {
        arity = 2;
        precedence = 1;
        identifier = "-";
    }
    scalar apply(const vector<scalar> & scalars) const {
        check_arity (scalars.size());
        return scalars[0] - scalars[1];
    }
};

class FunctionMultiply: public Function {
public:
    FunctionMultiply () {
        arity = 2;
        precedence = 2;
        identifier = "*";
    }
    scalar apply(const vector<scalar> & scalars) const {
        check_arity (scalars.size());
        return scalars[0] * scalars[1];
    }
};

class FunctionDivide: public Function {
public:
    FunctionDivide () {
        arity = 2;
        precedence = 2;
        identifier = "/";
    }
    scalar apply(const vector<scalar> & scalars) const {
        check_arity (scalars.size());
        return scalars[0] / scalars[1];
    }
};

class FunctionNegate: public Function {
public:
    FunctionNegate () {
        arity = 1;
        precedence = 10;
        identifier = "~";
    }
    scalar apply(const vector<scalar> & scalars) const {
        check_arity (scalars.size());
        return Polynomial(-1) * scalars[0];
    }
};

//////////////////////////////////////////
//  Mathematical functions
//////////////////////////////////////////

class FunctionLog: public Function {
public:
    FunctionLog () {
        arity = 1;
        identifier = "log";
    }
    scalar apply(const vector<scalar> & scalars) const {
        check_arity (scalars.size());
        check_constants (scalars);

        if (scalars[0].get_0() < EPS) {
            throw string("Can't take logarithm a number less than or equal to 0");
        }
        return scalar(log(scalars[0].get_0()));
    }
};

class FunctionMax: public Function {
public:
    FunctionMax () {
        arity = 2;
        identifier = "max";
    }
    scalar apply(const vector<scalar> & scalars) const {
        check_arity (scalars.size());
        check_constants (scalars);

        return scalar(max(scalars[0].get_0(), scalars[1].get_0()));
    }
};

class FunctionMin: public Function {
public:
    FunctionMin () {
        arity = 2;
        identifier = "min";
    }
    scalar apply(const vector<scalar> & scalars) const {
        check_arity (scalars.size());
        check_constants(scalars);

        return scalar(min(scalars[0].get_0(), scalars[1].get_0()));
    }
};

class FunctionPow: public Function {
public:
    FunctionPow () {
        arity = 2;
        identifier = "pow";
    }
    scalar apply(const vector<scalar> & scalars) const {
        check_arity (scalars.size());
        check_constants(scalars);

        return scalar(pow(scalars[0].get_0(), scalars[1].get_0()));
    }
};

class FunctionSin: public Function {
public:
    FunctionSin () {
        arity = 1;
        identifier = "sin";
    }
    scalar apply(const vector<scalar> & scalars) const {
        check_arity (scalars.size());
        check_constants(scalars);
        return scalar(sin(scalars[0].get_0()));
    }
};

class FunctionCos: public Function {
public:
    FunctionCos () {
        arity = 1;
        identifier = "cos";
    }
    scalar apply(const vector<scalar> & scalars) const {
        check_arity (scalars.size());
        check_constants(scalars);
        return scalar(cos(scalars[0].get_0()));
    }
};

////////////////////////////////////////////////////////////////////
//  FunctionFactory as as a builder for the functions declared above
////////////////////////////////////////////////////////////////////

class FunctionFactory {
public:
    static unique_ptr<AbstractNode> build(const Token & token) {
        if (token.token_type == TOKEN_OPERATOR) {
            if (token.identifier == "+")
                return unique_ptr<Function>(new FunctionAdd());
            else if (token.identifier == "-")
                return unique_ptr<Function>(new FunctionSubstract());
            else if (token.identifier == "*")
                return unique_ptr<Function>(new FunctionMultiply());
            else if (token.identifier == "/")
                return unique_ptr<Function>(new FunctionDivide());
            else if (token.identifier == "~")
                return unique_ptr<Function>(new FunctionNegate());
            else
                throw string("Invalid mathematical operator " + token.identifier);
        } else if (token.token_type == TOKEN_FUNCTION) {
            if (token.identifier == "log")
                return unique_ptr<Function>(new FunctionLog());
            else if (token.identifier == "max")
                return unique_ptr<Function>(new FunctionMax());
            else if (token.identifier == "min")
                return unique_ptr<Function>(new FunctionMin());
            else if (token.identifier == "pow")
                return unique_ptr<Function>(new FunctionPow());
            else if (token.identifier == "sin")
                return unique_ptr<Function>(new FunctionSin());
            else if (token.identifier == "cos")
                return unique_ptr<Function>(new FunctionCos());
            else
                throw string("Invalid mathematical function " + token.identifier);
        }
    }
};

class Scalar : public AbstractNode {
private:
    scalar value;
    void parse(const Token & token) {
        if (token.token_type != TOKEN_NUMBER && token.token_type != TOKEN_VARIABLE) {
            throw string("Invalid polynomial value");
        }

        if (token.token_type == TOKEN_NUMBER) {
            value_type coeff0 = stod(token.identifier);
            value = Polynomial(coeff0);
        } else {
            value = Polynomial("x");
        }
    }
public:
    Scalar (const Token & token) {
        type = NODE_SCALAR;
        identifier = token.identifier;
        parse(token);
    }

    scalar get_value() {
        return value;
    }
};