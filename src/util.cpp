#include "datatypes/types.h"
#include "util.h"
#include <algorithm>


void check_arity(std::string &name, size_t expected, size_t actual) {
    if (expected != actual) {
        throw std::runtime_error("Expected " + std::to_string(expected) + " arguments, got " + std::to_string(actual));
    }
}

ListPtr cdr(const ValuePtr &list) {
    auto list_value = std::static_pointer_cast<ListValue>(list);
    auto result = std::make_shared<ListValue>();
    for (size_t i = 1; i < list_value->size(); i++) {
        result->add_value(list_value->get_value(i));
    }
    return result;
}

bool check_signature(const std::vector<ValuePtr> &argv, const std::vector<ValueType> &types) {
    if (argv.size() != types.size()) {
        return false;
    }
    for (size_t i = 0; i < argv.size(); i++) {
        if (argv[i]->get_type() != types[i]) {
            return false;
        }
    }
    return true;
}


bool check_numeric_types(const std::vector<ValuePtr> &argv) {
     return std::all_of(argv.begin(), argv.end(), [](const ValuePtr &arg) {
         return arg->get_type() == ValueType::Integer || arg->get_type() == ValueType::Float;
     });
}
