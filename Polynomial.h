/*
Standard Polynomial functionality
Supports addition, substraction, division, multiplication on degree 0 polynomials
Supports addition, multiplication on degree 1 polynomials
Supports solving for the roots of a degree 1 polynomial
*/

#include <vector>
#include <cmath>

using namespace std;

typedef double value_type;

#define POLYNOMIAL_EPS 1e-6

class Polynomial {
private:
    vector<value_type> coeff;
public:
    ~Polynomial () {;} 

    Polynomial() {
        coeff.resize(1);
        coeff[0] = 0;
    }

    Polynomial (const value_type & value) {
        coeff.resize(1);
        coeff[0] = value;
    }

    Polynomial (const vector<value_type> & values) {
        coeff = values;
    }

    Polynomial (string x) {
        coeff.resize(2);
        coeff[0] = 0;
        coeff[1] = 1;
    }

    Polynomial operator- (void) const {
        Polynomial result = *this;
        for (int i = 0; i < degree(); ++i)
            result.coeff[i] *= -1;
        return result;
    }

    Polynomial operator+ (const Polynomial & right) const {
        int max_degree = max (degree(), right.degree());
    
        vector<value_type> new_coeff (max_degree);

        for (int i = 0; i < max_degree; ++i) {
            if (i < degree())
                new_coeff[i] += coeff[i];
            if (i < right.degree())
                new_coeff[i] += right.coeff[i];
        }

        return Polynomial (new_coeff);
    } 

    Polynomial operator- (const Polynomial & right) const {
        // degree 0 only
        if (degree() > 1 && right.degree() > 1) {
            throw string ("Substraction not supported for polynomials of degree >= 1");
        }

        Polynomial result = *this;
        result.coeff[0] -= right.coeff[0];
        return result;
    } 

    Polynomial operator* (const Polynomial & right) const {
        if (degree() >= 2 && right.degree() >= 2) {
            throw string ("Multiplication of polynomials of degree >= 2 not allowed");
        }

        int max_degree = max (degree(), right.degree());

        vector<value_type> new_coeff (max_degree);

        if (degree() == max_degree) {
            new_coeff = coeff;
            for (int i = 0; i < degree(); ++i)
                new_coeff[i] *= right.coeff[0];
        } else {
            new_coeff = right.coeff;
            for (int i = 0; i < right.degree(); ++i)
                new_coeff[i] *= coeff[0];
        }

        return Polynomial (new_coeff);
    } 

    Polynomial operator/ (const Polynomial & right) const {
        // degree 0 only
        if (right.degree() > 1) {
            throw string ("Division not supported by polynomials of degree >= 1");
        }

        if (abs(right.coeff[0]) < POLYNOMIAL_EPS) {
            throw string ("Can't divide polynomial by 0");
        }

        Polynomial result = *this;
        for (int i = 0; i < degree(); ++i)
            result.coeff[i] /= right.coeff[0];
        return result;
    }

    int degree() const {
        return coeff.size();
    }

    bool is_constant() const {
        return coeff.size() == 1;
    }

    value_type get_0() const {
        return coeff[0];
    }

    vector<value_type> get_coeff() const {
        return coeff;
    }

    static Polynomial Zero() {
        return Polynomial (0);
    }

    value_type solve_degree_1() const {
        if (degree() == 0 || abs(coeff[1]) < POLYNOMIAL_EPS) {
            if (abs(coeff[0]) < POLYNOMIAL_EPS) {
                throw string ("Expression evaluates to 0, infinite number of solutions");
            } else {
                throw string ("Constant can't equal 0, no solutions");
            }
        }

        return -coeff[0] / coeff[1];
    }
};
