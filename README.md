# Scheme
**NPRG041 course project**

[Scheme programming language](https://standards.scheme.org/)  interpreter implemented in C++20.


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
  - [x] [tail call optimization](https://en.wikipedia.org/wiki/Tail_call)
- [x] control structures (if, cond, begin, let,...)
- [x] quoting
- [x] standard library functions 

Some of the library functions are possible to implement in Scheme itself, but here they are implemented in C++.

# How to use
CMake is used to build the project. When the executable is built 
simply use `./Scheme` to run the repl, or `./Scheme <file>` to interpret a .scm file.

There is a `Dockerfile` that can be used to build a docker image with the interpreter.
```bash
sudo docker build -t schemecpp .
sudo docker run -it schemecpp
```

# Developer notes

## High level documentation
The interpreter does the following steps, that are separated into various files.

1. Read the input [reader](src/reader.h)
2. Parse the input into an AST (abstract syntax tree) [parser](src/reader.h)
3. Evaluate the AST [eval](src/evaluator.cpp)
4. Print the result [printer](src/printer.cpp)

See the approximate illustration from `mal`:
![img](https://raw.githubusercontent.com/kanaka/mal/master/process/step5_tco.png)

### Reader and parser
Leverages the straightforward grammar of the Lisp family of languages to parse the input into an AST.

Contrary to most of the implementations, there are no linked lists and internally everything is stored in `std::vector`s.

### Evaluator
The evaluator is implemented as a recursive function, that takes the AST and the current environment as arguments.
The basic idea is to evaluate the AST in the following way:
1. If the AST is a symbol, look it up in the environment and return the value.
2. If the AST is a list, use the first element of the list as a function name, evaluate rest of the list and pass it to the function as arguments.
3. If the AST is a literal, return it.
4. (More complexity in cases of control structures)

During evaluation the environment can be modified by the `define` and `set!` keywords. Or another environment can be created and used by using `let`.

Note: Some of the recursion is omitted due to tail call optimization.

### Printing
The result of any evaluation is a `Value` object which has a to_string method. This method is used to print the result.

## Extending the interpreter 
Additional function symbols can be created by adding them to `BaseEnvironment` constructor in `src/datatypes/environment.cpp`.

Additional control structures can be added by modifying `eval` function in `src/eval.cpp`.

Additional data types can be added by extending `Value` class from `src/datatypes/types.h`. This will probably also require modifying the parser.

## Testing
The `tests` target in CMake builds and runs a test executable from `tests/test.cpp`.
There is also an example .scm file in `tests/` directory, which can be used to test support for running a file.

# Resources used:
- [mal - Make a Lisp](https://github.com/kanaka/mal)
- https://craftinginterpreters.com/
- https://standards.scheme.org/official/r3rs.pdf
- Daniel P. Friedman, Matthias Felleisen, The Little Schemer, 4th Edition
## License
MIT 
