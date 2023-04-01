#include "util.h"
#include "evaluator.h"
#include "datatypes/closure.h"
#include "datatypes/environment.h"
#include "printer.h"
#include <string>
#include <iostream>


ValuePtr apply_fn(const std::shared_ptr<FunctionValue>& fn, std::vector<std::shared_ptr<Value> > args) {
    return fn->get_function()(args.size(), args);
}

ValuePtr Closure::call(const std::vector<ValuePtr> &args) {
    // Create a new environment with the captured environment as the outer environment
    EnvironmentPtr new_env = std::make_shared<Environment>(env_);

    // Bind the arguments to the formal parameters in the new environment
    for (size_t i = 0; i < formal_params_->size(); ++i) {
        new_env->set(formal_params_->get_value(i)->to_string(), args[i]);
    }

    // Evaluate the body of the Closure with the new environment
    // evaluate all bodies and return the last one
    // for each expression in body
    auto body_list = std::static_pointer_cast<ListValue>(body_);
    for (int i = 0; i < body_list->size() - 1; ++i) {
        eval(body_list->get_value(i), new_env);
    }
    return eval(body_list->get_value(body_list->size() - 1), new_env);
}

std::shared_ptr<Value> eval_ast(const ValuePtr &value, const EnvironmentPtr& env) {
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

ValuePtr eval(const ValuePtr &ast_in, const EnvironmentPtr &env_in) {
    auto ast = ast_in;
    auto env = env_in;
    tco: // tail call optimization
    while (true) {
        if (ast->get_type() != ValueType::List)
            return eval_ast(ast, env);
            // if it's an empty list return unchanged
        else if (std::static_pointer_cast<ListValue>(ast)->size() == 0)
            return ast;
        else {
            auto list_ast = std::static_pointer_cast<ListValue>(ast);

            if (car<Value>(list_ast)->get_type() == ValueType::Symbol) {
                auto symbol = car<SymbolValue>(list_ast);
                auto symbol_name = symbol->to_string();
                // NOTE: function defines not supported
                if (symbol_name == "define") {
                    auto result = eval(list_ast->get_value(2), env);
                    env->set(list_ast->get_value(1)->to_string(), result);
                    return result;

                } else if (symbol_name == "let*" || symbol_name == "letrec" || symbol_name == "let") {

                    //   create a new environment using the current environment as the outer value and then use the first parameter as a list_ast of new bindings in the "let*" environment.
                    //   Take the second element of the binding list_ast, call EVAL using the new "let*" environment as the evaluation environment,
                    //   then call set on the "let*" environment using the first binding list_ast element as the key and the evaluated second element as the value. This is repeated for each odd/even pair in the binding list_ast. Note in particular, the bindings earlier in the list_ast can be referred to by later bindings. Finally, the second parameter (third element) of the original let* form is evaluated using the new "let*" environment and the result is returned as the result of the let* (the new let environment is discarded upon completion).
                    auto outer = env;
                    auto new_env = std::make_shared<Environment>(outer);
                    auto bindings_list = car<ListValue>(cdr(list_ast));
                    auto body = cdr(cdr(list_ast));
                    //iterate each binding
                    for (int i = 0; i < bindings_list->size(); ++i) {
                        auto name = car<SymbolValue>(bindings_list->get_value(i));
                        auto second_param = cdr(bindings_list->get_value(i))->get_value(0);
                        auto value = eval(second_param, new_env);
                        new_env->set(name->to_string(), value);
                    }
                    // many expressions can follow the bindings
                    for (int i = 0; i < body->size() - 1; ++i) {
                        eval(body->get_value(i), new_env);
                    }
                    ast = body->get_value(body->size() - 1);
                    env = new_env;
                    goto tco;

                } else if (symbol_name == "quote") {
                    return list_ast->get_value(1);
                } else if (symbol_name == "lambda") {
//                lambda: Return a new function Closure. The body of that Closure does the following:
//                1. Create a new environment using env (closed over from outer scope) as the outer parameter,
//                2. the first parameter (second list element of ast from the outer scope) as the binds parameter,
//                3. and the parameters to the Closure as the exprs parameter.
//                4. Call eval on the second parameter (third list element of ast from outer scope),
//                5. using the new environment. Use the result as the return value of the Closure.
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
                    ast = exprs->get_value(exprs->size() - 1);
                    goto tco;


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
                        ast = car<Value>(cdr(cdr(list_ast)));
                        goto tco;
                    } else {
                        ast = car<Value>(cdr(cdr(cdr(list_ast))));
                        goto tco;
                    }
                } else if (symbol_name == "cond") {
//                    Each 〈clause〉 should be of the form
//                            (〈test〉 〈expression〉 . . . )
//                    where 〈test〉 is any expression. The last 〈clause〉 may be
//                    an “else clause,” which has the form
//                            (else 〈expression1〉 〈expression2〉 . . . ).
//                    Semantics: A cond expression is evaluated by evaluating
//                    the 〈test〉 expressions of successive 〈clause〉s in order until
//                    one of them evaluates to a true value (see section 6.1).
//                    When a 〈test〉 evaluates to a true value, then the remaining
//                    〈expression〉s in its 〈clause〉 are evaluated in order, and the
//                    result of the last 〈expression〉 in the 〈clause〉 is returned
//                    as the result of the entire cond expression. If the selected
                    auto clauses = cdr(list_ast);
                    for (int i = 0; i < clauses->size(); ++i) {
                        auto clause = std::static_pointer_cast<ListValue>(clauses->get_value(i));
                        auto test = car<Value>(clause);
                        // evaluate test
                        if (eval(test, env)->is_true()) {
                            if (clause->size() == 1) { return std::make_shared<BoolValue>(true); }
                            auto exprs = cdr(clause);
                            for (int j = 0; j < exprs->size() - 1; ++j) {
                                eval(exprs->get_value(j), env);

                            }
                            ast = exprs->get_value(exprs->size() - 1);
                            goto tco;
                        }


                    }


                }


            }


            // otherwise
            // eval_list
            auto resolved_list = std::static_pointer_cast<ListValue>(eval_ast(ast, env));

            auto first = car<Value>(resolved_list);
            if (first->get_type() == ValueType::Function) {
                auto function = car<FunctionValue>(resolved_list);
                auto args = std::vector<ValuePtr>();

                for (size_t i = 1; i < resolved_list->size(); ++i) {
                    args.push_back(resolved_list->get_value(i));
                }
                return apply_fn(function, args);

            } else if (first->get_type() == ValueType::Closure) {


                auto closure = car<Closure>(resolved_list);
                auto args = cdr(resolved_list);
//                    regular function (not one defined by fn*): apply_fn/invoke it as before (in step 4).
//                    a fn* value: set ast to the ast attribute of f.
//                    Generate a new environment using the env and params attributes of f as the outer and binds arguments and args as the exprs argument.
//                    Set env to the new environment. Continue at the beginning of the loop.

                env = std::make_shared<Environment>(closure->get_env(), closure->get_formal_params(), args);
                auto body_list = std::static_pointer_cast<ListValue>(closure->get_body());
                for (int i = 0; i < body_list->size() - 1; ++i) {
                    eval(body_list->get_value(i), env);
                }
                // last expression in body
                ast = body_list->get_value(body_list->size() - 1);
                goto tco;

            } else {
                throw std::runtime_error(
                        "eval error: " + car<Value>(resolved_list)->to_string() + " is not a function");

            }
        }
    }  // end while loop
}