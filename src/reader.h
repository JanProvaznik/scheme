#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <utility>
#include "datatypes/types.h"

enum class TOKEN_TYPE
{
  LPAREN,
  RPAREN,
  STRING,
  INTEGER,
  FLOAT,
  IDENTIFIER,
  QUOTE,
  EOF_TOKEN,
  TRUE,
  FALSE,
};

class Tokenizer
{
std::pair<TOKEN_TYPE, std::string> current_token;
public:
explicit Tokenizer(const std::string& source);

std::pair<TOKEN_TYPE, std::string> next_token();
std::pair<TOKEN_TYPE, std::string> peek_token();
std::vector<std::pair<TOKEN_TYPE, std::string> > tokenize();

private:
bool is_scheme_alpha(unsigned char c);

std::string source;
size_t pos;
};

// takes a string tokenizes it
class Reader
{
std::shared_ptr<ListValue> read_list(Tokenizer& tokenizer);
std::shared_ptr<Value> read_atom(Tokenizer& tokenizer);

public:

std::shared_ptr<Value> read_form(Tokenizer &tokenizer);
};
#endif // TOKENIZER_H
