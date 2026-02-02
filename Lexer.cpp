#include "Lexer.hpp"

Lexer::Lexer(const std::string& src) : _src(src), _pos(0), _line(1) {}
Lexer::~Lexer() {}

// char Lexer::peek() const {
// 	if (_pos >= _src.length())
// 		return '\0';
// 	return _src[_pos];
// }

bool Lexer::isSpace(char c) const {
	return ((c >= 9 && c <= 13) || c == 32);
}

bool Lexer::isSpecial(char c) const {
	return (c == '{' || c == '}' || c == ';');
}

void Lexer::skipWhitespace() {
	while (_pos < _src.length())
	{
		char c = _src[_pos];
		if (c == '\n') {
			_line++;
			_pos++;
		}
		else if (isSpace(c)) {
			_pos++;
		}
		else if (c == '#') {
			while (_pos < _src.length() && _src[_pos] != '\n') {
				_pos++;
			}
		}
		else {
			break;
		}
	}
}

Token Lexer::readQuotedString(char quote) {
	_pos++;
	size_t start = _pos;
	while (_pos < _src.length() && _src[_pos] != quote) {
		if (_src[_pos] == '\n') 
			_line++;
		_pos++;
	}

	std::string value = _src.substr(start, _pos - start);
	if (_pos < _src.length()) {
		_pos++;
	} else {
		std::cerr << "Error: Unclosed quote at line --> " << _line << std::endl;
		// hna wa9ila fin n9dro nkhdmo exception ida avancena flproject
	}
	return Token(T_WORD, value, _line);
}

Token Lexer::readWord() {
	size_t start = _pos;

	while(_pos < _src.length()) {
		char c = _src[_pos];
		if (isSpace(c) || isSpecial(c) || c == '#') {
			break;
		}
		_pos++;
	}
	std::string value = _src.substr(start, _pos - start);
	return Token(T_WORD, value, _line);
}

std::vector<Token> Lexer::tokenize() {
	std::vector<Token> tokens;

	while (_pos < _src.length()) {
		skipWhitespace();
		if (_pos >= _src.length())
			break;
		char c = _src[_pos];
		if (c == '{') {
			tokens.push_back(Token(T_LBRACE, "{", _line));
			_pos++;
		}
		else if (c == '}') {
			tokens.push_back(Token(T_RBRACE, "}", _line));
			_pos++;
		}
		else if (c == ';') {
			tokens.push_back(Token(T_SEMICOLON, ";", _line));
			_pos++;
		}
		else if (c == '"' || c == '\'') {
			tokens.push_back(readQuotedString(c));
		}
		else {
			tokens.push_back(readWord());
		}
	}
	tokens.push_back(Token(T_EOF, "", _line));
	return tokens;
}

