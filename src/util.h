#ifndef SCHEME_UTIL_H
#define SCHEME_UTIL_H

#include <memory>
#include <string>
#include "datatypes/environment.h"
#include "datatypes/types.h"

using EnvironmentPtr = std::shared_ptr<Environment>;
using ValuePtr = std::shared_ptr<Value>;
using ListPtr = std::shared_ptr<ListValue>;

template<typename T>
std::shared_ptr<T> car(const ValuePtr &list) {
    // cast to list
    auto list_value = std::static_pointer_cast<ListValue>(list);
    // return casted value
    return std::static_pointer_cast<T>(list_value->get_value(0));
}

ListPtr cdr(const ValuePtr &list);


bool check_signature(const std::vector<ValuePtr> &argv, const std::vector<ValueType> &types);
void check_arity(std::string &name, size_t expected, size_t actual);


#endif //SCHEME_UTIL_H
