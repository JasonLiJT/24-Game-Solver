#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <set>

using namespace std;

constexpr double PI = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280;
double transcendental[4] = {PI, exp(1), log(PI), exp(PI)};

struct Card {
    double num;
    double symbol;
    bool operator<(const Card& other) const {
        return num < other.num;
    }
    Card operator+(const Card& other) const {
        return Card{num + other.num, symbol + other.symbol};
    }
    Card operator-(const Card& other) const {
        return Card{num - other.num, symbol - other.symbol};
    }
    Card operator*(const Card& other) const {
        return Card{num * other.num, symbol * other.symbol};
    }
    Card operator/(const Card& other) const {
        return Card{num / other.num, symbol / other.symbol};
    }
};

struct Expression {
    string str;
    double symbolEigen;
    bool operator<(const Expression& other) const {
        if (fabs(symbolEigen - other.symbolEigen) < 1e-6) {  // Equal
            return false;
        }
        return symbolEigen < other.symbolEigen;
    }
};

Card calculateFromOperator(const Card& left, const Card& right, char op) {
    switch (op) {
        case '+':
            return left + right;
        case '-':
            return left - right;
        case '*':
            return left * right;
        case '/':
            return left / right;
        default:
            cerr << "Error! Wrong symbol: " << op << endl;
            throw -1;
    }
}

int main() {
    cout << "Please enter 4 cards in [1, 13] (e.g. 1 4 5 6) followed by an optional target (default 24):" << endl;
    string input;
    stringstream ss;
    getline(cin, input);
    ss << input;
    Card cards[4] = {};
    double target;
    for (int i = 0; i < 4; ++i) {
        ss >> cards[i].num;
        if (abs(cards[i].num) == 1 || cards[i].num == 0) {
            cards[i].symbol = cards[i].num;
        } else {
            cards[i].symbol = transcendental[i];
        }
    }
    if (!(ss >> target)) {
        target = 24;
    }
    set<Expression> solutions;
    char operations[4] = {'+', '-', '*', '/'};
    int operationSlots[3] = {};
    int precedence[3] = {0, 1, 2};

    sort(cards, cards + 4);
    for (int i = 1; i < 4; ++i) {
        if (cards[i - 1].num == cards[i].num) {
            cards[i].symbol = cards[i - 1].symbol;
        }
    }
    sort(precedence, precedence + 3);
    do {  // Permutate cards
        for (int operationsCombination = 0; operationsCombination < 4 * 4 * 4; ++operationsCombination) {
            int tmp = operationsCombination;
            for (int i = 2; i >= 0; --i) {
                operationSlots[i] = tmp % 4;
                tmp /= 4;
            }
            do {  // Permutate precedence
                int calculated[4] = {};
                string subExpressions[4] = {};
                Expression expression;
                Card subResults[4];
                copy(cards, cards + 4, subResults);
                double result;
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
                    try {
                        subResults[current] = calculateFromOperator(left, right, operations[operationSlots[current]]);
                    } catch (int errCode) {
                        return errCode;
                    }
                    string tmpStr = tmpExpression.str();
                    if (i == 2) {
                        result = subResults[current].num;
                        expression.str = tmpStr;
                        expression.symbolEigen = subResults[current].symbol;
                        break;
                    }
                    for (int j = 0; j < 4; ++j) {
                        if (calculated[j] == i + 1) {
                            subResults[j] = subResults[current];
                            subExpressions[j] = tmpStr;
                        }
                    }
                }
                if (fabs(result - target) < 1e-6) {
                    solutions.insert(expression);
                }
            } while (next_permutation(precedence, precedence + 3));
        }
    } while (next_permutation(cards, cards + 4));
    if (solutions.empty()) {
        cout << "No solutions!" << endl;
    } else {
        int count = 0;
        for (const Expression& sol : solutions) {
            cout << "#" << ++count << ": " << sol.str << " = " << target << endl;
        }
    }

    return 0;
}
