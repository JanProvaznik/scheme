#include <iostream>
#include <string>
#include "reader.h"
#include "printer.h"
#include "environment.h"
#include "util.h"
#include "datatypes/Closure.h"


void run_tests();

ValuePtr eval(const ValuePtr &ast, EnvironmentPtr &env);

std::shared_ptr<Value> eval_ast(const ValuePtr &value, EnvironmentPtr env);

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


ValuePtr apply(std::shared_ptr<FunctionValue> fn, std::vector<std::shared_ptr<Value> > args) {
  return fn->get_function()(args.size(), args);
}


ValuePtr Closure::call(const std::vector<ValuePtr> &args) {
  // Create a new environment with the captured environment as the outer environment
  EnvironmentPtr new_env = std::make_shared<Environment>(env_);

  // Bind the arguments to the formal parameters in the new environment
  for (size_t i = 0; i < formal_params_->size(); ++i) {
    new_env->set(formal_params_->get_value(i)->to_string(), args[i]);
  }

  // Evaluate the body of the closure with the new environment
  // evaluate all bodies and return the last one
  // for each expression in body
  auto body_list = std::static_pointer_cast<ListValue>(body_);
  for (int i = 0; i < body_list->size() - 1; ++i) {
    eval(body_list->get_value(i), new_env);
  }
  return eval(body_list->get_value(body_list->size() - 1), new_env);
}

// precondition: the list is nonempty and evaluated
ValuePtr eval_list(std::shared_ptr<ListValue> list, EnvironmentPtr env) {
  auto first = list->get_value(0);
  if (first->get_type() == ValueType::Function) {
    auto function = car<FunctionValue>(list);
    auto args = std::vector<ValuePtr>();

    for (size_t i = 1; i < list->size(); ++i) {
      args.push_back(list->get_value(i));
    }
    return apply(function, args);

  } else if (first->get_type() == ValueType::Closure) {
    auto closure = car<Closure>(list);
    auto args = std::vector<ValuePtr>();

    for (size_t i = 1; i < list->size(); ++i) {
      args.push_back(list->get_value(i));
    }
    return closure->call(args);

  } else {
    throw std::runtime_error("eval error: " + list->get_value(0)->to_string() + " is not a function");

  }
}

std::shared_ptr<Value> eval_ast(const ValuePtr &value, EnvironmentPtr env) {
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
    auto list_ast = std::static_pointer_cast<ListValue>(ast);

    if (car<Value>(list_ast)->get_type() == ValueType::Symbol) {
      auto symbol = car<SymbolValue>(list_ast);
      auto symbol_name = symbol->to_string();
      if (symbol_name == "define") {
	// TODO figure out if this is a simple define or a function define
	auto result = eval(list_ast->get_value(2), env);

	env->set(list_ast->get_value(1)->to_string(), result);
	return result;

      } else if (symbol_name == "let") {
	// this doesn't seem important, for now just use let*
      } else if (symbol_name == "let*") {

	//   create a new environment using the current environment as the outer value and then use the first parameter as a list_ast of new bindings in the "let*" environment.
	//   Take the second element of the binding list_ast, call EVAL using the new "let*" environment as the evaluation environment,
	//   then call set on the "let*" environment using the first binding list_ast element as the key and the evaluated second element as the value. This is repeated for each odd/even pair in the binding list_ast. Note in particular, the bindings earlier in the list_ast can be referred to by later bindings. Finally, the second parameter (third element) of the original let* form is evaluated using the new "let*" environment and the result is returned as the result of the let* (the new let environment is discarded upon completion).
	auto outer = env;
	auto new_env = std::make_shared<Environment>(outer);
//            (let* ((c 2)) c) -> 2
	auto bindings_list = car<ListValue>(cdr(list_ast));
	auto body = cdr(cdr(list_ast));
	//iterate each binding
	for (int i = 0; i < bindings_list->size(); ++i) {
//                    auto name = std::static_pointer_cast<SymbolValue>(
//                            std::static_pointer_cast<ListValue>(bindings_list->get_value(i))->get_value(0));
	  // rewrite this line with car and cdr
	  auto name = car<SymbolValue>(bindings_list->get_value(i));
	  // rewrite this line with car and cdr
//                    auto second_param = std::static_pointer_cast<ListValue>(bindings_list->get_value(i))->get_value(1);
	  auto second_param = cdr(bindings_list->get_value(i))->get_value(0);
	  auto value = eval(second_param, new_env);                     //TODO check if eval is right
	  new_env->set(name->to_string(), value);
	}
	// many expressions can follow the bindings
	for (int i = 0; i < body->size() - 1; ++i) {
	  eval(body->get_value(i), new_env);
	}
	return eval(body->get_value(body->size() - 1), new_env);


      } else if (symbol_name == "quote") {
//            return list_ast->get_value(1);
      } else if (symbol_name == "lambda") {
//                lambda: Return a new function closure. The body of that closure does the following:
//                1. Create a new environment using env (closed over from outer scope) as the outer parameter,
//                2. the first parameter (second list element of ast from the outer scope) as the binds parameter,
//                3. and the parameters to the closure as the exprs parameter.
//                4. Call eval on the second parameter (third list element of ast from outer scope),
//                5. using the new environment. Use the result as the return value of the closure.
	auto binds = car<ListValue>(cdr(list_ast));
	auto exprs = cdr(cdr(list_ast));
	auto closure = std::make_shared<Closure>(env, binds, exprs);
	return closure;
      } else if (symbol_name == "begin") {
//                (begin 〈expression1〉 〈expression2〉 . . . ) library syntax
//                The 〈expression〉s are evaluated sequentially from left to
//                right, and the value(s) of the last 〈expression〉 is(are) returned.
//                This expression type is used to sequence side effects such as input and output.
	auto exprs = cdr(list_ast);
	for (int i = 0; i < exprs->size() - 1; ++i) {
	  eval(exprs->get_value(i), env);

	}
	return eval(exprs->get_value(exprs->size() - 1), env);


      } else if (symbol_name == "set!") {
	auto what = car<SymbolValue>(cdr(list_ast));
	auto value = eval(car<Value>(cdr(cdr(list_ast))), env);
//                env->set(what->to_string(), value);
	env->update_existing(what->to_string(), value);
	return value;
      } else if (symbol_name == "if") {
//                (if 〈test〉 〈consequent〉 〈alternate〉) syntax
//                        (if 〈test〉 〈consequent〉) syntax
//                Syntax: 〈Test〉, 〈consequent〉, and 〈alternate〉 may be arbitrary expressions.
//                        Semantics: An if expression is evaluated as follows: first,
//                〈test〉 is evaluated. If it yields a true value (see section 6.3.1), then 〈consequent〉 is evaluated and its value(s)
//                is(are) returned. Otherwise 〈alternate〉 is evaluated and its
//                value(s) is(are) returned. If 〈test〉 yields a false value and
//                no 〈alternate〉 is specified, then the result of the expression
//                is unspecified.
	auto test = eval(car<Value>(cdr(list_ast)), env);
	if (test->is_true()) {
	  return eval(car<Value>(cdr(cdr(list_ast))), env);
	} else {
	  return eval(car<Value>(cdr(cdr(cdr(list_ast)))), env);
	}

      }
    }

    auto resolved_list = std::static_pointer_cast<ListValue>(eval_ast(ast, env));
    // otherwise
    // eval the list_ast and then call the first element with the rest of the list_ast
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

void test_complex();

void car_cdr_test();


void run_tests() {
  test_complex();
  car_cdr_test();
}

void test_complex() {
  std::string source = "'(define (factorial n) (if (= n 0) 1 (* n (factorial (- n 1)))))";
  // with lambda so that define can be used
//    std::string source = "'(define factorial (lambda (n) (if (= n 0) 1 (* n (factorial (- n 1))))))";
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
  }
}

void car_cdr_test() {
  // test car and cdr
  auto list = std::make_shared<ListValue>();
  list->add_value(std::make_shared<IntegerValue>(1));
  list->add_value(std::make_shared<IntegerValue>(2));
  list->add_value(std::make_shared<IntegerValue>(3));

  auto car_result = car<IntegerValue>(list);
  auto cdr_result = cdr(list);

  if (car_result->get_value() != 1) {
    std::cout << "car test failed" << std::endl;
  }
  if (cdr_result->size() != 2) {
    std::cout << "cdr test failed" << std::endl;
  }
  if (cdr_result->get_value(0)->to_string() != "2") {
    std::cout << "cdr test failed" << std::endl;
  }
  if (cdr_result->get_value(1)->to_string() != "3") {
    std::cout << "cdr test failed" << std::endl;
  }

  // test car and cdr with a list of one element
  auto list2 = std::make_shared<ListValue>();
  list2->add_value(std::make_shared<IntegerValue>(1));
  auto car_result2 = car<IntegerValue>(list2);
  auto cdr_result2 = cdr(list2);

  if (car_result2->get_value() != 1) {
    std::cout << "car test failed" << std::endl;
  }
  if (cdr_result2->size() != 0) {
    std::cout << "cdr test failed" << std::endl;
  }
}

//  (define d (lambda (x) (+ x x)))
//  (d 5)

// turns out this is probably not needed with sufficient macro support, if I want to reimplement put this to eval within the block of keywords
// same can probably be done for begin
//else if (symbol_name == "cond") {
//                (cond 〈clause1〉 〈clause2〉 . . . ) library syntax
//                Syntax: Each 〈clause〉 should be of the form
//                        (〈test〉 〈expression1〉 . . . )
//                where 〈test〉 is any expression. Alternatively, a 〈clause〉
//                may be of the form
//                        (〈test〉 => 〈expression〉)
//                The last 〈clause〉 may be an “else clause,” which has the
//                form
//                        (else 〈expression1〉 〈expression2〉 . . . ).
//                Semantics: A cond expression is evaluated by evaluating
//                the 〈test〉 expressions of successive 〈clause〉s in order until
//                one of them evaluates to a true value (see section 6.3.1).
//                When a 〈test〉 evaluates to a true value, then the remain-ing 〈expression〉s in its 〈clause〉 are evaluated in order,
//                and the result(s) of the last 〈expression〉 in the 〈clause〉
//                is(are) returned as the result(s) of the entire cond expression. If the selected 〈clause〉 contains only the 〈test〉 and no
//                〈expression〉s, then the value of the 〈test〉 is returned as the
//                result.
//auto clauses = cdr(list_ast);
//for (int i = 0; i < clauses->size(); ++i) {
//auto clause = car<ListValue>(clauses->get_value(i));
//auto test = car<Value>(clause);
//auto rest = cdr(clause);
//if (test->is_true()) {
//if (rest->size() == 2 && rest->get_value(1)->to_string() == "=>") {

//                If the selected 〈clause〉 uses the => alternate form,
//                then the 〈expression〉 is evaluated. Its value must be a procedure that accepts one argument; this procedure is then
//                called on the value of the 〈test〉 and the value(s) returned
//                by this procedure is(are) returned by the cond expression.
//                If all 〈test〉s evaluate to false values, and there is no else
//                clause, then the result of the conditional expression is un-specified; if there is an else clause, then its 〈expression〉s are
//                evaluated, and the value(s) of the last one is(are) returned.
//}
//for (int j = 0; j < rest->size() - 1; ++j) {
//eval(rest->get_value(j), env);
//}
//return eval(rest->get_value(rest->size() - 1), env);
//}
//}
//return std::make_shared<NilValue>();
////
//
//}