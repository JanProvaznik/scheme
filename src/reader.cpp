#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <utility>
#include "reader.h"


Tokenizer::Tokenizer(const std::string &source) : source(source), pos(0) {
  this->next_token();
}

// this could theoretically be done with regex, but it seems very slow and unreliable
std::pair<TOKEN_TYPE, std::string> Tokenizer::next_token() {
  // skip whitespace
  while (pos < source.size() && std::isspace(source[pos])) {
    pos++;
  }
  if (pos == source.size()) {
    auto pair = std::make_pair(TOKEN_TYPE::EOF_TOKEN, "");
    this->current_token = pair;
    return pair;

  }
  if (source[pos] == '(') {
    pos++;
    auto pair = std::make_pair(TOKEN_TYPE::LPAREN, "(");
    this->current_token = pair;
    return pair;
  }
  if (source[pos] == ')') {
    pos++;
    auto pair = std::make_pair(TOKEN_TYPE::RPAREN, ")");
    this->current_token = pair;
    return pair;
  }
  if (source[pos] == '\'') {
    pos++;
    auto pair = std::make_pair(TOKEN_TYPE::QUOTE, "'");
    this->current_token = pair;
    return pair;
  }
  // TODO quasiquote
  if (source[pos] == '#') {
    pos++;
    if (pos < source.size() && source[pos] == 't') {
      pos++;
      auto pair = std::make_pair(TOKEN_TYPE::TRUE, "#t");
      this->current_token = pair;
      return pair;
    }
    if (pos < source.size() && source[pos] == 'f') {
      pos++;
      auto pair = std::make_pair(TOKEN_TYPE::FALSE, "#f");
      this->current_token = pair;
      return pair;
    }
    throw std::runtime_error("invalid boolean literal");
  }
  // FIXME: isn't hash also used for vectors??
  // comment
  if (source[pos] == ';') {
    // find the end of the comment
    size_t end = source.find('\n', pos + 1);
    if (end == std::string::npos) {
      // comment goes to the end of the file
      pos = source.size();
    } else {
      // skip the comment
      pos = end + 1;
    }
    return next_token();
  }

  // check if we have a string
  if (source[pos] == '"') {
    // find the end of the string
    size_t end = source.find('"', pos + 1);
    if (end == std::string::npos) {
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
  // TODO: consider handling weird representations such as hex

  if (std::isdigit(source[pos]) || source[pos] == '-') {
    // find the end of the number
    size_t end = pos;
    if (source[pos] == '-') {++end;}
    while (end < source.size() && std::isdigit(source[end])) {
      end++;
    }
    // float
    if (end < source.size() && source[end] == '.') {
      ++end;
      while (end < source.size() && std::isdigit(source[end])) {
	++end;
      }
      std::string token = source.substr(pos, end - pos);
      // check that it's really a float and not some kind of obscure legal symbol like -1..1
      size_t i = 0;
      bool saw_dot = false;
      for (auto c : token) {
	if (i == 0 && c == '-') {
	  ++i;
	  continue;
	}
	if (c == '.') {
	  if (saw_dot) {
	    goto not_number;
	  }
	  saw_dot = true;
	  continue;
	}
	if (!std::isdigit(c)) {
	  goto not_number;
	}
	++i;
      }
      pos = end;
      auto pair = std::make_pair(TOKEN_TYPE::FLOAT, token);
      this->current_token = pair;
      return pair;
    }
    // integer
    std::string token = source.substr(pos, end - pos);
    // check that it's really an integer and not some kind of obscure symbol like -1-1
    if (token == "-") {
      goto not_number;
    }
    size_t i = 0;
    for (auto c : token) {
      if (i == 0 && c == '-') {
	++i;
	continue;
      }
      if (!std::isdigit(c)) {
	goto not_number;
      }
      ++i;
    }

    pos = end;
    auto pair = std::make_pair(TOKEN_TYPE::INTEGER, token);
    this->current_token = pair;
    return pair;
  }
not_number:
  // check if we have an identifier symbol, actually we don't have to consider numbers because are not mentioned in the spec
  if (is_scheme_alpha(source[pos])) {
    // find the end of the identifier
    size_t end = pos;
    while (end < source.size() && is_scheme_alpha(source[end])) {
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


bool Tokenizer::is_scheme_alpha(unsigned char c) {
  // check if c is a scheme extended alpha character that can be used as an identifier
  // scheme symbols are any of the following characters: ! $ % & * + - . / : < = > ? @ ^ _ ~
  // or any alphanumeric character, //TODO: this is a bit ambiguous from the spec, I could also exclude numbers but that would be weird
  if (std::isalnum(c)) {
    return true;
  }
  if (c == '!' || c == '$' || c == '%' || c == '&' || c == '*' || c == '+' || c == '-' || c == '.' || c == '/' ||
      c == ':' || c == '<' || c == '=' || c == '>' || c == '?' || c == '@' || c == '^' || c == '_' || c == '~') {
    return true;
  }
  return false;
}

std::vector<std::pair<TOKEN_TYPE, std::string> > Tokenizer::tokenize() {
  std::vector<std::pair<TOKEN_TYPE, std::string> > tokens;
  while (pos < source.size()) {
    tokens.push_back(next_token());
  }

  return tokens;
}

std::pair<TOKEN_TYPE, std::string> Tokenizer::peek_token() {
  return current_token;
}


std::shared_ptr<Value> Reader::read_form(Tokenizer &tokenizer) {

  auto top_token = tokenizer.peek_token();
  if (top_token.first == TOKEN_TYPE::LPAREN) {
    return read_list(tokenizer);
  } else {
    return read_atom(tokenizer);
  }
}

std::shared_ptr<ListValue> Reader::read_list(Tokenizer &tokenizer) {
  // create a List object
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

  } else if (tokenizer.peek_token().first == TOKEN_TYPE::FLOAT) {
    auto value = std::make_shared<FloatValue>(std::stof(tokenizer.peek_token().second));
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
    // ??? TODO:
  }
}
