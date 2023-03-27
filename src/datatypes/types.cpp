#include <string>
#include <iostream>
#include "types.h"

void ListValue::add_value(const std::shared_ptr<Value> &value) {
  values.push_back(value);
}

std::shared_ptr<Value> ListValue::get_value(size_t index) const {
  return values[index];
}

size_t ListValue::size() const {
  return values.size();
}
IntegerValue::IntegerValue(std::int64_t value) : NumberValue(ValueType::Integer) {
  this->value = value;
}
