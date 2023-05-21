#include "../src/reader.h"
#include "../src/evaluator.h"


void test_tokenizer() {
    std::string source = "'(define (factorial n) (if (= n 0) 1 (* n (factorial (- n 1)))))";
    Tokenizer tokenizer(source);
    int i = 0;
    std::vector<TOKEN_TYPE> types = {TOKEN_TYPE::QUOTE, TOKEN_TYPE::LPAREN, TOKEN_TYPE::IDENTIFIER,
                                     TOKEN_TYPE::LPAREN, TOKEN_TYPE::IDENTIFIER, TOKEN_TYPE::IDENTIFIER,
                                     TOKEN_TYPE::RPAREN, TOKEN_TYPE::LPAREN, TOKEN_TYPE::IDENTIFIER, TOKEN_TYPE::LPAREN,
                                     TOKEN_TYPE::IDENTIFIER, TOKEN_TYPE::IDENTIFIER, TOKEN_TYPE::INTEGER,
                                     TOKEN_TYPE::RPAREN,
                                     TOKEN_TYPE::INTEGER, TOKEN_TYPE::LPAREN, TOKEN_TYPE::IDENTIFIER,
                                     TOKEN_TYPE::IDENTIFIER,
                                     TOKEN_TYPE::LPAREN, TOKEN_TYPE::IDENTIFIER, TOKEN_TYPE::LPAREN,
                                     TOKEN_TYPE::IDENTIFIER,
                                     TOKEN_TYPE::IDENTIFIER, TOKEN_TYPE::INTEGER, TOKEN_TYPE::RPAREN,
                                     TOKEN_TYPE::RPAREN,
                                     TOKEN_TYPE::RPAREN, TOKEN_TYPE::RPAREN, TOKEN_TYPE::RPAREN, TOKEN_TYPE::EOF_TOKEN};
    while (true) {
        auto [type, token] = tokenizer.peek_token();
        if (type != types[i++]) {

            std::cout << "Error: " << token << " is not of the correct type " << std::endl;

        }
        if (type == TOKEN_TYPE::EOF_TOKEN) {
            break;
        }
        tokenizer.next_token();
    }
}

void test_car_cdr() {
    // test car and cdr
    auto list = std::make_shared<ListValue>();
    list->add_value(std::make_shared<IntegerValue>(1));
    list->add_value(std::make_shared<IntegerValue>(2));
    list->add_value(std::make_shared<IntegerValue>(3));

    auto car_result = car<IntegerValue>(list);
    auto cdr_result = cdr(list);

    if (car_result->get_value() != 1) {
        std::cout << "car test failed" << std::endl;
    }
    if (cdr_result->size() != 2) {
        std::cout << "cdr test failed" << std::endl;
    }
    if (cdr_result->get_value(0)->to_string() != "2") {
        std::cout << "cdr test failed" << std::endl;
    }
    if (cdr_result->get_value(1)->to_string() != "3") {
        std::cout << "cdr test failed" << std::endl;
    }

    // test car and cdr with a list of one element
    auto list2 = std::make_shared<ListValue>();
    list2->add_value(std::make_shared<IntegerValue>(1));
    auto car_result2 = car<IntegerValue>(list2);
    auto cdr_result2 = cdr(list2);

    if (car_result2->get_value() != 1) {
        std::cout << "car test failed" << std::endl;
    }
    if (cdr_result2->size() != 0) {
        std::cout << "cdr test failed" << std::endl;
    }
}

// end-to-end tests
bool eval_from_string_test(const std::string& source, const std::string& expected_output, const EnvironmentPtr &env) {
    Tokenizer tokenizer(source);
    Reader reader;
    auto ast = reader.read_form(tokenizer);
    auto result = eval(ast, env);
    if (result->to_string() != expected_output) {
        // compare char by char
        int differs_at = -1;
        for (int i = 0; i < std::min(result->to_string().size(), expected_output.size()); i++) {
            if (result->to_string()[i] != expected_output[i]) {
                differs_at = i;
                break;
            }
        }
        std::cout << "Error: " << source << " did not evaluate to " << expected_output << " but to: "
                  << result->to_string() << " " <<
                  " difference at position " << differs_at << std::endl;
        return false;
    }
    return true;

}

void test_read_numbers() {
    EnvironmentPtr env = std::make_shared<BaseEnvironment>();
    auto input_output_pairs = {
            std::make_pair("1", "1"),
            std::make_pair("7", "7"),
            std::make_pair("7", "7"),
            std::make_pair("-123", "-123"),
    };
    for (auto [input, output]: input_output_pairs) {
        eval_from_string_test(input, output, env);
    }
}

void test_read_nil_true_false() {
    EnvironmentPtr env = std::make_shared<BaseEnvironment>();
    auto input_output_pairs = {
            std::make_pair("nil", "nil"),
            std::make_pair("true", "#t"),
            std::make_pair("false", "#f"),
            std::make_pair("else", "#t"),
            std::make_pair("#t", "#t"),
            std::make_pair("#f", "#f"),
    };
    for (auto [input, output]: input_output_pairs) {
        eval_from_string_test(input, output, env);
    }
}

void test_read_lists() {
    EnvironmentPtr env = std::make_shared<BaseEnvironment>();
    auto input_output_pairs = {
            std::make_pair("'(+ 1 2)", "(+ 1 2)"),
            std::make_pair("( )", "()"),
            std::make_pair("'((3 4))", "((3 4))"),
            std::make_pair("'(+ 1 (+ 2 3))", "(+ 1 (+ 2 3))"),
            std::make_pair("'( +   1   (+   2 3   )   )", "(+ 1 (+ 2 3))"),
            std::make_pair("'(* 1 2)", "(* 1 2)"),
            std::make_pair("'(** 1 2)", "(** 1 2)"),
            std::make_pair("'(* -3 6)", "(* -3 6)"),
            std::make_pair("'(()())", "(() ())"),
            std::make_pair("'()", "()"),
            std::make_pair("()", "()"),
    };
    for (auto [input, output]: input_output_pairs) {
        eval_from_string_test(input, output, env);
    }

}

void test_read_strings() {
    EnvironmentPtr env = std::make_shared<BaseEnvironment>();
    auto input_output_pairs = {
            std::make_pair("\"abc\"", "\"abc\""),
            std::make_pair("\"abc (with parens)\"", "\"abc (with parens)\""),
            std::make_pair("\"\"", "\"\""),
            std::make_pair("\"&\"", "\"&\""),
            std::make_pair("\"'\"", "\"'\""),
            std::make_pair("\"(\"", "\"(\""),
            std::make_pair("\")\"", "\")\""),
            std::make_pair("\"*\"", "\"*\""),
            std::make_pair("\"+\"", "\"+\""),
            std::make_pair("\",\"", "\",\""),
            std::make_pair("\"-\"", "\"-\""),
            std::make_pair("\"/\"", "\"/\""),
            std::make_pair("\":\"", "\":\""),
            std::make_pair("\";\"", "\";\""),
            std::make_pair("\"<\"", "\"<\""),
            std::make_pair("\"=\"", "\"=\""),
            std::make_pair("\">\"", "\">\""),
            std::make_pair("\"?\"", "\"?\""),
            std::make_pair("\"@\"", "\"@\""),
            std::make_pair("\"[\"", "\"[\""),
            std::make_pair("\"]\"", "\"]\""),
            std::make_pair("\"^\"", "\"^\""),
            std::make_pair("\"_\"", "\"_\""),
            std::make_pair("\"`\"", "\"`\""),
            std::make_pair("\"{\"", "\"{\""),
            std::make_pair("\"}\"", "\"}\""),
            std::make_pair("\"~\"", "\"~\""),
            std::make_pair("\"!\"", "\"!\""),
    };
    for (auto [input, output]: input_output_pairs) {
        eval_from_string_test(input, output, env);
    }


}

void test_arithmetic() {
    EnvironmentPtr env = std::make_shared<BaseEnvironment>();

    auto input_output_pairs = {
            std::make_pair("(+ 1 2)", "3"),
            std::make_pair("(+ 5 (* 2 3))", "11"),
            std::make_pair("(- (+ 5 (* 2 3)) 3)", "8"),
            std::make_pair("(/ (- (+ 5 (* 2 3)) 3) 4)", "2"),
            std::make_pair("(/ (- (+ 515 (* 87 311)) 302) 27)", "1010"),
            std::make_pair("(* -3 6)", "-18"),
            std::make_pair("(/ (- (+ 515 (* -87 311)) 296) 27)", "-994"),
    };
    for (auto [input, output]: input_output_pairs) {
        eval_from_string_test(input, output, env);
    }
}

void test_case_insensitive() {
    EnvironmentPtr env = std::make_shared<BaseEnvironment>();

    auto input_output_pairs = {
            std::make_pair("(define mynum 111)", "111"),
            std::make_pair("MyNuM", "111"),
    };
    for (auto [input, output]: input_output_pairs) {
        eval_from_string_test(input, output, env);
    }
}

void test_list_functions() {
    EnvironmentPtr env = std::make_shared<BaseEnvironment>();

    auto input_output_pairs = {
            std::make_pair("(list)", "()"),
            std::make_pair("(list? (list))", "#t"),
            std::make_pair("(empty? (list))", "#t"),
            std::make_pair("(empty? (list 1))", "#f"),
            std::make_pair("(list 1 2 3)", "(1 2 3)"),
            std::make_pair("(count (list 1 2 3))", "3"),
            std::make_pair("(length (list))", "0"),
            std::make_pair("(if (> (count (list 1 2 3)) 3) 89 78)", "78"),
            std::make_pair("(if (>= (count (list 1 2 3)) 3) 89 78)", "89"),
    };
    for (auto [input, output]: input_output_pairs) {
        eval_from_string_test(input, output, env);
    }
}

void test_if() {
    EnvironmentPtr env = std::make_shared<BaseEnvironment>();

    auto input_output_pairs = {
            std::make_pair("(if true 7 8)", "7"),
            std::make_pair("(if false 7 8)", "8"),
            std::make_pair("(if false 7 false)", "#f"),
            std::make_pair("(if true (+ 1 7) (+ 1 8))", "8"),
            std::make_pair("(if false (+ 1 7) (+ 1 8))", "9"),
            std::make_pair("(if nil 7 8)", "8"),
            std::make_pair("(if 0 7 8)", "8"),
            std::make_pair("(if (list) 7 8)", "8"),
            std::make_pair("(if (list 1 2 3) 7 8)", "8"),
    };
    for (auto [input, output]: input_output_pairs) {
        eval_from_string_test(input, output, env);
    }
}

void test_num_conditionals() {

    EnvironmentPtr env = std::make_shared<BaseEnvironment>();
    auto input_output_pairs = {
            std::make_pair("(= 2 1)", "#f"),
            std::make_pair("(= 1 1)", "#t"),
            std::make_pair("(= 1 2)", "#f"),
            std::make_pair("(= 1 (+ 1 1))", "#f"),
            std::make_pair("(= 2 (+ 1 1))", "#t"),
            std::make_pair("(> 2 1)", "#t"),
            std::make_pair("(> 1 1)", "#f"),
            std::make_pair("(> 1 2)", "#f"),
            std::make_pair("(>= 2 1)", "#t"),
            std::make_pair("(>= 1 1)", "#t"),
            std::make_pair("(>= 1 2)", "#f"),
            std::make_pair("(< 2 1)", "#f"),
            std::make_pair("(< 1 1)", "#f"),
            std::make_pair("(< 1 2)", "#t"),
            std::make_pair("(<= 2 1)", "#f"),
            std::make_pair("(<= 1 1)", "#t"),
            std::make_pair("(<= 1 2)", "#t"),
            std::make_pair("(= 1 1)", "#t"),
            std::make_pair("(= 0 0)", "#t"),
            std::make_pair("(= 1 0)", "#f"),
    };
    for (auto [input, output]: input_output_pairs) {
        eval_from_string_test(input, output, env);
    }

}

void test_lambda() {
    EnvironmentPtr env = std::make_shared<BaseEnvironment>();

    auto input_output_pairs = {
            std::make_pair("(+ 1 2)", "3"),
            std::make_pair("( (lambda (a b) (+ b a)) 3 4)", "7"),
            std::make_pair("( (lambda () 4) )", "4"),
            std::make_pair("( (lambda (f x) (f x)) (lambda (a) (+ 1 a)) 7)", "8"),
            std::make_pair("( ( (lambda (a) (lambda (b) (+ a b))) 5) 7)", "12"),
            std::make_pair("(define gen-plus5 (lambda () (lambda (b) (+ 5 b))))", "#<Closure>"),
            std::make_pair("(define plus5 (gen-plus5))", "#<Closure>"),
            std::make_pair("(plus5 7)", "12"),
            std::make_pair("(define gen-plusX (lambda (x) (lambda (b) (+ x b))))", "#<Closure>"),
            std::make_pair("(define plus7 (gen-plusX 7))", "#<Closure>"),
            std::make_pair("(plus7 8)", "15"),

//        recursive sumdown function
            std::make_pair("(define sumdown (lambda (N) (if (> N 0) (+ N (sumdown  (- N 1))) 0)))", "#<Closure>"),
            std::make_pair("(sumdown 1)", "1"),
            std::make_pair("(sumdown 2)", "3"),
            std::make_pair("(sumdown 6)", "21"),
//        recursive fibonacci function
            std::make_pair("(define fib (lambda (N) (if (= N 0) 1 (if (= N 1) 1 (+ (fib (- N 1)) (fib (- N 2)))))))",
                           "#<Closure>"),
            std::make_pair("(fib 1)", "1"),
            std::make_pair("(fib 2)", "2"),
            std::make_pair("(fib 4)", "5"),
    };
    for (auto [input, output]: input_output_pairs) {
        eval_from_string_test(input, output, env);
    }


}

void test_string_equality() {
    EnvironmentPtr env = std::make_shared<BaseEnvironment>();
    auto input_output_pairs = {
            std::make_pair(R"((equal? "" ""))", "#t"),
            std::make_pair(R"((equal? "abc" "abc"))", "#t"),
            std::make_pair(R"((equal? "abc" ""))", "#f"),
            std::make_pair(R"((equal? "" "abc"))", "#f"),
            std::make_pair(R"((equal? "abc" "def"))", "#f"),
            std::make_pair(R"((equal? "abc" "ABC"))", "#f"),
            std::make_pair("(equal? (list) \"\")", "#f"),
            std::make_pair("(equal? \"\" (list))", "#f"),
    };
    for (auto [input, output]: input_output_pairs) {
        eval_from_string_test(input, output, env);
    }

}

void test_tail_calls() {

    EnvironmentPtr env = std::make_shared<BaseEnvironment>();

    auto input_output_pairs = {
            std::make_pair("(define sum2 (lambda (n acc) (if (= n 0) acc (sum2 (- n 1) (+ n acc)))))", "#<Closure>"),
            std::make_pair("(sum2 10 0)", "55"),
            std::make_pair("(define res2 nil)", "nil"),
            std::make_pair("(define res2 (sum2 10000 0))", "50005000"),
            std::make_pair("(define foo (lambda (n) (if (= n 0) 0 (bar (- n 1)))))", "#<Closure>"),
            std::make_pair("(define bar (lambda (n) (if (= n 0) 0 (foo (- n 1)))))", "#<Closure>"),
            std::make_pair("(foo 10000)", "0"),
            std::make_pair("(begin (begin 1 2))", "2"),
    };

    for (auto [input, output]: input_output_pairs) {
        eval_from_string_test(input, output, env);
    }

}

void test_cons_concat() {
    EnvironmentPtr env = std::make_shared<BaseEnvironment>();
    auto input_output_pairs = {
            std::make_pair("(cons 1 (list))", "(1)"),
            std::make_pair("(cons 1 (list 2))", "(1 2)"),
            std::make_pair("(cons 1 (list 2 3))", "(1 2 3)"),
            std::make_pair("(cons (list 1) (list 2 3))", "((1) 2 3)"),
            std::make_pair("(define a (list 2 3))", "(2 3)"),
            std::make_pair("(cons 1 a)", "(1 2 3)"),
            std::make_pair("a", "(2 3)"),
            std::make_pair("(concat (list 1 2) (list 3 4))", "(1 2 3 4)"),
            std::make_pair("(concat (list) (list))", "()"),
    };

    for (auto [input, output]: input_output_pairs) {
        eval_from_string_test(input, output, env);
    }
}

void test_quote() {
    EnvironmentPtr env = std::make_shared<BaseEnvironment>();

    auto input_output_pairs = {
            std::make_pair("(quote 7)", "7"),
            std::make_pair("(quote (1 2 3))", "(1 2 3)"),
            std::make_pair("(quote (1 2 (3 4)))", "(1 2 (3 4))"),
            std::make_pair("(equal? (quote abc) (quote abc))", "#t"),
            std::make_pair("(equal? (quote abc) (quote abcd))", "#f"),
            std::make_pair("(equal? (quote abc) \"abc\")", "#f"),
            std::make_pair("(equal? \"abc\" (quote abc))", "#f"),
            std::make_pair("(equal? (quote abc) nil)", "#f"),
            std::make_pair("(equal? nil (quote abc))", "#f"),
            std::make_pair("'7", "7"),
            std::make_pair("'(1 2 3)", "(1 2 3)"),
            std::make_pair("'(1 2 (3 4))", "(1 2 (3 4))"),
            std::make_pair("(symbol? 'abc)", "#t"),
            std::make_pair("(symbol? \"abc\")", "#f"),

    };

    for (auto [input, output]: input_output_pairs) {
        eval_from_string_test(input, output, env);
    }
}

void test_map() {

    EnvironmentPtr env = std::make_shared<BaseEnvironment>();

    auto input_output_pairs = {
            std::make_pair("(define nums (list 1 2 3))", "(1 2 3)"),
            std::make_pair("(define double (lambda (a) (* 2 a)))", "#<Closure>"),
            std::make_pair("(double 3)", "6"),
            std::make_pair("(map double nums)", "(2 4 6)"),
            std::make_pair("(map (lambda (x) (symbol? x)) (list 1 (quote two) \"three\"))", "(#f #t #f)"),
    };

    for (auto [input, output]: input_output_pairs) {
        eval_from_string_test(input, output, env);
    }
}

void test_eq(){
    EnvironmentPtr env = std::make_shared<BaseEnvironment>();
    auto input_output_pairs = {
            std::make_pair("(eq? 'a 'a)", "#t"),
            std::make_pair("(eq? (list 'a) (list 'a))", "#f"),
            std::make_pair("(eq? '() '())", "#t"),
            std::make_pair("(eq? car car)", "#t"),
            std::make_pair("(let ((x '(a))) (eq? x x))", "#t"),
            std::make_pair("(let ((p (lambda (x) x))) (eq? p p))", "#t"),
    };
    for (auto [input, output]: input_output_pairs) {
        eval_from_string_test(input, output, env);
    }
}
void test_eqv(){
    EnvironmentPtr env = std::make_shared<BaseEnvironment>();
    auto input_output_pairs = {
            std::make_pair("(eqv? 'a 'a)", "#t"),
            std::make_pair("(eqv? 'a 'b)", "#f"),
            std::make_pair("(eqv? 2 2)", "#t"),
            std::make_pair("(eqv? (list 'a) (list 'a))", "#f"),
            std::make_pair("(eqv? '() '())", "#t"),
            std::make_pair("(eqv? car car)", "#t"),
            std::make_pair("(let ((x '(a))) (eqv? x x))", "#t"),
            std::make_pair("(let ((p (lambda (x) x))) (eqv? p p))", "#t"),
            std::make_pair(R"((eqv? "" ""))", "#t"),
            std::make_pair("(eqv? 100000000 100000000)", "#t"),
            std::make_pair("(eqv? (lambda () 1) (lambda () 2))", "#f"),
            std::make_pair("(eqv? #f 'nil)", "#f"),

    };
    for (auto [input, output]: input_output_pairs) {
        eval_from_string_test(input, output, env);
    }
}

void test_equal(){

    EnvironmentPtr env = std::make_shared<BaseEnvironment>();
    auto input_output_pairs = {
            std::make_pair("(equal? 'a 'a)", "#t"),
            std::make_pair("(equal? '(a) '(a))", "#t"),
            std::make_pair("(equal? '(a (b) c) '(a (b) c))", "#t"),
            std::make_pair(R"((equal? "abc" "abc"))", "#t"),
            std::make_pair("(equal? 2 2)", "#t"),
            std::make_pair("(equal? (make-vector 5 'a) (make-vector 5 'a))", "#t"),
    };


    for (auto [input, output]: input_output_pairs) {
        eval_from_string_test(input, output, env);
    }
}
void test_define(){
    // define function does not work, if you want to use functions use lambda
    EnvironmentPtr env = std::make_shared<BaseEnvironment>();
    auto input_output_pairs = {
            std::make_pair("(define x 3)", "3"),
            std::make_pair("x", "3"),
            std::make_pair("(define x 4)", "4"),
            std::make_pair("x", "4"),
            std::make_pair("(define x (lambda (x) (* x x)))", "#<Closure>"),
            std::make_pair("(x 3)", "9"),
            std::make_pair("(define x 5)", "5"),
            std::make_pair("(define x (lambda (x) (* x x)))", "#<Closure>"),
            std::make_pair("(x 3)", "9")
    };
    for (auto [input, output]: input_output_pairs) {
        eval_from_string_test(input, output, env);
    }

}
void test_let(){
    // let letrec and let* are all the same
    EnvironmentPtr env = std::make_shared<BaseEnvironment>();
    auto input_output_pairs = {
            std::make_pair("(let ((x 2) (y 3)) (* x y))", "6"),
            std::make_pair("(let ((x 2) (y 3)) (let ((x 7) (z (+ x y))) (* z x)))", "35"),
            std::make_pair("(let ((x 2) (y 3)) (let* ((x 7) (z (+ x y))) (* z x)))", "70"),
            std::make_pair("(let ((x 2) (y 3)) (letrec ((foo (lambda (z) (+ x y z)))) (foo 4)))", "9"),
            std::make_pair("(let ((x 2) (y 3)) (let ((x 7) (z (+ x y))) (* z x)))", "35"),
            std::make_pair("(let ((x 2) (y 3)) (letrec ((foo (lambda (z) (+ x y z)))) (foo 4)))", "9"),
            std::make_pair("(let ((x 2) (y 3)) (letrec ((foo (lambda (z) (+ x y z)))) (let ((x 7) (z (+ x y))) (* z (foo 4)))))", "63"),
            std::make_pair("(let ((x 2) (y 3)) (let* ((x 7) (z (+ x y))) (* z x)))", "70")
    };
    for (auto [input, output]: input_output_pairs) {
        eval_from_string_test(input, output, env);
    }

}
void test_cond(){

    EnvironmentPtr env = std::make_shared<BaseEnvironment>();

    auto input_output_pairs = {
            std::make_pair("(cond ((> 3 2) 'greater) ((< 3 2) 'less))", "greater"),
            std::make_pair("(cond ((> 3 3) 'greater) ((< 3 3) 'less) (else 'equal))", "equal"),
            std::make_pair("(cond ((> 3 3) 'greater) ((< 3 3) 'less))", "#f"),
            std::make_pair("(cond ((> 3 2) 'greater))", "greater"),
            std::make_pair("(cond ((> 3 3) 'greater))", "#f")
    };
    for (auto [input, output]: input_output_pairs) {
        eval_from_string_test(input, output, env);
    }

}

// - abs, quotient, remainder, max, min
void test_num_functions(){

    EnvironmentPtr env = std::make_shared<BaseEnvironment>();

    auto input_output_pairs = {
            std::make_pair("(abs -7)", "7"),
            std::make_pair("(abs 7)", "7"),
            std::make_pair("(abs 0)", "0"),
            std::make_pair("(abs -2.5)", "2.5"),
            std::make_pair("(abs 2.5)", "2.5"),
            std::make_pair("(abs 0.0)", "0"),

            std::make_pair("(quotient 6 4)", "1"),
            std::make_pair("(quotient 6 3)", "2"),
            std::make_pair("(quotient 6 2)", "3"),

            std::make_pair("(remainder 6 4)", "2"),
            std::make_pair("(remainder 6 3)", "0"),
            std::make_pair("(remainder 6 2)", "0"),

            std::make_pair("(max 6 4)", "6"),
            std::make_pair("(max 6.0 3.1)", "6"),
            std::make_pair("(max -1 2)", "2"),

            std::make_pair("(min 6 4)", "4"),
            std::make_pair("(min 6.0 3.1)", "3.1"),
            std::make_pair("(min -1 2)", "-1"),
    };
    for (auto [input, output]: input_output_pairs) {
        eval_from_string_test(input, output, env);
    }
}


void test_type_questions(){

    EnvironmentPtr env = std::make_shared<BaseEnvironment>();
    auto input_output_pairs = {
// - number? 
            std::make_pair("(number? 1)", "#t"),
            std::make_pair("(number? 1.0)", "#t"),
            std::make_pair("(number? #t)", "#f"),
            std::make_pair("(number? 'a)", "#f"),
            std::make_pair("(number? \"a\")", "#f"),
            std::make_pair("(number? (cons 1 2))", "#f"),
            std::make_pair("(number? (lambda (x) x))", "#f"),
// real? 
            std::make_pair("(real? 1)", "#t"),
            std::make_pair("(real? 1.0)", "#t"),
            std::make_pair("(real? #t)", "#f"),
            std::make_pair("(real? 'a)", "#f"),
            std::make_pair("(real? \"a\")", "#f"),
            std::make_pair("(real? (cons 1 2))", "#f"),
            std::make_pair("(real? (lambda (x) x))", "#f"),
// integer? 
            std::make_pair("(integer? 1)", "#t"),
            std::make_pair("(integer? 1.0)", "#f"),
            std::make_pair("(integer? #t)", "#f"),
            std::make_pair("(integer? 'a)", "#f"),
            std::make_pair("(integer? \"a\")", "#f"),
            std::make_pair("(integer? (cons 1 2))", "#f"),
            std::make_pair("(integer? (lambda (x) x))", "#f"),
// rational?
            std::make_pair("(rational? 1)", "#t"),
            std::make_pair("(rational? 1.0)", "#f"),
            std::make_pair("(rational? #t)", "#f"),
            std::make_pair("(rational? 'a)", "#f"),
            std::make_pair("(rational? \"a\")", "#f"),
            std::make_pair("(rational? (cons 1 2))", "#f"),
            std::make_pair("(rational? (lambda (x) x))", "#f"),
// boolean? 
            std::make_pair("(boolean? 1)", "#f"),
            std::make_pair("(boolean? 1.0)", "#f"),
            std::make_pair("(boolean? #t)", "#t"),
            std::make_pair("(boolean? 'a)", "#f"),
            std::make_pair("(boolean? \"a\")", "#f"),
            std::make_pair("(boolean? (cons 1 2))", "#f"),
            std::make_pair("(boolean? (lambda (x) x))", "#f"),
// symbol? 
            std::make_pair("(symbol? 1)", "#f"),
            std::make_pair("(symbol? 1.0)", "#f"),
            std::make_pair("(symbol? #t)", "#f"),
            std::make_pair("(symbol? 'a)", "#t"),
            std::make_pair("(symbol? \"a\")", "#f"),
            std::make_pair("(symbol? (cons 1 2))", "#f"),
            std::make_pair("(symbol? (lambda (x) x))", "#f"),
// string? 
            std::make_pair("(string? 1)", "#f"),
            std::make_pair("(string? 1.0)", "#f"),
            std::make_pair("(string? #t)", "#f"),
            std::make_pair("(string? 'a)", "#f"),
            std::make_pair("(string? \"a\")", "#t"),
            std::make_pair("(string? (cons 1 2))", "#f"),
            std::make_pair("(string? (lambda (x) x))", "#f"),

// pair? 
            std::make_pair("(pair? 1)", "#f"),
            std::make_pair("(pair? 1.0)", "#f"),
            std::make_pair("(pair? #t)", "#f"),
            std::make_pair("(pair? 'a)", "#f"),
            std::make_pair("(pair? \"a\")", "#f"),
            std::make_pair("(pair? (cons 1 2))", "#t"),
            std::make_pair("(pair? (lambda (x) x))", "#f"),
// list?
            std::make_pair("(list? 1)", "#f"),
            std::make_pair("(list? 1.0)", "#f"),
            std::make_pair("(list? #t)", "#f"),
            std::make_pair("(list? 'a)", "#f"),
            std::make_pair("(list? \"a\")", "#f"),
            std::make_pair("(list? (cons 1 2))", "#t"),
            std::make_pair("(list? (lambda (x) x))", "#f"),
// vector?
            std::make_pair("(vector? 1)", "#f"),
            std::make_pair("(vector? 1.0)", "#f"),
            std::make_pair("(vector? #t)", "#f"),
            std::make_pair("(vector? 'a)", "#f"),
            std::make_pair("(vector? \"a\")", "#f"),
            std::make_pair("(vector? (cons 1 2))", "#f"),
            std::make_pair("(vector? (lambda (x) x))", "#f"),
// procedure?
            std::make_pair("(procedure? 1)", "#f"),
            std::make_pair("(procedure? 1.0)", "#f"),
            std::make_pair("(procedure? #t)", "#f"),
            std::make_pair("(procedure? 'a)", "#f"),
            std::make_pair("(procedure? \"a\")", "#f"),
            std::make_pair("(procedure? (cons 1 2))", "#f"),
            std::make_pair("(procedure? (lambda (x) x))", "#t"),

    };

    for (auto [input, output]: input_output_pairs) {
        eval_from_string_test(input, output, env);
    }

}

void test_num_questions() {
    EnvironmentPtr env = std::make_shared<BaseEnvironment>();

    auto input_output_pairs = {
// - zero? 
            std::make_pair("(zero? 0)", "#t"),
            std::make_pair("(zero? 1)", "#f"),
            std::make_pair("(zero? -1)", "#f"),
            std::make_pair("(zero? 1.0)", "#f"),
        // positive? 
            std::make_pair("(positive? 0)", "#f"),
            std::make_pair("(positive? 1)", "#t"),
            std::make_pair("(positive? -1)", "#f"),
            std::make_pair("(positive? 1.0)", "#t"),
        //negative?
            std::make_pair("(negative? 0)", "#f"),
            std::make_pair("(negative? 1)", "#f"),
            std::make_pair("(negative? -1)", "#t"),
            std::make_pair("(negative? 1.0)", "#f"),
        // odd?
            std::make_pair("(odd? 0)", "#f"),
            std::make_pair("(odd? 1)", "#t"),
            std::make_pair("(odd? 33421)", "#t"),
            std::make_pair("(odd? -1)", "#t"),
            std::make_pair("(odd? 2)", "#f"),

        // even?
            std::make_pair("(even? 0)", "#t"),
            std::make_pair("(even? 1)", "#f"),
            std::make_pair("(even? 33421)", "#f"),
            std::make_pair("(even? -1)", "#f"),
            std::make_pair("(even? 2)", "#t"),
        // exact?
            std::make_pair("(exact? 0)", "#t"),
            std::make_pair("(exact? 33421)", "#t"),
            std::make_pair("(exact? -1)", "#t"),
            std::make_pair("(exact? 1.0)", "#f"),
        // inexact? 
            std::make_pair("(inexact? 0)", "#f"),
            std::make_pair("(inexact? 33421)", "#f"),
            std::make_pair("(inexact? -1)", "#f"),
            std::make_pair("(inexact? 1.0)", "#t"),


    };
    for (auto [input, output]: input_output_pairs) {
        eval_from_string_test(input, output, env);
    }
}
// - car cdr, set-car, set-cdr

// null?

//- string->symbol, symbol->string

// - not and or 
// - length, append
// - string-length, string-ref
// - `string=?, string<?, string>?, string<=, string>=
// - make-vector,vector-length, vector-ref,vector-set!, vector->list,list->vector
// - for-each
// - memq,memv,member
// - assq, assv, assoc
// display write newline tested by running external file


void run_tests() {
    test_tokenizer();
    test_car_cdr();
    test_read_numbers();
    test_read_nil_true_false();
    test_read_lists();
    test_read_strings();
    test_arithmetic();
    test_case_insensitive();
    test_list_functions();
    test_if();
    test_num_conditionals();
    test_lambda();
    test_string_equality();
    test_tail_calls();
    test_cons_concat();
    test_quote();
    test_map();
    test_eq();
    test_eqv();
    test_equal();
    test_define();
    test_let();
    test_cond();
    test_num_functions();
    test_type_questions();
    test_num_questions();

}

int main(int argc, char **argv) {
    std::cout << "Running tests..." << std::endl;
    run_tests();
    std::cout << "Testing complete!" << std::endl;
    return 0;

}

