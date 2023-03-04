#include <string>
#include <iostream>
#include "types.h"

void ListValue::add_value(const std::shared_ptr<Value> & value) {
    this->values.push_back(value);
}
// fix the bug in the aforementioned snippet
