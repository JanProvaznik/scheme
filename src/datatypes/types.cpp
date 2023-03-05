#include <string>
#include <iostream>
#include "types.h"

void ListValue::add_value(const std::shared_ptr<Value> &value) {
    values.push_back(value);
}
