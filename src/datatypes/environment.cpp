#include "../evaluator.h"
#include "closure.h"
#include "types.h"
#include "environment.h"

#include <utility>


void Environment::set(const std::string &key, const ValuePtr &value) {
    data[key] = value;
}

void Environment::update_existing(const std::string &key, const ValuePtr &value) {
    if (data.find(key) != data.end()) {
        data[key] = value;
    } else if (outer != nullptr) {
        outer->update_existing(key, value);
    } else {
        throw std::runtime_error("Symbol " + key + " not found");
    }
}

Environment::Environment(EnvironmentPtr outer, const std::shared_ptr<ListValue> &binds,
                         const std::shared_ptr<ListValue> &exprs)
        : Value(ValueType::Environment), outer(std::move(std::move(outer))) {
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
    auto make_arithmetic_function = [](auto operation) {
        return [operation](size_t argc, std::vector<ValuePtr> &argv) {
            if (!check_signature(argv, {ValueType::Integer, ValueType::Integer}) && !check_signature(argv, {ValueType::Float, ValueType::Float}) && !check_signature(argv, {ValueType::Integer, ValueType::Float}) && !check_signature(argv, {ValueType::Float, ValueType::Integer}))
            {
                throw std::runtime_error("Invalid argument types, this is only a numeric operator.");
            }

            if (argv[0]->get_type() == ValueType::Float || argv[1]->get_type() == ValueType::Float) {
                double result = operation(argv[0]->to_double(), argv[1]->to_double());
                return std::static_pointer_cast<Value>(std::make_shared<FloatValue>(result));
            } else {
                int result = operation(std::static_pointer_cast<IntegerValue>(argv[0])->get_value(),
                                       std::static_pointer_cast<IntegerValue>(argv[1])->get_value());
                return std::static_pointer_cast<Value>(std::make_shared<IntegerValue>(result));
            }
        };
    };

    set("+", std::make_shared<FunctionValue>(make_arithmetic_function(std::plus<>{})));
    set("-", std::make_shared<FunctionValue>(make_arithmetic_function(std::minus<>{})));
    set("*", std::make_shared<FunctionValue>(make_arithmetic_function(std::multiplies<>{})));
    set("/", std::make_shared<FunctionValue>(make_arithmetic_function(std::divides<>{})));


    set("true", std::make_shared<BoolValue>(true));
    set("false", std::make_shared<BoolValue>(false));
    set("nil", std::make_shared<NilValue>());
    set("else", std::make_shared<BoolValue>(true));

    auto write_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        for (auto &arg : argv) {
            std::cout << arg->to_string();
        }
        return std::static_pointer_cast<Value>(std::make_shared<NilValue>());
    };
    set("write", std::make_shared<FunctionValue>(write_function_pointer));

//list: take the parameters and return them as a list.
    auto list_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        return std::static_pointer_cast<Value>(std::make_shared<ListValue>(argv));
    };
    set("list", std::make_shared<FunctionValue>(list_function_pointer));
//list?: return true if the first parameter is a list, false otherwise.
    auto list_question_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(argv[0]->get_type() == ValueType::List));
    };
    set("list?", std::make_shared<FunctionValue>(list_question_function_pointer));
//empty?: treat the first parameter as a list and return true if the list is empty and false if it contains any elements.
    auto empty_queston_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        if (argv[0]->get_type() != ValueType::List) {
            return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(false));
        }
        auto list = std::static_pointer_cast<ListValue>(argv[0]);
        return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(list->size() == 0));


    };
    set("empty?", std::make_shared<FunctionValue>(empty_queston_function_pointer));
//count: treat the first parameter as a list and return the number of elements that it contains.
    auto count_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {

        auto list = std::static_pointer_cast<ListValue>(argv[0]);
        return std::static_pointer_cast<Value>(std::make_shared<IntegerValue>(list->size()));
    };
    set("count", std::make_shared<FunctionValue>(count_function_pointer));

    auto make_comparison_function = [](auto comparator) {
        return [comparator](size_t argc, std::vector<ValuePtr> &argv) {
            auto n1 = argv[0]->to_double();
            auto n2 = argv[1]->to_double();
            return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(comparator(n1, n2)));
        };
    };

    set("=", std::make_shared<FunctionValue>(make_comparison_function(std::equal_to<double>())));
    set(">", std::make_shared<FunctionValue>(make_comparison_function(std::greater<double>())));
    set("<", std::make_shared<FunctionValue>(make_comparison_function(std::less<double>())));
    set(">=", std::make_shared<FunctionValue>(make_comparison_function(std::greater_equal<double>())));
    set("<=", std::make_shared<FunctionValue>(make_comparison_function(std::less_equal<double>())));

// abs
    auto abs_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        auto n = argv[0]->to_double();
        return std::static_pointer_cast<Value>(std::make_shared<FloatValue>(std::abs(n)));
    };
    set("abs", std::make_shared<FunctionValue>(abs_function_pointer));

// quotient
    auto quotient_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        auto a = std::static_pointer_cast<IntegerValue>(argv[0]);
        auto b = std::static_pointer_cast<IntegerValue>(argv[1]);
        return std::static_pointer_cast<Value>(std::make_shared<IntegerValue>(a->get_value() / b->get_value()));
    };
    set("quotient", std::make_shared<FunctionValue>(quotient_function_pointer));

// remainder
    auto remainder_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        auto a = std::static_pointer_cast<IntegerValue>(argv[0]);
        auto b = std::static_pointer_cast<IntegerValue>(argv[1]);
        return std::static_pointer_cast<Value>(std::make_shared<IntegerValue>(a->get_value() % b->get_value()));
    };
    set("remainder", std::make_shared<FunctionValue>(remainder_function_pointer));

// max both for integer and float
    auto max_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        if (argv[0]->get_type() == ValueType::Integer && argv[1]->get_type() == ValueType::Integer) {
            auto a = std::static_pointer_cast<IntegerValue>(argv[0])->get_value();
            auto b = std::static_pointer_cast<IntegerValue>(argv[1])->get_value();
            return std::static_pointer_cast<Value>(std::make_shared<IntegerValue>(std::max(a, b)));
        }
        auto a = argv[0]->to_double();
        auto b = argv[1]->to_double();
        return std::static_pointer_cast<Value>(std::make_shared<FloatValue>(std::max(a, b)));
    };
    set("max", std::make_shared<FunctionValue>(max_function_pointer));
// min

    auto min_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        if (argv[0]->get_type() == ValueType::Integer && argv[1]->get_type() == ValueType::Integer) {
            auto a = std::static_pointer_cast<IntegerValue>(argv[0])->get_value();
            auto b = std::static_pointer_cast<IntegerValue>(argv[1])->get_value();
            return std::static_pointer_cast<Value>(std::make_shared<IntegerValue>(std::min(a, b)));
        }
        auto a = argv[0]->to_double();
        auto b = argv[1]->to_double();
        return std::static_pointer_cast<Value>(std::make_shared<FloatValue>(std::min(a, b)));
    };
    set("min", std::make_shared<FunctionValue>(min_function_pointer));

// number?

    auto number_question_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(
                argv[0]->get_type() == ValueType::Integer || argv[0]->get_type() == ValueType::Float));
    };
    set("number?", std::make_shared<FunctionValue>(number_question_function_pointer));
// real?

    auto real_question_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(
                argv[0]->get_type() == ValueType::Integer || argv[0]->get_type() == ValueType::Float));
    };
    set("real?", std::make_shared<FunctionValue>(real_question_function_pointer));
// integer?
    auto integer_question_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(argv[0]->get_type() == ValueType::Integer));
    };
    set("integer?", std::make_shared<FunctionValue>(integer_question_function_pointer));
    set("rational?", std::make_shared<FunctionValue>(integer_question_function_pointer));


// boolean?
    auto boolean_question_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(argv[0]->get_type() == ValueType::Bool));
    };
    set("boolean?", std::make_shared<FunctionValue>(boolean_question_function_pointer));

    // newline
    auto newline_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        std::cout << std::endl;
        return std::static_pointer_cast<Value>(std::make_shared<NilValue>());
    };
    set("newline", std::make_shared<FunctionValue>(newline_function_pointer));
    // display
    auto display_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        std::cout << argv[0]->to_string();
        return std::static_pointer_cast<Value>(std::make_shared<NilValue>());
    };
    set("display", std::make_shared<FunctionValue>(display_function_pointer));

// cons
    auto cons_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
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
    auto concat_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
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


    auto number_comparison_function = [](const auto &comparison, size_t argc, std::vector<ValuePtr> &argv) {
        auto n = argv[0]->to_double();
        return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(comparison(n)));
    };

    auto zero_comparison = [](double n) {
        return n == 0;
    };
    set("zero?", std::make_shared<FunctionValue>([number_comparison_function, zero_comparison](auto &&PH1, auto &&PH2) {
        return number_comparison_function(zero_comparison, std::forward<decltype(PH1)>(PH1),
                                          std::forward<decltype(PH2)>(PH2));
    }));

    auto positive_comparison = [](double n) {
        return n > 0;
    };
    set("positive?",
        std::make_shared<FunctionValue>([number_comparison_function, positive_comparison](auto &&PH1, auto &&PH2) {
            return number_comparison_function(positive_comparison, std::forward<decltype(PH1)>(PH1),
                                              std::forward<decltype(PH2)>(PH2));
        }));

    auto negative_comparison = [](double n) {
        return n < 0;
    };
    set("negative?",
        std::make_shared<FunctionValue>([number_comparison_function, negative_comparison](auto &&PH1, auto &&PH2) {
            return number_comparison_function(negative_comparison, std::forward<decltype(PH1)>(PH1),
                                              std::forward<decltype(PH2)>(PH2));
        }));

    auto odd_question_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        if (argv[0]->get_type() != ValueType::Integer) {
            return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(false));
        }
        auto n = std::static_pointer_cast<IntegerValue>(argv[0])->get_value();
        return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(n % 2 == 1));
    };
    set("odd?", std::make_shared<FunctionValue>(odd_question_function_pointer));
// even
    auto even_question_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        if (argv[0]->get_type() != ValueType::Integer) {
            return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(false));
        }
        auto n = std::static_pointer_cast<IntegerValue>(argv[0])->get_value();
        return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(n % 2 == 0));
    };
    set("even?", std::make_shared<FunctionValue>(even_question_function_pointer));

// exact
    auto exact_question_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(argv[0]->get_type() == ValueType::Integer));
    };
    set("exact?", std::make_shared<FunctionValue>(exact_question_function_pointer));

// inexact
    auto inexact_question_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(argv[0]->get_type() == ValueType::Float));
    };
    set("inexact?",
        std::static_pointer_cast<Value>(std::make_shared<FunctionValue>(inexact_question_function_pointer)));




//    Not returns #t if obj is false, and returns #f otherwise
    auto not_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        if (argv[0]->get_type() != ValueType::Bool) {
            return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(false));
        }// return ! is true
        return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(!argv[0]->is_true()));
    };
    set("not", std::make_shared<FunctionValue>(not_function_pointer));

// pair? (or more like list?)
    auto pair_queston_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(argv[0]->get_type() == ValueType::List));
    };
    set("pair?", std::make_shared<FunctionValue>(pair_queston_function_pointer));

// car
    auto car_function_pointer = [](size_t argc, const std::vector<ValuePtr> &argv) {
        return car<Value>(argv[0]);
    };
    set("car", std::make_shared<FunctionValue>(car_function_pointer));


// cdr
    auto cdr_function_pointer = [](size_t argc, const std::vector<ValuePtr> &argv) {
        return std::static_pointer_cast<Value>(cdr(argv[0]));
    };
    set("cdr", std::make_shared<FunctionValue>(cdr_function_pointer));

// set-car!
    auto set_car_bang_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        auto list = std::static_pointer_cast<ListValue>(argv[0]);
        list->set_value(0, argv[1]);
        return std::static_pointer_cast<Value>(std::make_shared<NilValue>());
    };
    set("set-car!", std::make_shared<FunctionValue>(set_car_bang_function_pointer));

// set-cdr!
    auto set_cdr_bang_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
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

    auto null_question_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        return std::static_pointer_cast<Value>(
                std::make_shared<BoolValue>(argv[0]->get_type() == ValueType::List && argv[0]->to_string() == "()"));
    };

    set("null?", std::make_shared<FunctionValue>(null_question_function_pointer));

// length
    set("length", std::make_shared<FunctionValue>(count_function_pointer));

// append
    set("append", std::make_shared<FunctionValue>(concat_function_pointer));


//string->symbol
    auto string_to_symbol_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        auto string = std::static_pointer_cast<StringValue>(argv[0]);
        return std::static_pointer_cast<Value>(std::make_shared<SymbolValue>(string->to_string()));
    };
    set("string->symbol", std::make_shared<FunctionValue>(string_to_symbol_function_pointer));


//symbol?
    auto symbol_question_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(argv[0]->get_type() == ValueType::Symbol));
    };
    set("symbol?", std::make_shared<FunctionValue>(symbol_question_function_pointer));

//symbol->string

    auto symbol_to_string_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        auto symbol = std::static_pointer_cast<SymbolValue>(argv[0]);
        return std::static_pointer_cast<Value>(std::make_shared<StringValue>(symbol->to_string()));
    };
    set("symbol->string", std::make_shared<FunctionValue>(symbol_to_string_function_pointer));

//string?
    auto string_question_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(argv[0]->get_type() == ValueType::String));
    };
    set("string?", std::make_shared<FunctionValue>(string_question_function_pointer));



//string-length

    auto string_length_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        auto string = std::static_pointer_cast<StringValue>(argv[0]);
        return std::static_pointer_cast<Value>(std::make_shared<IntegerValue>(string->to_string().length()));
    };
    set("string-length", std::make_shared<FunctionValue>(string_length_function_pointer));

//string-ref

    auto string_ref_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        auto string = std::static_pointer_cast<StringValue>(argv[0]);
        auto index = std::static_pointer_cast<IntegerValue>(argv[1]);
        return std::static_pointer_cast<Value>(
                std::make_shared<StringValue>(string->to_string().substr(index->get_value(), 1)));
    };
    set("string-ref", std::make_shared<FunctionValue>(string_ref_function_pointer));

    auto string_equal_question_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        auto string1 = std::static_pointer_cast<StringValue>(argv[0]);
        auto string2 = std::static_pointer_cast<StringValue>(argv[1]);
        return std::static_pointer_cast<Value>(
                std::make_shared<BoolValue>(string1->to_string() == string2->to_string()));
    };
    set("string=?", std::make_shared<FunctionValue>(string_equal_question_function_pointer));

    auto string_less_than_question_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        auto string1 = std::static_pointer_cast<StringValue>(argv[0]);
        auto string2 = std::static_pointer_cast<StringValue>(argv[1]);
        return std::static_pointer_cast<Value>(
                std::make_shared<BoolValue>(string1->to_string() < string2->to_string()));
    };
    set("string<?", std::make_shared<FunctionValue>(string_less_than_question_function_pointer));

// string>?
    auto string_greater_than_question_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        auto string1 = std::static_pointer_cast<StringValue>(argv[0]);
        auto string2 = std::static_pointer_cast<StringValue>(argv[1]);
        return std::static_pointer_cast<Value>(
                std::make_shared<BoolValue>(string1->to_string() > string2->to_string()));
    };
    set("string>?", std::make_shared<FunctionValue>(string_greater_than_question_function_pointer));

// string<=?
    auto string_less_than_or_equal_question_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        auto string1 = std::static_pointer_cast<StringValue>(argv[0]);
        auto string2 = std::static_pointer_cast<StringValue>(argv[1]);
        return std::static_pointer_cast<Value>(
                std::make_shared<BoolValue>(string1->to_string() <= string2->to_string()));
    };

    set("string<=?", std::make_shared<FunctionValue>(string_less_than_or_equal_question_function_pointer));

// string>=?
    auto string_greater_than_or_equal_question_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        auto string1 = std::static_pointer_cast<StringValue>(argv[0]);
        auto string2 = std::static_pointer_cast<StringValue>(argv[1]);
        return std::static_pointer_cast<Value>(
                std::make_shared<BoolValue>(string1->to_string() >= string2->to_string()));
    };
    set("string>=?", std::make_shared<FunctionValue>(string_greater_than_or_equal_question_function_pointer));

// vectors are lists, easy
    set("vector?", std::make_shared<FunctionValue>(list_question_function_pointer));

    //make-vector (size), makes a vector with nils
    auto make_vector_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        auto size = std::static_pointer_cast<IntegerValue>(argv[0]);
        auto list = std::make_shared<ListValue>();
        for (int i = 0; i < size->get_value(); i++) {
            list->add_value(std::make_shared<NilValue>());
        }
        return std::static_pointer_cast<Value>(list);
    };
// vector obj...
    set("make-vector", std::make_shared<FunctionValue>(make_vector_function_pointer));
// vector-length
    set("vector-length", std::make_shared<FunctionValue>(count_function_pointer));

// vector-ref
    auto vector_ref_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        auto list = std::static_pointer_cast<ListValue>(argv[0]);
        auto index = std::static_pointer_cast<IntegerValue>(argv[1]);
        return list->get_value(index->get_value());
    };

    set("vector-ref", std::make_shared<FunctionValue>(vector_ref_function_pointer));

// vector-set!
    auto vector_set_bang_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        auto list = std::static_pointer_cast<ListValue>(argv[0]);
        auto index = std::static_pointer_cast<IntegerValue>(argv[1]);
        list->set_value(index->get_value(), argv[2]);
        return std::static_pointer_cast<Value>(std::make_shared<NilValue>());
    };
    set("vector-set!", std::make_shared<FunctionValue>(vector_set_bang_function_pointer));
// vector->list
// list->vector just identity
    auto vector_to_list_function_pointer = [](size_t argc, const std::vector<ValuePtr> &argv) {
        return argv[0];
    };
    set("vector->list", std::make_shared<FunctionValue>(vector_to_list_function_pointer));
    set("list->vector", std::make_shared<FunctionValue>(vector_to_list_function_pointer));

// procedure?
    auto procedure_queston_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(
                argv[0]->get_type() == ValueType::Function || argv[0]->get_type() == ValueType::Closure));
    };
    set("procedure?", std::make_shared<FunctionValue>(procedure_queston_function_pointer));

// map
    auto map_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        auto list = std::static_pointer_cast<ListValue>(argv[1]);
        // it's a function or a Closure
        if (argv[1]->get_type() == ValueType::Function) {
            auto func = std::static_pointer_cast<FunctionValue>(argv[0]);
            auto newlist = std::make_shared<ListValue>();
            for (int i = 0; i < list->size(); i++) {
                std::vector<ValuePtr> args;
                args.push_back(list->get_value(i));
                newlist->add_value(apply_fn(func, args));
            }
            return std::static_pointer_cast<Value>(newlist);
        }
        auto closure = std::static_pointer_cast<Closure>(argv[0]);
        auto newlist = std::make_shared<ListValue>();
        for (int i = 0; i < list->size(); i++) {
            std::vector<ValuePtr> args;
            args.push_back(list->get_value(i));
            newlist->add_value(closure->call(args));
        }
        return std::static_pointer_cast<Value>(newlist);
    };
    set("map", std::make_shared<FunctionValue>(map_function_pointer));


// for-each
    auto for_each_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        auto list = std::static_pointer_cast<ListValue>(argv[1]);
        // it's a function or a Closure
        if (argv[1]->get_type() == ValueType::Function) {
            auto func = std::static_pointer_cast<FunctionValue>(argv[0]);
            for (int i = 0; i < list->size(); i++) {
                std::vector<ValuePtr> args;
                args.push_back(list->get_value(i));
                apply_fn(func, args);
            }
            return std::static_pointer_cast<Value>(std::make_shared<NilValue>());
        }
        auto closure = std::static_pointer_cast<Closure>(argv[0]);
        for (int i = 0; i < list->size(); i++) {
            std::vector<ValuePtr> args;
            args.push_back(list->get_value(i));
            closure->call(args);
        }
        return std::static_pointer_cast<Value>(std::make_shared<NilValue>());
    };
    set("for-each", std::make_shared<FunctionValue>(for_each_function_pointer));


//    Two structured mutable objects are operationally equivalent
//    if they have operationally equivalent values in corresponding positions,
//    and applying a mutation procedure to one causes the other to change as well.
    auto eqv_question_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        if (argv[0]->get_type() != argv[1]->get_type()) {
            return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(false));
        }
        if (argv[0]->get_type() == ValueType::Integer) {
            return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(
                    std::static_pointer_cast<IntegerValue>(argv[0])->get_value() ==
                    std::static_pointer_cast<IntegerValue>(argv[1])->get_value()));
        }
        if (argv[0]->get_type() == ValueType::Float) {
            return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(
                    std::static_pointer_cast<FloatValue>(argv[0])->get_value() ==
                    std::static_pointer_cast<FloatValue>(argv[1])->get_value()));
        }
        if (argv[0]->get_type() == ValueType::Bool) {
            return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(
                    std::static_pointer_cast<BoolValue>(argv[0])->is_true() ==
                    std::static_pointer_cast<BoolValue>(argv[1])->is_true()));
        }

        if (argv[0]->get_type() == ValueType::Symbol) {
            return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(
                    std::static_pointer_cast<SymbolValue>(argv[0])->to_string() ==
                    std::static_pointer_cast<SymbolValue>(argv[1])->to_string()));
        }
        if (argv[0]->get_type() == ValueType::Nil) {
            return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(true));
        }
        // strings (implementation defined)
        if (argv[0]->get_type() == ValueType::String) {
            return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(
                    std::static_pointer_cast<StringValue>(argv[0])->to_string() ==
                    std::static_pointer_cast<StringValue>(argv[1])->to_string()));
        }
        // empty lists
        if (argv[0]->get_type() == ValueType::List) {
            if (std::static_pointer_cast<ListValue>(argv[0])->size() == 0 &&
                std::static_pointer_cast<ListValue>(argv[1])->size() == 0) {
                return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(true));
            }
        }

        // otherwise it has to point to the same thing
        return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(argv[0] == argv[1]));


    };
    set("eqv?", std::make_shared<FunctionValue>(eqv_question_function_pointer));

// Eq?’s behavior on numbers and characters is implementation-dependent, but it will always return either true or false,
// and will return true only when eqv? would also return true. Eq? may also behave differently from eqv? on empty vectors and empty strings.
//    => simplify so they do the same thing
    auto eq_question_function_pointer = eqv_question_function_pointer;
    set("eq?", std::make_shared<FunctionValue>(eq_question_function_pointer));


//A rule of thumb is that objects are generally equal? when they print the same.
    auto equal_question_function_pointer = [](size_t argc, std::vector<ValuePtr> &argv) {
        return std::static_pointer_cast<Value>(
                std::make_shared<BoolValue>(argv[0]->to_string() == argv[1]->to_string()));
    };
    set("equal?", std::make_shared<FunctionValue>(equal_question_function_pointer));

//    These procedures return the first sublist of list whose car
//    is obj . If obj does not occur in list, #f (n.b.: not the empty
//    list) is returned. Memq uses eq? to compare obj with the
//    elements of list, while memv uses eqv? and member uses
//    equal?
    auto generic_member_function = [](const auto &comparison_function, size_t argc, const std::vector<ValuePtr> &argv) {
        auto list = std::static_pointer_cast<ListValue>(argv[1]);
        for (int i = 0; i < list->size(); i++) {
            auto val = list->get_value(i);
            std::vector<ValuePtr> args = {argv[0], val};
            if (comparison_function(2, args)->is_true()) {
                return std::static_pointer_cast<Value>(list->get_sublist(i));
            }
        }
        return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(false));
    };

    set("memq", std::make_shared<FunctionValue>(
            [generic_member_function, &eq_question_function_pointer](auto &&PH1, auto &&PH2) {
                return generic_member_function(eq_question_function_pointer, std::forward<decltype(PH1)>(PH1),
                                               std::forward<decltype(PH2)>(PH2));
            }));

    set("memv", std::make_shared<FunctionValue>(
            [generic_member_function, &eqv_question_function_pointer](auto &&PH1, auto &&PH2) {
                return generic_member_function(eqv_question_function_pointer, std::forward<decltype(PH1)>(PH1),
                                               std::forward<decltype(PH2)>(PH2));
            }));

    set("member", std::make_shared<FunctionValue>(
            [generic_member_function, &equal_question_function_pointer](auto &&PH1, auto &&PH2) {
                return generic_member_function(equal_question_function_pointer, std::forward<decltype(PH1)>(PH1),
                                               std::forward<decltype(PH2)>(PH2));
            }));

//    Alist (for “association list”) must be a list of pairs. These
//    procedures find the first pair in alist whose car field is obj ,
//    and returns that pair. If no pair in alist has obj as its car,
//    #f is returned. Assq uses eq? to compare obj with the car
//    fields of the pairs in alist, while assv uses eqv? and assoc
//    uses equal?
    auto generic_assoc_function = [](const auto &comparison_function, size_t argc, const std::vector<ValuePtr> &argv) {
        auto list = std::static_pointer_cast<ListValue>(argv[1]);
        for (int i = 0; i < list->size(); i++) {
            auto val = list->get_value(i);
            std::vector<ValuePtr> args = {argv[0], car<Value>(val)};
            if (comparison_function(2, args)->is_true()) {
                return std::static_pointer_cast<Value>(val);
            }
        }
        return std::static_pointer_cast<Value>(std::make_shared<BoolValue>(false));
    };

    set("assq", std::make_shared<FunctionValue>(
            [generic_assoc_function, &eq_question_function_pointer](auto &&PH1, auto &&PH2) {
                return generic_assoc_function(eq_question_function_pointer, std::forward<decltype(PH1)>(PH1),
                                              std::forward<decltype(PH2)>(PH2));
            }));
    set("assv", std::make_shared<FunctionValue>(
            [generic_assoc_function, &eqv_question_function_pointer](auto &&PH1, auto &&PH2) {
                return generic_assoc_function(eqv_question_function_pointer, std::forward<decltype(PH1)>(PH1),
                                              std::forward<decltype(PH2)>(PH2));
            }));
    set("assoc", std::make_shared<FunctionValue>(
            [generic_assoc_function, &equal_question_function_pointer](auto &&PH1, auto &&PH2) {
                return generic_assoc_function(equal_question_function_pointer, std::forward<decltype(PH1)>(PH1),
                                              std::forward<decltype(PH2)>(PH2));
            }));


}









