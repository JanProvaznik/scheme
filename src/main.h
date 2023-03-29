#ifndef SCHEME_MAIN_H
#define SCHEME_MAIN_H
#include "util.h"
ValuePtr eval(const ValuePtr &ast, EnvironmentPtr &env);
ValuePtr apply(std::shared_ptr<FunctionValue> fn, std::vector<std::shared_ptr<Value> > args);


#endif //SCHEME_MAIN_H
