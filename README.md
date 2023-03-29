# Scheme
NPRG041 course project
[Scheme](https://standards.scheme.org/) programming language interpreter implemented in C++20.

The interpreter should support features outlined in the [R3Rs standard](https://standards.scheme.org/official/r3rs.pdf): 
First goals:
- [ ] Handling different types of data, including numbers, booleans and lists
- [ ] Control structures such as if/else statements
- [ ] Defining and calling procedures, lambdas

The interpreter should also include appropriate error-handling to detect and report syntax errors in the Scheme code.

actually I'll try to implement even version 5, it's not that much more complex and the report is better

The project should include a comprehensive set of unit tests to ensure the correct behavior of the interpreter. The tests should cover all of the supported features of the interpreter and should be automated and easily runnable.


---

MVP:
parse to AST,

---
# done
- lexing
- parsing
- eval
- environment
- tail call optimization
- lambda, let, set!
- library functions
- if, cond


# TODO
- tests that are automated and have good coverage
- quote
- error checking
## maybe
- character constatnts #\
- hash (#) numbers (e,i,b,o,d,x)
- vector constants
- quasiquote
## bugs
- error on blank line
- make case-insensitive
- invalid parentheses in repl should throw error
# Resources used:
- [mal - Make a Lisp](https://github.com/kanaka/mal)
- https://craftinginterpreters.com/
- https://standards.scheme.org/official/r5rs.pdf
- Daniel P. Friedman, Matthias Felleisen, The Little Schemer, 4th Edition
## License
MIT 
