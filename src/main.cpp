#include <iostream>
#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include "reader.h"
#include "printer.h"
#include "environment.h"

using EnvironmentPtr = std::shared_ptr<Environment>;
using ValuePtr = std::shared_ptr<Value>;

void run_tests();

ValuePtr eval(const ValuePtr &ast, EnvironmentPtr &env);

std::shared_ptr<Value> read_file(const std::string &path) {
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    // tokenize source
    Tokenizer tokenizer(buffer.str());
    // parse tokens into ast
    Reader reader;
    return reader.read_form(tokenizer);


}

std::vector special_forms = {
        "let",
        "let*",
        "define",
        "set!", // almost the same as define actually
};

// TODO: car cdr cons eq?, type predicates
std::shared_ptr<Value> apply(std::shared_ptr<FunctionValue> fn, std::vector<std::shared_ptr<Value> > args) {
    return fn->get_function()(args.size(), args);
}


// precondition: the list is nonempty and evaluated
ValuePtr eval_list(std::shared_ptr<ListValue> list, EnvironmentPtr env) {
    auto first = list->get_value(0);
    if (first->get_type() == ValueType::Function) {
        auto function = std::static_pointer_cast<FunctionValue>(first);
        auto args = std::vector<ValuePtr>();
        for (size_t i = 1; i < list->size(); ++i) {
            args.push_back(list->get_value(i));
        }
        return apply(function, args);

    } else {
        //???

    }
}

std::shared_ptr<Value> eval_ast(const ValuePtr &value, EnvironmentPtr env) {
//    if (value->get_type() == ValueType::Symbol) {
//        // if special form then return
//        if (std::find(special_forms.begin(), special_forms.end(), value->to_string()) != special_forms.end())
//            return value;
//
//        return env->get(std::static_pointer_cast<SymbolValue>(value)->to_string());
//    } else
    if (value->get_type() == ValueType::List) {
        auto list = std::static_pointer_cast<ListValue>(value);
        auto list_with_evaluated_content = std::make_shared<ListValue>();
        for (size_t i = 0; i < list->size(); ++i) {
            list_with_evaluated_content->add_value(eval(list->get_value(i), env));
        }
        return list_with_evaluated_content;
    } else if (value->get_type() == ValueType::Symbol) {
        return env->get(std::static_pointer_cast<SymbolValue>(value)->to_string());

    } else {
        return value;
    }
}

ValuePtr eval(const ValuePtr &ast, EnvironmentPtr &env) {
    if (ast->get_type() != ValueType::List)
        return eval_ast(ast, env);
        // if it's an empty list return unchanged
    else if (ast->get_type() == ValueType::List && std::static_pointer_cast<ListValue>(ast)->size() == 0)
        return ast;
    else {
        auto list = std::static_pointer_cast<ListValue>(ast);

        // if first element is a special form handle differently
        if (list->get_value(0)->get_type() == ValueType::Symbol) {
            auto symbol = std::static_pointer_cast<SymbolValue>(list->get_value(0));
            auto symbol_name = symbol->to_string();
            if (symbol_name == "define") {
                //FIXME: throw if list is not of size 3
                auto result = eval(list->get_value(2), env);

                env->set(list->get_value(1)->to_string(), result);
                return result;

            } else if (symbol_name == "let") {
                // this doesn't seem important, for now just use let*
            } else if (symbol_name == "let*") {

                //   create a new environment using the current environment as the outer value and then use the first parameter as a list of new bindings in the "let*" environment.
                //   Take the second element of the binding list, call EVAL using the new "let*" environment as the evaluation environment,
                //   then call set on the "let*" environment using the first binding list element as the key and the evaluated second element as the value. This is repeated for each odd/even pair in the binding list. Note in particular, the bindings earlier in the list can be referred to by later bindings. Finally, the second parameter (third element) of the original let* form is evaluated using the new "let*" environment and the result is returned as the result of the let* (the new let environment is discarded upon completion).
                auto outer = env;
                auto new_env = std::make_shared<Environment>(outer);
//            (let* ((c 2)) c) -> 2
                auto bindings_list = std::static_pointer_cast<ListValue>(list->get_value(1));
                auto body = list->get_value(2);
                //iterate each binding
                for (int i = 0; i < bindings_list->size(); ++i) {
                    auto name = std::static_pointer_cast<SymbolValue>(
                            std::static_pointer_cast<ListValue>(bindings_list->get_value(i))->get_value(0));
                    auto second_param = std::static_pointer_cast<ListValue>(bindings_list->get_value(i))->get_value(1);
                    auto value = eval(second_param, new_env); //TODO check if eval is right
                    new_env->set(name->to_string(), value);

                }

                return eval(body, new_env);


            } else if (symbol_name == "quote") {
//            return list->get_value(1);
            } else if (symbol_name == "lambda") {

            } else {
//                throw std::runtime_error("Unknown symbol form");
            }
        }

        auto resolved_list = std::static_pointer_cast<ListValue>(eval_ast(ast, env));
        // otherwise
        // eval the list and then call the first element with the rest of the list
        return eval_list(resolved_list, env);
    }


}

ValuePtr read() {
    // read source and return a list of tokens
    std::string line;
    std::getline(std::cin, line);
    // tokenize source
    Tokenizer tokenizer(line);
    // parse tokens into ast
    Reader reader;
    return reader.read_form(tokenizer);
}

void rep(EnvironmentPtr &env) {
    print(*eval(read(), env));

}

void execute_file(std::string &path) {
    BaseEnvironment e;
    EnvironmentPtr eptr = std::make_shared<BaseEnvironment>(e);
    print(*eval(read_file(path), eptr));
}


void repl() {
    BaseEnvironment environment;
    EnvironmentPtr environment_ptr = std::make_shared<BaseEnvironment>(environment);
    while (true) {
        std::cout << ">> ";
        rep(environment_ptr);
    }

}

int main(int argc, char const *argv[]) {
    run_tests();
    if (argc == 1) {
        repl();
    } else if (argc == 2) {
        execute_file((std::string &) argv[1]);
    } else {
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
