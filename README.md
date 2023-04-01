# Scheme
NPRG041 course project
[Scheme](https://standards.scheme.org/) programming language interpreter implemented in C++20.

The interpreter should support features outlined in the [R3Rs standard](https://standards.scheme.org/official/r3rs.pdf): 

# Features
- [x] mostly compliant with R3Rs essential features
- [x] support for various data types:
  - [x] booleans
  - [x] integers
  - [x] floats
  - [x] symbols
  - [x] strings
  - [x] lists
- [x] defining and using variables
- [x] defining and using functions
  - [x] tail call optimization
- [x] control structures (if cond)
- [x] quoting
- [x] standard library functions 

The interpreter should also include appropriate error-handling to detect and report syntax errors in the Scheme code.

The project should include a comprehensive set of unit tests to ensure the correct behavior of the interpreter. The tests should cover all of the supported features of the interpreter and should be automated and easily runnable.

Some of the library functions are possible to implement in Scheme itself, but the implementation is mostly in C++.

# How to use
CMake is used to build the project. When the executable is built 
simply use `./scheme` to run the repl, or `./scheme <file>` to run a file.


# TODO
- tests that are automated and have good coverage
# Resources used:
- [mal - Make a Lisp](https://github.com/kanaka/mal)
- https://craftinginterpreters.com/
- https://standards.scheme.org/official/r5rs.pdf
- Daniel P. Friedman, Matthias Felleisen, The Little Schemer, 4th Edition
## License
MIT 
