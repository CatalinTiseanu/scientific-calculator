
#include "Calculator.h"

Calculator MyCalculator;

int main(int argc, char* argv[]) {
    MyCalculator.test();

    if (argc == 1) {
        cout << "Usage: ./calculator \"expression\"" << "\n";
        cout << "Example: \"./calculator 3 + 4*5\"" << "\n";
    } else {
        string expression = "";

        for (int i = 1; i < argc; ++i)
            expression += string(argv[i]);

        cerr << "Evaluating expression: " << expression << "\n";
        cout << "Result: " + MyCalculator.eval(expression) << "\n";
    }

    return 0;
}