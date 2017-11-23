
#include "Node.h"

#include <sstream>

#define LEFT_PARANTHESES '('
#define RIGHT_PARANTHESES ')'
#define COMMA ','
#define EQUAL_SIGN '='
#define MINUS_SIGN '-'
#define VARIABLE 'x'

class Calculator {
private:
    bool verbose;

    // Parses a floating point number from the beginning of a string
    string parse_number(const string & expression);

    // Gets the next token from the expression 
    Token get_token(string & expression, bool expect_operator);

    // Tokenizes the given expression
    // example: For "4 +7=10" it returns {4,whitespace,+,7,=,10}

    vector<Token> tokenize_expression(const string & expression);

    // Build the reverse polish notation of the expression using the Shunting-yard algorithm
    vector<unique_ptr<AbstractNode>> build_reverse_polish_notation(const vector<Token> & tokens);

    // Computes the result polynomial from an expression in reverse polish notation
    scalar process_reverse_polish_notation(const vector<unique_ptr<AbstractNode>> & output_queue);

    // Evaluates an expression support 2 modes:
    // 1. Standard evaluation of an expression consisting only of constants
    // 2. Solving for the root of an expression consisting of 

    value_type compute_constant_result(const string & expression);
public:
    // Evaluates an expression support 2 modes:
    // 1. Standard evaluation of an expression consisting only of constants
    // 2. Solving for the root of an expression consisting of 

    string eval(const string & expression);

    void test();

    Calculator () {;}
    Calculator (bool _verbose);
}; 

//////////////////////////////////////////////////////////////

Calculator::Calculator(bool _verbose) {
    verbose = _verbose;
}

string Calculator::parse_number(const string & expression) {
    unsigned int j = 1;
    int number_dots = 0;
    while (j < expression.size()) {
        if (isalpha(expression[j]) || expression[j] == LEFT_PARANTHESES) {
            throw string("Invalid floating number: contains invalid characters");
        } else if (!isdigit(expression[j]) && expression[j] != '.') {
            break;
        }

        number_dots += expression[j] == '.';
        ++j;
    }

    if (number_dots > 1) {
        throw string("Invalid floating number: too many dots");
    }

    return expression.substr(0, j);
}

Token Calculator::get_token(string & expression, bool expect_operator) {
    if (expression[0] == ' ' || expression[0] == '\t') {
        // whitespace
        expression.erase(0, 1);
        return Token ("whitespace", TOKEN_WHITESPACE);
    } else if (expression[0] == COMMA) {
        // comma
         expression.erase(0, 1);
        return Token (string(1, COMMA), TOKEN_COMMA);
    } else if (isdigit(expression[0])) {
        // number
        string identifier = parse_number (expression);
        expression.erase(0, identifier.size());
        return Token (identifier, TOKEN_NUMBER);
    } else if (expression[0] == VARIABLE && !(expression.size() > 2 && isalpha(expression[1]))) {
        // x variable
        string identifier = string(1, expression[0]); 
        expression.erase(0, 1);
        return Token (identifier, TOKEN_VARIABLE);
    } else if (isalpha(expression[0])) {
        // function
        unsigned int j = 1;
        while (j < expression.size()) {
            if (!isalpha(expression[j])) {
                if (expression[j] != LEFT_PARANTHESES && expression[j] != ' ' && expression[j] != '\t') {
                    throw string("Invalid function definition");
                }
                break;
            }
            ++j;
        }

        string identifier = expression.substr(0, j);
        expression.erase(0, j);
        return Token (identifier, TOKEN_FUNCTION);
    } else if (expression[0] == LEFT_PARANTHESES) {
        // (
        string identifier = string(1, expression[0]); 
        expression.erase(0, 1);
        return Token (identifier, TOKEN_LEFT_PARANTHESES);
    } else if (expression[0] == RIGHT_PARANTHESES) {
        // )
        string identifier = string(1, expression[0]);
        expression.erase(0, 1);
        return Token (identifier, TOKEN_RIGHT_PARANTHESES);
    } else if (expression[0] == EQUAL_SIGN) {
        // =
        string identifier = string(1, expression[0]);
        expression.erase(0, 1);
        return Token (identifier, TOKEN_EQUAL_SIGN);
    } else if (expression[0] == MINUS_SIGN && !expect_operator) {
        // negation sign
        string identifier = string("~");
        expression.erase(0, 1);
        return Token (identifier, TOKEN_OPERATOR);
    } else {
        // operator
    
        if (expression[0] == '+' || expression[0] == '-' || expression[0] == '*' || expression[0] == '/') {
            auto identifier = string(1, expression[0]);
            expression.erase(0, 1);
            return Token (identifier, TOKEN_OPERATOR);
        } else {
            throw string("Invalid operator");
        }
    }
}

vector<Token> Calculator::tokenize_expression(const string & expression) {
    auto remaining_expression = expression;

    vector<Token> tokens;

    bool expect_operator = false;

    while (remaining_expression.size() > 0) {
        Token token;

        try {
            token = get_token(remaining_expression, expect_operator);
            tokens.push_back(token);
        } catch(string error) {
            cout << "Error: " + error << "\n";
            exit(0);
        }

        if (token.token_type == TOKEN_RIGHT_PARANTHESES || token.token_type == TOKEN_NUMBER || token.token_type == TOKEN_VARIABLE) {
            expect_operator = true;
        } else if (token.token_type != TOKEN_WHITESPACE) {
            expect_operator = false;
        }
    }

    return tokens;
}

vector<unique_ptr<AbstractNode>> Calculator::build_reverse_polish_notation(const vector<Token> & tokens) {
    if (verbose) {
        cerr << "Bulding reverse polish notation\n";
    }

    vector<unique_ptr<AbstractNode>> output_queue;
    stack<Token> buffer;

    for (const auto & token : tokens) {
        if (verbose) {
            cerr << "Processing: " << token.identifier << " " << token.token_type << "\n";
        }

        if (token.token_type == TOKEN_WHITESPACE) {
            continue;
        } else if (token.token_type == TOKEN_NUMBER) {
            output_queue.push_back(unique_ptr<AbstractNode>(new Scalar(token)));
        } else if (token.token_type == TOKEN_VARIABLE) {
            output_queue.push_back(unique_ptr<AbstractNode>(new Scalar(token)));
        } else if (token.token_type == TOKEN_OPERATOR) {
            auto next_operator = FunctionFactory::build(token);
            while (!buffer.empty() && buffer.top().token_type == TOKEN_OPERATOR) {
                auto peek_operator = FunctionFactory::build(buffer.top());
                if (peek_operator->get_precedence() >= next_operator->get_precedence()) {   
                    output_queue.push_back(move(peek_operator));
                    buffer.pop();
                } else {
                    break;
                }
            }
            buffer.push(token);
        } else if (token.token_type == TOKEN_FUNCTION) {
            buffer.push(token);
        } else if (token.token_type == TOKEN_COMMA) {
            while (!buffer.empty() && buffer.top().token_type != TOKEN_LEFT_PARANTHESES) {
                output_queue.push_back(FunctionFactory::build(buffer.top()));
                buffer.pop();
            }
            if (buffer.empty()) {
                throw string("Invalid function declaration: missing left parantheses");
            }
        } else if (token.token_type == TOKEN_LEFT_PARANTHESES) {
            buffer.push(token);
        } else if (token.token_type == TOKEN_RIGHT_PARANTHESES) {
            while (!buffer.empty() && buffer.top().token_type != TOKEN_LEFT_PARANTHESES) {
                output_queue.push_back(FunctionFactory::build(buffer.top()));
                buffer.pop();
            }

            if (buffer.empty()) {
                throw string("Invalid parantheses: missing left parantheses");
            }

            buffer.pop();

            // TODO: IF FUNCTION POP ONTO OUTPUT
            if (!buffer.empty() && buffer.top().token_type == TOKEN_FUNCTION) {
                output_queue.push_back(FunctionFactory::build(buffer.top()));
                buffer.pop();
            }
        } else {
            throw string("Unknown token: " + token.identifier);
        }
    }

    // Push the remaining operators onto the output_queue
    while (!buffer.empty()) {
        if (buffer.top().token_type == TOKEN_LEFT_PARANTHESES || buffer.top().token_type == TOKEN_RIGHT_PARANTHESES) {
            throw string("Mismatched parantheses");
        }
        output_queue.push_back(FunctionFactory::build(buffer.top()));
        buffer.pop();
    }

    if (verbose) {
        cerr << "Finished building Reverse Polish Notation\n";
    }

    return output_queue;
}

scalar Calculator::process_reverse_polish_notation(const vector<unique_ptr<AbstractNode>> & output_queue) {
    if (verbose) {
        cerr << "Process reverse polish notation\n";
    }

    scalar result = scalar::Zero();

    stack<scalar> buffer;

    for (unsigned int i = 0; i < output_queue.size(); ++i) {
        if (output_queue[i]->get_type() == NODE_SCALAR) {
            Scalar * current_scalar = dynamic_cast<Scalar*>(output_queue[i].get());
            buffer.push(current_scalar->get_value());
        } else {
            Function * current_function = dynamic_cast<Function*>(output_queue[i].get());
            vector<scalar> operands;
            for (int nr_operand = 0; nr_operand < current_function->get_arity(); ++nr_operand) {
                if (buffer.empty()) {
                    throw string("Insufficient number of operands for " + current_function->get_identifier());
                }
                operands.push_back(buffer.top());
                buffer.pop();
            }
            
            reverse(operands.begin(), operands.end());
            
            try {
                scalar cur_result = current_function->apply(operands);
                buffer.push(cur_result);
            } catch (string error) {
                cerr << error << "\n";
                exit(0);
            }
        }
    }

    if (buffer.empty()) {
        throw string("Insufficient scalars left");
    }

    result = buffer.top();
    buffer.pop();

    if (!buffer.empty()) {
        throw string("Too many scalars left");
    }        

    return result;
}

value_type Calculator::compute_constant_result(const string & expression) {
    // get tokens for the expression
    vector<Token> tokens;
    try {
        tokens = tokenize_expression(expression);
    } catch (string error) {
        throw string("Error in tokenizer: " + error + "\n");
    }

    if (verbose) {
        cerr << "Tokenizer finished:" << "\n";    
        for (const auto & token : tokens)
            cerr << "Token: " << token.identifier << " " << token.token_type << "\n";
    }

    // Decide on the type of expression (compute value or solve for x)

    int nr_equal_signs = 0;
    bool contains_variable = false;

    for (const auto & token : tokens) {
        nr_equal_signs += token.token_type == TOKEN_EQUAL_SIGN;
        contains_variable |= token.token_type == TOKEN_VARIABLE;
    }

    if (nr_equal_signs > 1) {
        throw string("Expression contains too many equal signs");
    }

    // If it contains a variable it must contain and equal sign and vicevers
    if ((contains_variable && !nr_equal_signs) || (!contains_variable && nr_equal_signs)) {
        throw string("Expression must contain both a variable and equal sign or neither");
    }

    // Change equal sign to minus and proceed as before
    for (unsigned int i = 0; i < tokens.size(); ++i) {
        if (tokens[i].token_type == TOKEN_EQUAL_SIGN) {
            tokens[i].token_type = TOKEN_OPERATOR;
            tokens[i].identifier = "-";
        }
    }

    bool is_equation = contains_variable;

    vector<unique_ptr<AbstractNode>> output_queue;

    try {
        output_queue = build_reverse_polish_notation(tokens);

        if (verbose) {
            for (unsigned int i = 0; i < output_queue.size(); ++i) {
                cerr << output_queue[i]->get_identifier() << " ";
            }
            cerr << "\n";
        }
    } catch (string error) {
        throw string("Error in building reverse polish notation: " + error);
    }

    scalar result;

    try {
        result = process_reverse_polish_notation (output_queue);
    } catch (string error) {
        throw string("Error in processing reverse polish notation: " + error);
    }

    value_type final_result;

    if (!is_equation) {
        final_result = result.get_0();
    } else {
        if (verbose) {
            cerr << "Final polynomial: ";
            auto coeff = result.get_coeff();
            for (unsigned int i = 0; i < coeff.size(); ++i)
                cerr << coeff[i] << " ";
            cerr << "\n";
        }
        try {
            final_result = result.solve_degree_1();
        } catch(string error) {
            throw error;
        }
    }

    return final_result;
}

string Calculator::eval(const string & expression) {
    try {
        auto result = compute_constant_result (expression); 
    
        stringstream ss;
        ss << result;
        return ss.str();
    } catch (string error) {
        return error;
    }
}

void Calculator::test() {
    assert (eval("4 + 9") == "13");

    assert (eval("x + 5 = 11") == "6");
    assert (eval("x * 0 = 10") == "Constant can't equal 0, no solutions");
    
    assert (eval("=") == "Expression must contain both a variable and equal sign or neither");

    assert (eval("max(1)") == "Error in processing reverse polish notation: Insufficient number of operands for max");

    assert (eval("x + x * (10 / cos(2)) = min(15, pow(2, 3))") == "-0.347373");

    assert (eval("(5") == "Error in building reverse polish notation: Mismatched parantheses");

    assert (eval("lag(10)") == "Error in building reverse polish notation: Invalid mathematical function lag");
}