#include <string>
#include <vector>
#include <string_view>
#include <iostream>
#include <utility>

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

#ifndef TOKENIZER_H
#define TOKENIZER_H
class Tokenizer
{
public:
explicit Tokenizer(const std::string& source);

std::pair<TOKEN_TYPE, std::string_view> next_token();
std::vector<std::pair<TOKEN_TYPE, std::string_view> > tokenize();

private:
bool is_scheme_alpha(unsigned char c);

std::string_view source;
size_t pos;
};
#endif // TOKENIZER_H
