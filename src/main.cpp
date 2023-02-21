#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// Scheme Interpreter implemented in C++

std::string read_file(const std::string & path)
{
  std::ifstream file(path);
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}
class Environment {
// idea: have a map that maps strings to vars
// var is a superclass of all types which are: number, string, pair,
// or var could be just a string that points to code
// which is better?
int a = 1;


};

class AST {
int b =2;

};

std::string eval(const Environment& env, const std::string & source)
{
  std::cout << source << std::endl;
  return source;
}

// so first I want a simple tokenizer that can give me a ast from the

std::string read()
{
  // read source and return a list of tokens
  std::string line;
  std::getline(std::cin, line);
  return line;
}

void print(const std::string & source)
{
  std::cout << source << std::endl;
}

void rep(const Environment& env){
  std::string source = read();
  // print evaled
  print(eval(env, source));

}


void repl(){
  Environment environment;
  while (true)
  {
    std::cout << ">> ";
    rep(environment);
  }

}

int main(int argc, char const *argv[])
{
  if (argc == 1) {
    repl();
  }
  else if (argc == 2)
  {
    // run file given as an argument
    // read file
    std::string source = read_file(argv[1]);
    eval(Environment(), source);

  }

  return 0;
}
///
///
///
// a header file for main :

// Scheme Interpreter implemented in C++

// a header file for environment:
