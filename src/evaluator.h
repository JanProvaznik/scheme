#ifndef SCHEME_EVALUATOR_H
#define SCHEME_EVALUATOR_H


#include "util.h"

ValuePtr apply_fn(const std::shared_ptr<FunctionValue> &fn, std::vector<std::shared_ptr<Value> > args);

std::shared_ptr<Value> eval_ast(const ValuePtr &value, const EnvironmentPtr &env);

std::pair<bool, ValuePtr> eval_list(ValuePtr &ast, EnvironmentPtr &env);

void eval_let(ValuePtr &ast, EnvironmentPtr &env, const std::shared_ptr<ListValue> &list_ast);

std::pair<bool, ValuePtr> eval_cond(ValuePtr &ast, const EnvironmentPtr &env, const std::shared_ptr<ListValue> &list_ast);

void eval_if(ValuePtr &ast, const EnvironmentPtr &env, const std::shared_ptr<ListValue> &list_ast);

void eval_begin(ValuePtr &ast, const std::shared_ptr<ListValue> &list_ast, const EnvironmentPtr &env);

ValuePtr eval(const ValuePtr &ast_in, const EnvironmentPtr &env_in);

#endif //SCHEME_EVALUATOR_H
