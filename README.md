# Scientific calculator

## Description

Computes the result of an expression involving operators and functions.  

Example `sin(pow(( 4 - 9 / 100),  2)) - max(cos(12), 4 * 2)` equals `7.59236`.

Supports custom operators and functions.
Written in C++14 in November 2015.

Use ./build.sh to build on unix.
Use ./calculator "expression" to evaluate an expression.

One of the main goals of the project was to make it very easy to add additional mathematical
functions and operators. Additionally, since polynomials are supported it should be easy to
extend to solving arbitrary degree polynomials.
Finally, over 30 scenarios of malformed expression erros are thrown in order to help the user
understand how to fix the error.

## Functionality

Supports 2 modes:

(1) Evaluate a standard expression containing the +, -, *, / operators, parantheses and arbitrary
mathematical functions.
Right now max, min, sin, cos, pow and log (base e) are supported.

For example, this is the code for the `sin` function:

```
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
```

(2) Solve for the roots of degree 1 polynomial.

This supports all of the above functionalities except only addition and multiplication are allowed
on degree 1 polynomials.

This means that:  
* "x + x * (10 / cos(2)) = min(15, pow(2, 3))" is a valid expression
* "x * x = 2" is an invalid expression, since it's a second-degree polynomial in `x`

## Testing

Testcases can be added in the Calculator::test() method
