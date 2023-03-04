# Scheme
NPRG041 course project
[Scheme](https://standards.scheme.org/) programming language interpreter implemented in C++20.

The interpreter should support features outlined in the [R3Rs standard](https://standards.scheme.org/official/r3rs.pdf): 
First goals:
- [ ] Handling different types of data, including numbers, booleans and lists
- [ ] Control structures such as if/else statements
- [ ] Defining and calling precedures, lambdas

The interpreter should also include appropriate error-handling to detect and report syntax errors in the Scheme code.

actually I'll try to implement even version 5, it's not that much more complex and the report is better

The project should include a comprehensive set of unit tests to ensure the correct behavior of the interpreter. The tests should cover all of the supported features of the interpreter and should be automated and easily runnable.


---

MVP:
parse to AST,  

I want a class for 
- pair
- number types
- char
- bool
- lib functions
- vector
---
---
---
---
---
---
---
# done
- lexing


# TODO
- [ ] implement `eval` function
- make case-insensitive
- vector constants
# maybe
- character constatnts #\
- hash (#) numbers (e,i,b,o,d,x)




## License
MIT 
## Project status
If you have run out of energy or time for your project, put a note at the top of the README saying that development has slowed down or stopped completely. Someone may choose to fork your project or volunteer to step in as a maintainer or owner, allowing your project to keep going. You can also make an explicit request for maintainers.
