#include <string>
#include <iostream>
#include "datatypes/types.h"

void print(Value &value) {
  value.print();
}

std::string SymbolValue::to_string() const {
  return value;
}

std::string ListValue::to_string() const {
  std::string result = "(";
  for (auto &value: values) {
    result += value->to_string() + " ";
  }
  if (result.size() > 1) {
    result.pop_back();
  }
  result += ")";
  return result;
}


std::string StringValue::to_string() const {
  return value;
}

std::string BoolValue::to_string() const {
  return value ? "#t" : "#f";
}

std::string FloatValue::to_string() const {
  return std::to_string(this->value);
}


std::string IntegerValue::to_string() const {
  return std::to_string(this->value);
}


