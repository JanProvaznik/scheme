#include "util.h"
#include "main.h"
#include "datatypes/Closure.h"
#include "datatypes/types.h"
#include "environment.h"

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
  : Value(ValueType::Environment), outer(std::move(std::move(outer))){
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

BaseEnvironment::BaseEnvironment() : Environment( nullptr)  {
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


//list: take the parameters and return them as a list.
  auto list_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
                 return std::static_pointer_cast<Value>(std::make_shared<ListValue>(argv));
                   };
  set("list", std::make_shared<FunctionValue>(list_function_pointer));
//list?: return true if the first parameter is a list, false otherwise.
auto list_queston_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
                 return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(argv[0]->get_type() == ValueType::List));
                   };
  set("list?", std::make_shared<FunctionValue>(list_queston_function_pointer));
//empty?: treat the first parameter as a list and return true if the list is empty and false if it contains any elements.
auto empty_queston_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
                if (argv[0]->get_type() != ValueType::List) {
                    return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(false));
                }
                auto list = std::static_pointer_cast<ListValue>(argv[0]);
                return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(list->size() == 0));


                   };
  set("empty?", std::make_shared<FunctionValue>(empty_queston_function_pointer));
//count: treat the first parameter as a list and return the number of elements that it contains.
auto count_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
                auto list = std::static_pointer_cast<ListValue>(argv[0]);
                return std::static_pointer_cast<Value>(std::make_shared<IntegerValue>(list->size()));
                   };
  set("count", std::make_shared<FunctionValue>(count_function_pointer));
//=: compare the first two parameters and return true if they are the same type and contain the same value. In the case of equal length lists, each element of the list should be compared for equality and if they are the same return true, otherwise false.
auto eq_function = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
    //TODO actually implement this
                if (argv[0]->get_type() != argv[1]->get_type()) {
                    return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(false));
                }
                if (argv[0]->get_type() == ValueType::Integer) {
                    auto a = std::static_pointer_cast<IntegerValue>(argv[0]);
                    auto b = std::static_pointer_cast<IntegerValue>(argv[1]);
                    return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(a->get_value() == b->get_value()));
                }



                if (argv[0]->get_type() == ValueType::List) {
                    auto list1 = std::static_pointer_cast<ListValue>(argv[0]);
                    auto list2 = std::static_pointer_cast<ListValue>(argv[1]);
                    if (list1->size() != list2->size()) {
                        return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(false));
                    }
                    for (int i = 0; i < list1->size(); i++) {
                        if (list1->get_value(i) != list2->get_value(i)) {
                            return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(false));
                        }
                    }
                    return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(true));
                }
                return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(argv[0] == argv[1]));
                   };
  set("=", std::make_shared<FunctionValue>(eq_function));
//<, <=, >, and >=: treat the first two parameters as numbers and do the corresponding numeric comparison, returning either true or false.

// boolean?
auto boolean_queston_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
                 return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(argv[0]->get_type() == ValueType::Bool));
                   };
    set("boolean?", std::make_shared<FunctionValue>(boolean_queston_function_pointer));

    // newline
    auto newline_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
        std::cout << std::endl;
        return std::static_pointer_cast<Value>(std::make_shared<NilValue>());
    };
    set("newline", std::make_shared<FunctionValue>(newline_function_pointer));
    // display
    auto display_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
        std::cout << argv[0]->to_string();
        return std::static_pointer_cast<Value>(std::make_shared<NilValue>());
    };
    set("display", std::make_shared<FunctionValue>(display_function_pointer));
   // quote
//    auto quote_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
//         return argv[0];
//    };
//    set("quote", std::make_shared<FunctionValue>(quote_function_pointer));

// cons
auto cons_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
    auto old_list = std::static_pointer_cast<ListValue>(argv[1]);
    auto list = std::make_shared<ListValue>();
    list->add_value(argv[0]);
    for (int i = 0; i < old_list->size(); i++) {
        list->add_value(old_list->get_value(i));
    }
    return std::static_pointer_cast<Value>(list);
};
set("cons", std::make_shared<FunctionValue>(cons_function_pointer));

// concat
auto concat_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
    auto list1 = std::static_pointer_cast<ListValue>(argv[0]);
    auto list2 = std::static_pointer_cast<ListValue>(argv[1]);
    auto list = std::make_shared<ListValue>();
    for (int i = 0; i < list1->size(); i++) {
        list->add_value(list1->get_value(i));
    }
    for (int i = 0; i < list2->size(); i++) {
        list->add_value(list2->get_value(i));
    }
    return std::static_pointer_cast<Value>(list);
};
set("concat", std::make_shared<FunctionValue>(concat_function_pointer));

// zero?
auto zero_queston_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
    if (argv[0]->get_type() != ValueType::Integer) {
        throw std::runtime_error("zero? expects an integer");
    }
    auto integer = std::static_pointer_cast<IntegerValue>(argv[0]);
    return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(integer->get_value() == 0));
};
set("zero?", std::make_shared<FunctionValue>(zero_queston_function_pointer));

//    Not returns #t if obj is false, and returns #f otherwise
auto not_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
    if (argv[0]->get_type() != ValueType::Bool) {
        return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(false));
    }// return ! is true
    return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(!argv[0]->is_true()));
};
set("not", std::make_shared<FunctionValue>(not_function_pointer));

// pair? (or more like list?)
auto pair_queston_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
    return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(argv[0]->get_type() == ValueType::List));
};
set("pair?", std::make_shared<FunctionValue>(pair_queston_function_pointer));

// car
auto car_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
return car<Value> (argv[0]);
};
set("car", std::make_shared<FunctionValue>(car_function_pointer));


// cdr
auto cdr_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
return std::static_pointer_cast<Value>(cdr(argv[0]));
};
set("cdr", std::make_shared<FunctionValue>(cdr_function_pointer));

// set-car!
auto set_car_bang_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
    auto list = std::static_pointer_cast<ListValue>(argv[0]);
    list->set_value(0, argv[1]);
    return std::static_pointer_cast<Value>(std::make_shared<NilValue>());
};
set("set-car!", std::make_shared<FunctionValue>(set_car_bang_function_pointer));

// set-cdr!
auto set_cdr_bang_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
    auto list = std::static_pointer_cast<ListValue>(argv[0]);
     while (list->size() > 1) {
        list->pop_back();
     }
     auto cdrlist = std::static_pointer_cast<ListValue>(argv[1]);
        for (int i = 0; i < cdrlist->size(); i++) {
            list->add_value(cdrlist->get_value(i));
        }
    return std::static_pointer_cast<Value>(std::make_shared<NilValue>());
};
set("set-cdr!", std::make_shared<FunctionValue>(set_cdr_bang_function_pointer));

// null?: Returns #t if obj is the empty list, otherwise returns #f.

auto null_queston_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
    return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(argv[0]->get_type() == ValueType::List && argv[0]->to_string() == "()"));
};

set("null?", std::make_shared<FunctionValue>(null_queston_function_pointer));

// length
auto length_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
    auto list = std::static_pointer_cast<ListValue>(argv[0]);
    return std::static_pointer_cast<Value>(std::make_shared<IntegerValue>(list->size()));
};

set("length", std::make_shared<FunctionValue>(length_function_pointer));

// append
set("append", std::make_shared<FunctionValue>(concat_function_pointer));

// memq
//memv
//member

//assq
//assv
//assoc

//string->symbol

//symbol?

//symbol->string

//string?

//string-length

//string-ref

//string=? string<?
// schemeable: string>? string<=? string>=?

// vectors are lists, easy
set("vector?", std::make_shared<FunctionValue>(list_queston_function_pointer));
//make-vector (size), makes a vector with nils
auto make_vector_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
    auto size = std::static_pointer_cast<IntegerValue>(argv[0]);
    auto list = std::make_shared<ListValue>();
    for (int i = 0; i < size->get_value(); i++) {
        list->add_value(std::make_shared<NilValue>());
    }
    return std::static_pointer_cast<Value>(list);
};
// vector obj...
set("make-vector", std::make_shared<FunctionValue>(list_function_pointer));
// vector-length
set("vector-length", std::make_shared<FunctionValue>(length_function_pointer));

// vector-ref
auto vector_ref_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
    auto list = std::static_pointer_cast<ListValue>(argv[0]);
    auto index = std::static_pointer_cast<IntegerValue>(argv[1]);
    return list->get_value(index->get_value());
};

set("vector-ref", std::make_shared<FunctionValue>(vector_ref_function_pointer));

// vector-set!
auto vector_set_bang_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
    auto list = std::static_pointer_cast<ListValue>(argv[0]);
    auto index = std::static_pointer_cast<IntegerValue>(argv[1]);
    list->set_value(index->get_value(), argv[2]);
    return std::static_pointer_cast<Value>(std::make_shared<NilValue>());
};
set("vector-set!", std::make_shared<FunctionValue>(vector_set_bang_function_pointer));
// vector->list
// list->vector just identity
auto vector_to_list_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
    return argv[0];
};
set("vector->list", std::make_shared<FunctionValue>(vector_to_list_function_pointer));
set("list->vector", std::make_shared<FunctionValue>(vector_to_list_function_pointer));

// procedure?
auto procedure_queston_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
    return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(argv[0]->get_type() == ValueType::Function || argv[0]->get_type() == ValueType::Closure));
};
set("procedure?", std::make_shared<FunctionValue>(procedure_queston_function_pointer));

// map
auto map_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
    auto list = std::static_pointer_cast<ListValue>(argv[0]);
    // it's a function or a closure
    if (argv[1]->get_type() == ValueType::Function) {
        auto func = std::static_pointer_cast<FunctionValue>(argv[1]);
        auto newlist = std::make_shared<ListValue>();
        for (int i = 0; i < list->size(); i++) {
            std::vector<std::shared_ptr<Value> > args;
            args.push_back(list->get_value(i));
            newlist->add_value(apply(func, args));
        }
        return std::static_pointer_cast<Value>(newlist);
    }
    auto closure = std::static_pointer_cast<Closure>(argv[1]);
    auto newlist = std::make_shared<ListValue>();
    for (int i = 0; i < list->size(); i++) {
        std::vector<std::shared_ptr<Value> > args;
        args.push_back(list->get_value(i));
        newlist->add_value(closure->call(args));
    }
    return std::static_pointer_cast<Value>(newlist);
};
set("map", std::make_shared<FunctionValue>(map_function_pointer));


// for-each
auto for_each_function_pointer = [](size_t argc, std::vector<std::shared_ptr<Value> > &argv) {
    auto list = std::static_pointer_cast<ListValue>(argv[0]);
    // it's a function or a closure
    if (argv[1]->get_type() == ValueType::Function) {
        auto func = std::static_pointer_cast<FunctionValue>(argv[1]);
        for (int i = 0; i < list->size(); i++) {
            std::vector<std::shared_ptr<Value> > args;
            args.push_back(list->get_value(i));
            apply(func, args);
        }
        return std::static_pointer_cast<Value>(std::make_shared<NilValue>());
    }
    auto closure = std::static_pointer_cast<Closure>(argv[1]);
    for (int i = 0; i < list->size(); i++) {
        std::vector<std::shared_ptr<Value> > args;
        args.push_back(list->get_value(i));
        closure->call(args);
    }
    return std::static_pointer_cast<Value>(std::make_shared<NilValue>());
};
set("for-each", std::make_shared<FunctionValue>(for_each_function_pointer));

// call-with-current-continuation ??? wtf is this


// 6.10
// call-with-input-file
// call-with-output-file
// input-port?
// output-port?
// current-input-port
// current-output-port
// read
// read port
// char-ready?
// char-ready? port
// eof-object?

// write obj
// write obj port
// display obj
// display obj port
// newline port

// load filename




}

// TODO eq? eqv? equal?

// TODO = <

// abs
// quotient
// remainder

// can be done in scheme itself: max min, <=, >=, >

// characters????
//char?
// char=? char<?
// schemeable: char<=? char>=?, char>?
// char->integer
// integer->char
// read-char
// read-char port
//write-char char
//write-char char port




