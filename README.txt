- no operator precedence; left-to-right only (this is a Feature)
- grouping determined by spacing. Ex: 1/ 2+2 -> 0.25
- uses 128 bit DECIMAL floating point (34 digits)

Run `make`, then `./calc`
(requires a c compiler, and the readline library)
Code is in `calc.c`. `calc.c++` and `main.lua` were experiments.

Parsing:
spaces after an operator are treated as `(` (closed automatically at end of expression)
spaces before an infix operator are treated as `)` (if there are open parentheses)
Ex:
1+2 / 3+4 -> (1+2)/(3+4)
-1^ 2+2 /3 -> -1^(2+2)/3
- 5+10 -> -(5+10)

Operators:
infix: + - * / % ^
prefix: -
(more can be added easily)

Values:
numbers must start with a digit or decimal point, then parsed by `strtod`.
`a` refers to the result of the previous expression

reminded by https://github.com/ninedotnine/happy-space
