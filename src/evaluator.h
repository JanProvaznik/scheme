#ifndef SCHEME_EVALUATOR_H
#define SCHEME_EVALUATOR_H


#include "util.h"
ValuePtr apply_fn(const std::shared_ptr<FunctionValue>& fn, std::vector<std::shared_ptr<Value> > args);

std::shared_ptr<Value> eval_ast(const ValuePtr &value, const EnvironmentPtr& env);

ValuePtr eval(const ValuePtr &ast_in, const EnvironmentPtr &env_in);


#endif //SCHEME_EVALUATOR_H
