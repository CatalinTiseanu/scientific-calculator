# Scientific calculator

Use ./build.sh to build on unix.
Use ./calculator "expression" to evaluate an expression.

One of the main goals of the project was to make it very easy to add additional mathematical
functions and operators.
Additionally, since full polynomials are supported it should be very easy to extend to solving
arbitrary degree polynomials.
Finally, over 30 scenarios of malformed expression erros are thrown in order to help the user
understand how to fix the error.

Supports 2 modes:

(1) Evaluate a standard expression containing the +, -, *, / operators, parantheses and arbitrary
mathematical functions.
Right now max, min, sin, cos, pow and log (base e) are supported.

This is the definition of the sin function for example:

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

This means that expressions:
    "x + x * (10 / cos(2)) = min(15, pow(2, 3))" is valid
    "x * x = 2" is invalid

## Testing

Testcases can be added in the Calculator::test() method
