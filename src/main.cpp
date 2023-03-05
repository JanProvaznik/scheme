#include <iostream>
#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include "reader.h"
#include "printer.h"

void run_tests();
// Scheme Interpreter implemented in C++

std::string read_file(const std::string & path)
{
  std::ifstream file(path);
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}
class Environment {
public:
    Environment() : outer(nullptr) {}
    Environment(std::shared_ptr<Environment> outer) : outer(outer) {}
    std::shared_ptr<Value> get(const std::string & key)
    {
        if (values.find(key) != values.end())
        {
            return values[key];
        }
        if (outer != nullptr)
        {
            return outer->get(key);
        }
        throw std::runtime_error("Key not found");
    }
    void set(const std::string & key, const std::shared_ptr<Value> & value)
    {
        values[key] = value;
    }
    std::shared_ptr<Environment> get_outer() const
    {
        return outer;
    }

private:
    std::map<std::string, std::shared_ptr<Value>> values;
    std::shared_ptr<Environment> outer;



};

class BaseEnvironment : public Environment
        {
    BaseEnvironment() : Environment(nullptr){


    }

        };


std::shared_ptr<Value> eval(const Environment& env, const std::string & source)
{
    // tokenize source
    Tokenizer tokenizer(source);
    // parse tokens into ast
    Reader reader;
    // evaluate ast
    return reader.read_form(tokenizer);


}

std::string read()
{
  // read source and return a list of tokens
  std::string line;
  std::getline(std::cin, line);
  return line;
}

void rep(const Environment& env){
  std::string source = read();
  pr_str(*eval(env, source));

}

void execute_file(std::string & path){
    Environment e;
    pr_str(*eval(e, read_file("path")));
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
    run_tests();
  if (argc == 1) {
    repl();
  }
  else if (argc == 2)
  {
    // run file given as an argument
    // read file
//    std::string source = read_file(argv[1]);
//    eval(Environment(), source);
      execute_file((std::string &) argv[1]);

  }

  return 0;
}
// I can't get the tests to work in their directory, so they are going to be here for now

void test_next_token()
{
    std::string source = "'(define (factorial n) (if (= n 0) 1 (* n (factorial (- n 1)))))";
    Tokenizer tokenizer(source);
    int i = 0;
    std::vector<TOKEN_TYPE> types = {TOKEN_TYPE::QUOTE,TOKEN_TYPE::LPAREN, TOKEN_TYPE::IDENTIFIER,
                                     TOKEN_TYPE::LPAREN, TOKEN_TYPE::IDENTIFIER, TOKEN_TYPE::IDENTIFIER,
                                     TOKEN_TYPE::RPAREN, TOKEN_TYPE::LPAREN, TOKEN_TYPE::IDENTIFIER, TOKEN_TYPE::LPAREN,
                                     TOKEN_TYPE::IDENTIFIER, TOKEN_TYPE::IDENTIFIER, TOKEN_TYPE::INTEGER, TOKEN_TYPE::RPAREN,
                                     TOKEN_TYPE::INTEGER, TOKEN_TYPE::LPAREN, TOKEN_TYPE::IDENTIFIER, TOKEN_TYPE::IDENTIFIER,
                                        TOKEN_TYPE::LPAREN, TOKEN_TYPE::IDENTIFIER, TOKEN_TYPE::LPAREN, TOKEN_TYPE::IDENTIFIER,
                                        TOKEN_TYPE::IDENTIFIER, TOKEN_TYPE::INTEGER, TOKEN_TYPE::RPAREN, TOKEN_TYPE::RPAREN,
                                        TOKEN_TYPE::RPAREN, TOKEN_TYPE::RPAREN, TOKEN_TYPE::RPAREN, TOKEN_TYPE::EOF_TOKEN};
    while (true)
    {
        auto [type, token] = tokenizer.peek_token();
        if (type != types[i++])
        {

            std::cout << "Error: " << token << " is not of the correct type " << std::endl;

        }
        if (type == TOKEN_TYPE::EOF_TOKEN)
        {
            break;
        }
        tokenizer.next_token();
//        std::cout << token << std::endl;
    }
}
//void test_tokenize()
//{
//    std::string source = "'(define (factorial n) (if (= n 0) 1 (* n (factorial (- n 1)))))";
//    Tokenizer tokenizer(source);
//    auto tokens = tokenizer.tokenize();
//    int i = 0;
//    std::vector<TOKEN_TYPE> types = {TOKEN_TYPE::QUOTE,TOKEN_TYPE::LPAREN, TOKEN_TYPE::IDENTIFIER,
//                                     TOKEN_TYPE::LPAREN, TOKEN_TYPE::IDENTIFIER, TOKEN_TYPE::IDENTIFIER,
//                                     TOKEN_TYPE::RPAREN, TOKEN_TYPE::LPAREN, TOKEN_TYPE::IDENTIFIER, TOKEN_TYPE::LPAREN,
//                                     TOKEN_TYPE::IDENTIFIER, TOKEN_TYPE::IDENTIFIER, TOKEN_TYPE::INTEGER, TOKEN_TYPE::RPAREN,
//                                     TOKEN_TYPE::INTEGER, TOKEN_TYPE::LPAREN, TOKEN_TYPE::IDENTIFIER, TOKEN_TYPE::IDENTIFIER,
//                                        TOKEN_TYPE::LPAREN, TOKEN_TYPE::IDENTIFIER, TOKEN_TYPE::LPAREN, TOKEN_TYPE::IDENTIFIER,
//                                        TOKEN_TYPE::IDENTIFIER, TOKEN_TYPE::INTEGER, TOKEN_TYPE::RPAREN, TOKEN_TYPE::RPAREN,
//                                        TOKEN_TYPE::RPAREN, TOKEN_TYPE::RPAREN, TOKEN_TYPE::RPAREN, TOKEN_TYPE::EOF_TOKEN};
//    for (auto [type, token] : tokens)
//    {
//        if (type != types[i++])
//        {
//            std::cout << "Error: " << token << " is not of the correct type " << std::endl;
//        }
//    }
//}


void run_tests(){
    test_next_token();
//    test_tokenize();
}
