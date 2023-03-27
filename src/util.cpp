#include "util.h"




ListPtr cons(const ValuePtr &value, const ListPtr &list) {
  auto result = std::make_shared<ListValue>();
  result->add_value(value);
  for (size_t i = 0; i < list->size(); i++) {
    result->add_value(list->get_value(i));
  }
  return result;
}

void throw_on_bad_arity(std::string &name, size_t expected, size_t actual) {
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


//std::vector special_forms = {
//        "let",
//        "let*",
//        "define",
//        "set!",
//};
