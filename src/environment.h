#ifndef SCHEME_ENVIRONMENT_H
#define SCHEME_ENVIRONMENT_H

#include "printer.h"
#include "reader.h"
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <iostream>

void throw_on_bad_arity(std::string & name, size_t expected, size_t actual) {
  if (expected != actual) {
    throw std::runtime_error("Expected " + std::to_string(expected) + " arguments, got " + std::to_string(actual));
  }
}

class Environment {
public:
Environment() : outer(nullptr) {
}

Environment(std::shared_ptr<Environment> outer) : outer(outer) {
}

std::shared_ptr<Value> get(const std::string &key) const {
  if (data.find(key) != data.end()) {
    return data.at(key);
  }
  if (outer != nullptr) {
    return outer->get(key);
  }
  throw std::runtime_error("Symbol " + key +" not found");
}

void set(const std::string &key, const std::shared_ptr<Value> &value) {
  data[key] = value;
}
//    find: takes a symbol key and if the current environment contains that key then return the environment.
//    If no key is found and outer is not nil then call find (recurse) on the outer environment.
std::shared_ptr<Environment> find(const std::string &key) {
  if (data.find(key) != data.end()) {
    return std::make_shared<Environment>(*this);
  }
  if (outer != nullptr) {
    return outer->find(key);
  }
  throw std::runtime_error("Symbol " + key +" not found");

}

std::shared_ptr<Environment> get_outer() const {
  return outer;
}

protected:
std::map<std::string, std::shared_ptr<Value> > data;
std::shared_ptr<Environment> outer;


};

class BaseEnvironment : public Environment {

public:
BaseEnvironment() : Environment(nullptr)  {     // todo: move to impl file
  auto plus_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > & argv) {
				 throw_on_bad_arity((std::string &) "+", 2, argc);// FIXME: this should be elsewhere
				 auto a = std::static_pointer_cast<IntegerValue>(argv[0]); // TODO: consider not casting here it seems wrong
				 auto b = std::static_pointer_cast<IntegerValue>(argv[1]);
				 return std::static_pointer_cast<Value>(std::make_shared<IntegerValue>(a->get_value() + b->get_value()));
			       };
  set("+", std::make_shared<FunctionValue>(plus_function_pointer));

  auto minus_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > & argv) {
				  throw_on_bad_arity((std::string &) "-", 2, argc); // FIXME: this should be elsewhere
				  auto a = std::static_pointer_cast<IntegerValue>(argv[0]);
				  auto b = std::static_pointer_cast<IntegerValue>(argv[1]);
				  return std::static_pointer_cast<Value>(std::make_shared<IntegerValue>(a->get_value() - b->get_value()));
				};
  set("-", std::make_shared<FunctionValue>(minus_function_pointer));

  auto multiply_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > & argv) {
				     throw_on_bad_arity((std::string &) "*", 2, argc); // FIXME: this should be elsewhere
				     auto a = std::static_pointer_cast<IntegerValue>(argv[0]);
				     auto b = std::static_pointer_cast<IntegerValue>(argv[1]);
				     return std::static_pointer_cast<Value>(std::make_shared<IntegerValue>(a->get_value() * b->get_value()));
				   };

  set("*", std::make_shared<FunctionValue>(multiply_function_pointer));

  auto divide_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > & argv) {
				   throw_on_bad_arity((std::string &) "/", 2, argc); // FIXME: this should be elsewhere
				   auto a = std::static_pointer_cast<IntegerValue>(argv[0]);
				   auto b = std::static_pointer_cast<IntegerValue>(argv[1]);
				   return std::static_pointer_cast<Value>(std::make_shared<IntegerValue>(a->get_value() / b->get_value()));
				 };

  set("/", std::make_shared<FunctionValue>(divide_function_pointer));

  // TODO: add more functions
  // TODO: solve different types of args




}
};

#endif //SCHEME_ENVIRONMENT_H
