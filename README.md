# 24-Game-Solver
Get all algebraically unique solutions to the 24 Game in C++. https://repl.it/@lijt931/24-Game-Solver

## Usage and Demo
Run directly [here](https://repl.it/@lijt931/24-Game-Solver), or compile in C++17 or above and run.

Follow the instructions:
```
Please enter 4 cards in [1, 13] (e.g. 1 4 5 6) followed by an optional target (default 24):
1 2 3 4
#1: (1+3)*(2+4) = 24
#2: ((1*2)*3)*4 = 24
#3: ((1+2)+3)*4 = 24
```

Note that other equivalent solutions like `4*3*2*1` are omitted.

The cards and the target can be arbitrary real numbers, for example:

```
Please enter 4 cards in [1, 13] (e.g. 1 4 5 6) followed by an optional target (default 24):
-1 -2.5 3 4.5 5.5
#1: (-2.5*(-1-3))-4.5 = 5.5
```

## Algorithm
The algorithm consists of two parts.
The first part use brute-force to generate all possible solutions and the second part eliminates equivalent ones.

### 1 - Brute-force Solutions
Each permutation of the 4 cards is traversed, then each permutation of the operands (+, -, *, /), then each permutation of precedence.

### 2- Eliminate equivalent expressions
One way to achieve this is to use a polynomial class and define the equivalence criterion.
This algorithm uses a simpler way, which is assigning a transcendental number (e.g. PI, e) to each card value, and compare the arithmetic results.
