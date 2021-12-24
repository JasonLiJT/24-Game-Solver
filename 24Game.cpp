#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <set>
#include <array>

using namespace std;

constexpr double PI = 3.14159265358979323846;
std::array<double, 4> transcendental = {PI, exp(1), log(PI), exp(PI)};

bool isClose(double a, double b, double thresh = 1e-6) {
    return fabs(a - b) < thresh;
}

struct Card {
    double num = 0;
    double symbol = 0;
    void set(double num_, double transcendental) {
        num = num_;
        if (abs(num) == 1 || num == 0) {
            symbol = num;
        } else {
            symbol = transcendental;
        }
    }
    bool operator<(const Card& other) const {
        return num < other.num;
    }
#define CARD_OP_DEF(op)                                    \
Card operator op(const Card& other) const {                \
    return Card{num op other.num, symbol op other.symbol}; \
}
    CARD_OP_DEF(+);
    CARD_OP_DEF(-);
    CARD_OP_DEF(*);
    CARD_OP_DEF(/);
#undef CARD_OP_DEF
};

struct Expression {
    string str;
    double symbolEigen = 0;
    bool hasNonIntSubResult = false;
    bool operator<(const Expression& other) const {
        if (isClose(symbolEigen, other.symbolEigen)) {  // Equal
            return false;
        }
        return symbolEigen < other.symbolEigen;
    }
};

Card calculateFromOperator(const Card& left, const Card& right, char op) {
    switch (op) {
        case '+': return left + right;
        case '-': return left - right;
        case '*': return left * right;
        case '/': return left / right;
        default:
            cerr << "Error! Wrong symbol: " << op << endl;
            throw -1;
    }
}

class TwentyFourGameSolver {
   private:
    set<Expression> solutions;  // set<Expression> will ignore equivalent expressions
    std::array<Card, 4> cards = {};
    std::array<char, 4> operations = {'+', '-', '*', '/'};
    std::array<int, 3> operationSlots = {};
    std::array<int, 3> precedence = {0, 1, 2};
    double target;
    bool hardSolutionsOnly;
   public:
    static std::array<double, 5> readInput() {
        cout << "Please enter 4 cards in [1, 13] (e.g. 1 4 5 6) followed by an optional target (default 24):" << endl;
        std::array<double, 5> inputs = {};
        string tmp;
        getline(cin, tmp);
        stringstream ss(tmp);
        for (int i = 0; i < 4; ++i) {
            ss >> inputs[i];
        }
        if (!(ss >> inputs[4])) {
            inputs[4] = 24;
        }
        return inputs;
    }

    TwentyFourGameSolver() : TwentyFourGameSolver(readInput()) {}
    TwentyFourGameSolver(std::array<double, 5> inputs, bool hardSolutionsOnly = false)
        : operations{'+', '-', '*', '/'}
        , operationSlots{}
        , precedence{0, 1, 2}
        , target(inputs[4])
        , hardSolutionsOnly(hardSolutionsOnly)
    {
        for (int i = 0; i < 4; ++i) {
            cards[i].set(inputs[i], transcendental[i]);
        }
    }

    void solve() {
        // Make sure duplicate cards have the same symbol
        sort(begin(cards), end(cards));
        for (int i = 1; i < 4; ++i) {
            if (cards[i - 1].num == cards[i].num) {
                cards[i].symbol = cards[i - 1].symbol;
            }
        }
        sort(begin(precedence), end(precedence));
        do {  // Permutate cards
            solvePermutedCards();
        } while (next_permutation(begin(cards), end(cards)));

        printSolutions();
    }

    void printSolutions() {
        if (hardSolutionsOnly && (solutions.empty()
            || any_of(begin(solutions), end(solutions), [](const auto& e) { return !e.hasNonIntSubResult; }))
        ) {
            cerr << cards[0].num << ' ';
            return;
        }
        if (solutions.empty()) {
            cout << "No solutions!" << endl;
        } else {
            int count = 0;
            for (const Expression& sol : solutions) {
                cout << "#" << ++count << ": " << sol.str << " = " << target << endl;
            }
        }
    }

    void solvePermutedCards() {
        for (int operationsCombination = 0; operationsCombination < 4 * 4 * 4; ++operationsCombination) {
            // There are 4^3 ways to fill the 3 slots with the 4 arithmetic operations
            int tmp = operationsCombination;
            for (int i = 2; i >= 0; --i) {
                operationSlots[i] = tmp % 4;
                tmp /= 4;
            }
            do {  // Permutate precedence
                solvePermutedPrecedence();
            } while (next_permutation(begin(precedence), end(precedence)));
        }
    }

    void solvePermutedPrecedence() {
        std::array<int, 4> calculated = {};
        std::array<string, 4> subExpressions = {};
        Expression expression;
        std::array<Card, 4> subResults;
        copy(begin(cards), end(cards), begin(subResults));
        double result;
        bool hasNonIntSubResult = false;
        for (int i = 0; i < 3; ++i) {
            int current = precedence[i];
            Card left, right;
            stringstream tmpExpression;
            if (i < 2) {
                tmpExpression << "(";
            }
            auto markAsCalculated = [&](int current) {
                if (calculated[current]) {
                    tmpExpression << subExpressions[current];
                    int prev = calculated[current];
                    for (int j = 0; j < 4; ++j) {
                        if (calculated[j] == prev) {
                            calculated[j] = i + 1;
                        }
                    }
                } else {
                    tmpExpression << cards[current].num;
                    calculated[current] = i + 1;
                }
            };
            markAsCalculated(current);
            left = subResults[current];
            tmpExpression << operations[operationSlots[current]];
            markAsCalculated(current + 1);
            right = subResults[current + 1];
            if (i < 2) {
                tmpExpression << ")";
            }
            subResults[current] = calculateFromOperator(left, right, operations[operationSlots[current]]);
            if (double num = subResults[current].num;
                !hasNonIntSubResult && !isClose(num, round(num))
            ) {
                hasNonIntSubResult = true;
            }

            string tmpStr = tmpExpression.str();
            if (i == 2) {
                result = subResults[current].num;
                expression.str = tmpStr;
                expression.symbolEigen = subResults[current].symbol;
                expression.hasNonIntSubResult = hasNonIntSubResult;
                break;
            }
            for (int j = 0; j < 4; ++j) {
                if (calculated[j] == i + 1) {
                    subResults[j] = subResults[current];
                    subExpressions[j] = tmpStr;
                }
            }
        }
        if (isClose(result, target)) {
            // set<Expression> will ignore equivalent expressions
            solutions.insert(std::move(expression));
        }
    }
};

void findHardCards(double max_card = 13, double target = 24) {
    for(double i = 1; i <= max_card; ++i) {
        for(double j = i; j <= max_card; ++j) {
            for(double k = j; k <= max_card; ++k) {
                for(double l = k; l <= max_card; ++l) {
                    TwentyFourGameSolver solver{{i, j, k, l, target}, true};
                    solver.solve();
                }
            }
        }
    }
}

int main() {
    TwentyFourGameSolver solver;
    solver.solve();
    // findHardCards();
}
