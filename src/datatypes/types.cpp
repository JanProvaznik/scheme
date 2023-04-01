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

void ListValue::insert_value(size_t index, const std::shared_ptr<Value> &value) {
    values.insert(values.begin() + index, value);

}

void ListValue::set_value(size_t index, const std::shared_ptr<Value> &value) {
    values[index] = value;
}

void ListValue::pop_back() {
    values.pop_back();
}

std::shared_ptr<ListValue> ListValue::get_sublist(size_t start) const {
    auto result = std::make_shared<ListValue>();
    for (size_t i = start; i < values.size(); i++) {
        result->add_value(values[i]);
    }
    return result;
}

IntegerValue::IntegerValue(std::int64_t value) : NumberValue(ValueType::Integer) {
    this->value = value;
}
