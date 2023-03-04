#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <utility>
#include "reader.h"


Tokenizer::Tokenizer(const std::string &source) : source(source), pos(0)
{
    this->next_token();
}
// this could theoretically be done with regex, but it seems very slow and unreliable
std::pair<TOKEN_TYPE, std::string> Tokenizer::next_token()
{
  // skip whitespace
  while (pos < source.size() && std::isspace(source[pos]))
  {
    pos++;
  }
  if (pos == source.size())
  {
    auto pair = std::make_pair(TOKEN_TYPE::EOF_TOKEN, "");
    this->current_token = pair;
    return pair;

  }
  if (source[pos] == '(')
  {
    pos++;
    auto pair = std::make_pair(TOKEN_TYPE::LPAREN, "(");
    this->current_token = pair;
    return pair;
  }
  if (source[pos] == ')')
  {
    pos++;
    auto pair = std::make_pair(TOKEN_TYPE::RPAREN, ")");
    this->current_token = pair;
    return pair;
  }
  if (source[pos] == '\'')
  {
    pos++;
    auto pair = std::make_pair(TOKEN_TYPE::QUOTE, "'");
    this->current_token = pair;
    return pair;
  }
  // TODO quasiquote
  if (source[pos] == '#')
  {
    pos++;
    if (pos < source.size() && source[pos] == 't')
    {
      pos++;
      auto pair = std::make_pair(TOKEN_TYPE::TRUE, "#t");
        this->current_token = pair;
        return pair;
    }
    if (pos < source.size() && source[pos] == 'f')
    {
      pos++;
      auto pair = std::make_pair(TOKEN_TYPE::FALSE, "#f");
      this->current_token = pair;
      return pair;
    }
    throw std::runtime_error("invalid boolean literal");
  }
  // FIXME: isn't hash also used for vectors??
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
    std::string token = source.substr(pos, end - pos + 1);
    pos = end + 1;
    auto pair = std::make_pair(TOKEN_TYPE::STRING, token);
    this->current_token = pair;
    return pair;
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
    std::string token = source.substr(pos, end - pos);
    pos = end;
    auto pair = std::make_pair(TOKEN_TYPE::INTEGER, token);
    this->current_token = pair;
    return pair;
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
    std::string token = source.substr(pos, end - pos);
    pos = end;
    auto pair = std::make_pair(TOKEN_TYPE::IDENTIFIER, token);
    this->current_token = pair;
    return pair;
  }
  // we have an unknown token
  // TODO: nice error messages, preferrably with line numbers
  throw std::runtime_error("unknown token");
}


bool Tokenizer::is_scheme_alpha(unsigned char c)
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

std::vector<std::pair<TOKEN_TYPE, std::string> > Tokenizer::tokenize()
{
  std::vector<std::pair<TOKEN_TYPE, std::string> > tokens;
  while (pos < source.size())
  {
    tokens.push_back(next_token());
  }

  return tokens;
}

std::pair<TOKEN_TYPE, std::string> Tokenizer::peek_token() {
    return current_token;
}


std::shared_ptr<Value> Reader::read_form(Tokenizer &tokenizer) {

//    Add the function read_form to reader.qx.
//    This function will peek at the first token in the Reader object and switch on the first character of that token.
//    If the character is a left paren then read_list is called with the Reader object.
//    Otherwise, read_atom is called with the Reader Object. The return value from read_form is a mal data type.
//    If your target language is statically typed then you will need some way for read_form to return a variant or subclass type.
//    For example, if your language is object oriented, then you can define a top level MalType (in types.qx) that all your mal data types inherit from.
//    The MalList type (which also inherits from MalType) will contain a list/array of other MalTypes.
    auto top_token = tokenizer.peek_token();
    if (top_token.first == TOKEN_TYPE::LPAREN) {
        return read_list(tokenizer);
    } else {
        return read_atom(tokenizer);
    }
}

std::shared_ptr<ListValue> Reader::read_list(Tokenizer &tokenizer) {
//    Add the function read_list to reader.qx.
//    This function will repeatedly call read_form with the Reader object until it encounters a ')' token (if it reach EOF before reading a ')' then that is an error).
//    It accumulates the results into a List type. If your language does not have a sequential data type that can hold mal type values you may need to implement one (in types.qx).
//    Note that read_list repeatedly calls read_form rather than read_atom.
//    This mutually recursive definition between read_list and read_form is what allows lists to contain lists.
      // create a ListValue object
        std::shared_ptr<ListValue> list = std::make_shared<ListValue>();
        while (true) {
            // read the next token
            auto token = tokenizer.next_token();
            // check if we have reached the end of the list
            if (token.first == TOKEN_TYPE::RPAREN) {
                break;
            }
            // read the next form
            auto form = read_form(tokenizer);
            list->add_value(form);
            // add the form to the list
        }
        return list;
}

std::shared_ptr<Value> Reader::read_atom(Tokenizer &tokenizer) {
    if (tokenizer.peek_token().first == TOKEN_TYPE::INTEGER) {
        auto value = std::make_shared<IntegerValue>(std::stoi(tokenizer.peek_token().second));
        return value;
    } else if (tokenizer.peek_token().first == TOKEN_TYPE::IDENTIFIER) {
        // create an IdentifierValue object
        auto value = std::make_shared<SymbolValue>(tokenizer.peek_token().second);
        return value;
    } else if (tokenizer.peek_token().first == TOKEN_TYPE::STRING) {
        // create a StringValue object
        auto value = std::make_shared<StringValue>(tokenizer.peek_token().second);
        return value;
    } else {
        // ???
}
}
