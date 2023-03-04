#include <string>
#include <iostream>
#include "datatypes/types.h"


std::string SymbolValue::to_string() const {
    return value;
}

std::string ListValue::to_string() const {
    std::string result = "(";
    for (auto& value : values)
    {
        result += value->to_string() + " ";
    }
    result += ")";
    return result;
}


std::string IntegerValue::to_string() const {
   return std::to_string(this->value);
}

