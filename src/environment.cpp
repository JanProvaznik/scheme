#include "util.h"
#include "environment.h"

#include <utility>

#include <utility>


void Environment::set(const std::string &key, const std::shared_ptr<Value> &value) {
  data[key] = value;
}

void Environment::update_existing(const std::string &key, const std::shared_ptr<Value> &value) {
  if (data.find(key) != data.end()) {
    data[key] = value;
  } else if (outer != nullptr) {
    outer->update_existing(key, value);
  } else {
    throw std::runtime_error("Symbol " + key + " not found");
  }
}

Environment::Environment(EnvironmentPtr outer, const std::shared_ptr<ListValue>& binds,
                         const std::shared_ptr<ListValue>& exprs)
  : outer(std::move(std::move(outer))) {
  for (size_t i = 0; i < binds->size(); i++) {
    auto bind = binds->get_value(i);
    auto expr = exprs->get_value(i);
    set(bind->to_string(), expr);
  }
}

ValuePtr Environment::get(const std::string &key) const {
  if (data.find(key) != data.end()) {
    return data.at(key);
  }
  if (outer != nullptr) {
    return outer->get(key);
  }
  throw std::runtime_error("Symbol " + key + " not found");
}

//    find: takes a symbol key and if the current environment contains that key then return the environment.
//    If no key is found and outer is not nil then call find (recurse) on the outer environment.
EnvironmentPtr Environment::find(const std::string &key) {
  if (data.find(key) != data.end()) {
    return std::make_shared<Environment>(*this);
  }
  if (outer != nullptr) {
    return outer->find(key);
  }
  throw std::runtime_error("Symbol " + key + " not found");

}

EnvironmentPtr Environment::get_outer() const {
  return outer;
}

BaseEnvironment::BaseEnvironment() : Environment(nullptr) {
  auto plus_function_pointer = [](size_t argc, std::vector<ValuePtr > &argv) {
				 throw_on_bad_arity((std::string &) "+", 2, argc);// FIXME: this should be elsewhere
				 // if any is float then return float else return int
				 if (argv[0]->get_type() == ValueType::Float || argv[1]->get_type() == ValueType::Float) {
				   return std::static_pointer_cast<Value>(
				     std::make_shared<FloatValue>(argv[0]->to_double() + argv[1]->to_double()));
				 } else {
				   auto a = std::static_pointer_cast<IntegerValue>(argv[0]);
				   auto b = std::static_pointer_cast<IntegerValue>(argv[1]);
				   return std::static_pointer_cast<Value>(std::make_shared<IntegerValue>(a->get_value() + b->get_value()));
				 }

			       };

  set("+", std::make_shared<FunctionValue>(plus_function_pointer));

  auto minus_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
				  throw_on_bad_arity((std::string &) "-", 2, argc);// FIXME: this should be elsewhere
				  // if any is float then return float else return int
				  if (argv[0]->get_type() == ValueType::Float || argv[1]->get_type() == ValueType::Float) {
				    return std::static_pointer_cast<Value>(
				      std::make_shared<FloatValue>(argv[0]->to_double() - argv[1]->to_double()));
				  } else {
				    auto a = std::static_pointer_cast<IntegerValue>(argv[0]);
				    auto b = std::static_pointer_cast<IntegerValue>(argv[1]);
				    return std::static_pointer_cast<Value>(std::make_shared<IntegerValue>(a->get_value() - b->get_value()));
				  }

				};

  set("-", std::make_shared<FunctionValue>(minus_function_pointer));

  auto multiply_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
				     throw_on_bad_arity((std::string &) "*", 2, argc);// FIXME: this should be elsewhere
				     // if any is float then return float else return int
				     if (argv[0]->get_type() == ValueType::Float || argv[1]->get_type() == ValueType::Float) {
				       return std::static_pointer_cast<Value>(
					 std::make_shared<FloatValue>(argv[0]->to_double() * argv[1]->to_double()));
				     } else {
				       auto a = std::static_pointer_cast<IntegerValue>(argv[0]);
				       auto b = std::static_pointer_cast<IntegerValue>(argv[1]);
				       return std::static_pointer_cast<Value>(std::make_shared<IntegerValue>(a->get_value() * b->get_value()));
				     }

				   };

  set("*", std::make_shared<FunctionValue>(multiply_function_pointer));

  auto divide_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
				   throw_on_bad_arity((std::string &) "/", 2, argc);// FIXME: this should be elsewhere
				   // if any is float then return float else return int
				   if (argv[0]->get_type() == ValueType::Float || argv[1]->get_type() == ValueType::Float) {
				     return std::static_pointer_cast<Value>(
				       std::make_shared<FloatValue>(argv[0]->to_double() / argv[1]->to_double()));
				   } else {
				     auto a = std::static_pointer_cast<IntegerValue>(argv[0]);
				     auto b = std::static_pointer_cast<IntegerValue>(argv[1]);
				     return std::static_pointer_cast<Value>(std::make_shared<IntegerValue>(a->get_value() / b->get_value()));
				   }

				 };

  set("/", std::make_shared<FunctionValue>(divide_function_pointer));
  set("true", std::make_shared<BoolValue>(true));
  set("false", std::make_shared<BoolValue>(false));
  set("nil", std::make_shared<NilValue>());
  set("else", std::make_shared<BoolValue>(true));



  // TODO: add more functions
  // TODO: solve different types of args




}
