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

class Tokenizer
{

bool is_scheme_alpha(unsigned char c)
{
  // check if c is a scheme extended alpha character that can be used as an identifier
  // scheme symbols are any of the following characters: ! $ % & * + - . / : < = > ? @ ^ _ ~
  // or any alphanumeric character, //TODO: this is a bit ambiguous from the spec, I could also exclude numbers but that would be weird
  if (std::isalnum(c))
  {
    return true;
  }
  if (c == '!' || c == '$' || c == '%' || c == '&' || c == '*' || c == '+' || c == '-' || c == '.' || c == '/' || c == ':' || c == '<' || c == '=' || c == '>' || c == '?' || c == '@' || c == '^' || c == '_' || c == '~')
  {
    return true;
  }
  return false;
}

public:
explicit Tokenizer(const std::string &source) : source(source), pos(0)
{
}
// this could theoretically be done with regex but it seems very slow and unreliable
std::pair<TOKEN_TYPE, std::string_view> next_token()
{
  // skip whitespace
  while (pos < source.size() && std::isspace(source[pos]))
  {
    pos++;
  }
  if (pos == source.size())
  {
    return std::make_pair(TOKEN_TYPE::EOF_TOKEN, "");
  }
  if (source[pos] == '(')
  {
    pos++;
    return std::make_pair(TOKEN_TYPE::LPAREN, "(");
  }
  if (source[pos] == ')')
  {
    pos++;
    return std::make_pair(TOKEN_TYPE::RPAREN, ")");
  }
  if (source[pos] == '\'')
  {
    pos++;
    return std::make_pair(TOKEN_TYPE::QUOTE, "'");
  }
  if (source[pos] == '#')
  {
    pos++;
    if (pos < source.size() && source[pos] == 't')
    {
      pos++;
      return std::make_pair(TOKEN_TYPE::TRUE, "#t");
    }
    if (pos < source.size() && source[pos] == 'f')
    {
      pos++;
      return std::make_pair(TOKEN_TYPE::FALSE, "#f");
    }
    throw std::runtime_error("invalid boolean literal");
  }
  // comment
  if (source[pos] == ';')
  {
    // find the end of the comment
    size_t end = source.find('\n', pos + 1);
    if (end == std::string::npos)
    {
      // comment goes to the end of the file
      pos = source.size();
    }
    else
    {
      // skip the comment
      pos = end + 1;
    }
    return next_token();
  }

  // check if we have a string
  if (source[pos] == '"')
  {
    // find the end of the string
    size_t end = source.find('"', pos + 1);
    if (end == std::string::npos)
    {
      throw std::runtime_error("unterminated string");
    }
    // FIXME: escape sequences
    std::string_view token = source.substr(pos, end - pos + 1);
    pos = end + 1;
    return std::make_pair(TOKEN_TYPE::STRING, token);
  }
  // numbers
  // TODO: handle floats
  // TODO: consider handling weird representations such as hex
  if (std::isdigit(source[pos]))
  {
    // find the end of the number
    size_t end = pos;
    while (end < source.size() && std::isdigit(source[end]))
    {
      end++;
    }
    // return the number
    std::string_view token = source.substr(pos, end - pos);
    pos = end;
    return std::make_pair(TOKEN_TYPE::INTEGER, token);
  }
  // check if we have an identifier symbol, actually we don't have to consider numbers because are not mentioned in the spec
  if (is_scheme_alpha(source[pos]))
  {
    // find the end of the identifier
    size_t end = pos;
    while (end < source.size() && is_scheme_alpha(source[end]))
    {
      end++;
    }
    // return the symbol
    std::string_view token = source.substr(pos, end - pos);
    pos = end;
    return std::make_pair(TOKEN_TYPE::IDENTIFIER, token);
  }
  // we have an unknown token
  // TODO: nice error messages, preferrably with line numbers
  throw std::runtime_error("unknown token");
}

private:
std::string_view source;
size_t pos;
};
// tests for the tokenizer
void t1()
{
  std::string source = "(define (factorial n) (if (= n 0) 1 (* n (factorial (- n 1)))))";
  Tokenizer tokenizer(source);
  while (true)
  {
    auto [type, token] = tokenizer.next_token();
    if (type == TOKEN_TYPE::EOF_TOKEN)
    {
      break;
    }
    std::cout << token << std::endl;
  }
}
