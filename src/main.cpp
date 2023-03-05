#include <iostream>
#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include "reader.h"
#include "printer.h"
#include "environment.h"

void run_tests();

std::string read_file(const std::string &path) {
  std::ifstream file(path);
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}



std::shared_ptr<Value> eval_list(std::shared_ptr<ListValue> list, const Environment &env) {
  if (list->size()!=0) {
    auto first = list->get_value(0);
    // if first is function then call it with the rest of the list
    if (first->get_type() == ValueType::Function) {
      auto function = std::static_pointer_cast<FunctionValue>(first);
      auto args = std::vector<std::shared_ptr<Value> >();
      for (size_t i = 1; i < list->size(); ++i) {
	args.push_back(list->get_value(i));
      }
      auto a = function->get_function()(args.size(), args);
      return a;

    }
  }
}
std::shared_ptr<Value> eval_ast(const std::shared_ptr<Value> &value, const Environment &env) {
  if (value->get_type() == ValueType::Symbol) {
    return env.get(std::static_pointer_cast<SymbolValue>(value)->to_string());
  }
  else if (value->get_type() == ValueType::List) {
    auto list = std::static_pointer_cast<ListValue>(value);
    auto result = std::make_shared<ListValue>();
    for (size_t i = 0; i < list->size(); ++i) {
      result->add_value(eval_ast(list->get_value(i), env));
    }
    return eval_list(result, env);
  }
  else {
    return value;
  }
}

std::shared_ptr<Value> eval(const Environment &env, const std::string &source) {
  // tokenize source
  Tokenizer tokenizer(source);
  // parse tokens into ast
  Reader reader;
  // evaluate ast
  auto parsed_expression = reader.read_form(tokenizer);
  return eval_ast(parsed_expression, env);


}

std::string read() {
  // read source and return a list of tokens
  std::string line;
  std::getline(std::cin, line);
  return line;
}

void rep(const Environment &env) {
  std::string source = read();
  pr_str(*eval(env, source));

}

void execute_file(std::string &path) {
  BaseEnvironment e;
  pr_str(*eval(e, read_file("path")));
}


void repl() {
  BaseEnvironment environment;
  while (true) {
    std::cout << ">> ";
    rep(environment);
  }

}

int main(int argc, char const *argv[]) {
  run_tests();
  if (argc == 1) {
    repl();
  } else if (argc == 2) {
    execute_file((std::string &) argv[1]);
  }
  else{
    std::cout << "Usage: " << argv[0] << " [file]" << std::endl;
    std::cout << "If no file is specified, the repl will be started" << std::endl;
    return 1;
  }

  return 0;
}
// I can't get the tests to work in their directory, so they are going to be here for now

void test_next_token() {
  std::string source = "'(define (factorial n) (if (= n 0) 1 (* n (factorial (- n 1)))))";
  Tokenizer tokenizer(source);
  int i = 0;
  std::vector<TOKEN_TYPE> types = {TOKEN_TYPE::QUOTE, TOKEN_TYPE::LPAREN, TOKEN_TYPE::IDENTIFIER,
                                   TOKEN_TYPE::LPAREN, TOKEN_TYPE::IDENTIFIER, TOKEN_TYPE::IDENTIFIER,
                                   TOKEN_TYPE::RPAREN, TOKEN_TYPE::LPAREN, TOKEN_TYPE::IDENTIFIER, TOKEN_TYPE::LPAREN,
                                   TOKEN_TYPE::IDENTIFIER, TOKEN_TYPE::IDENTIFIER, TOKEN_TYPE::INTEGER,
                                   TOKEN_TYPE::RPAREN,
                                   TOKEN_TYPE::INTEGER, TOKEN_TYPE::LPAREN, TOKEN_TYPE::IDENTIFIER,
                                   TOKEN_TYPE::IDENTIFIER,
                                   TOKEN_TYPE::LPAREN, TOKEN_TYPE::IDENTIFIER, TOKEN_TYPE::LPAREN,
                                   TOKEN_TYPE::IDENTIFIER,
                                   TOKEN_TYPE::IDENTIFIER, TOKEN_TYPE::INTEGER, TOKEN_TYPE::RPAREN,
                                   TOKEN_TYPE::RPAREN,
                                   TOKEN_TYPE::RPAREN, TOKEN_TYPE::RPAREN, TOKEN_TYPE::RPAREN, TOKEN_TYPE::EOF_TOKEN};
  while (true) {
    auto [type, token] = tokenizer.peek_token();
    if (type != types[i++]) {

      std::cout << "Error: " << token << " is not of the correct type " << std::endl;

    }
    if (type == TOKEN_TYPE::EOF_TOKEN) {
      break;
    }
    tokenizer.next_token();
//        std::cout << token << std::endl;
  }
}


void run_tests() {
  test_next_token();
}
