#include "generic.hpp"
#include <Computer.hpp>
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <ostream>
#include <unordered_map>
#include <vector>

static std::string clean_string(const std::string str){
	std::string buffer = str;

	// replace tabs with spaces for easier parsing
	for (auto& c : buffer)
		if(c == '\t')
			c = ' ';

	// cleanes leading whitespace and comments
	std::string cleaned;
	bool leading = true;
	for(auto c : buffer){
		if(leading && c == ' ')
			continue;
		if(c == ';')
			break;
		leading = false;
		cleaned.push_back(c);
	}

	// remove trailing whitespace
	while(cleaned.back() == ' ')
		cleaned.pop_back();


	// remove double spaces
	u64 pos = 0;
	while((pos = cleaned.find("  ", pos)) != std::string::npos)
		cleaned.replace(pos, 2, " ");

	return cleaned;
}

struct Token{
	enum Type{
		REGISTER,
		NUMBER,
		LABEL,
		INSTRUCTION,
		STRING,
		UNKNOWN
	} type;
	std::string value;

	static Token make_string(const std::string& str, bool is_string){
		Token token;
		token.type = is_string ? STRING : UNKNOWN;
		token.value = str;

		return token;
	}

	bool set_label_if(){
		if(value.empty() || type != UNKNOWN)
			return false;
		u64 pos = value.find(':');
		if(pos == std::string::npos)
			return false;
		this->type = LABEL;
		this->value = value.substr(0, pos);

		return true;
	}

	bool make_label_if(std::vector<Token*>& tokens){
		if(value.empty() || type != UNKNOWN)
			return false;
		for(auto& token : tokens){
			if(token->value == value && token->type == LABEL){
				this->type = LABEL;
				return true;
			}
		}
		return false;
	}
};

std::ostream& operator<<(std::ostream& os, const Token& token){
	// set color based on type
	switch(token.type){
		case Token::REGISTER:
			os << "\033[1;32m";
			break;
		case Token::NUMBER:
			os << "\033[1;34m";
			break;
		case Token::LABEL:
			os << "\033[1;35m";
			break;
		case Token::INSTRUCTION:
			os << "\033[1;36m";
			break;
		case Token::STRING:
			os << "\033[1;33m";
			break;
		case Token::UNKNOWN:
			os << "\033[0;37m";
			break;
		default:
			os << "\033[0;37m";
			break;
	}
	std::cout << (token.type == Token::STRING ? "\"" : "");
	os << token.value;
	std::cout << (token.type == Token::STRING ? "\"" : "");
	os << "\033[0m";
	return os;
}

std::vector<Token> tokenize(const std::string& str){
	std::vector<Token> tokens;

	// first it splits string into quotes and non-quotes
	bool inside_quotes = false;
	u64 start = 0;
	u64 end = 0;
	for(size_t i = 1; i < str.size(); i++){
		const char& c = str[i];
		if(c == '"'){
			end = i;
			tokens.push_back(Token::make_string(str.substr(start, end - start), inside_quotes));
			inside_quotes = !inside_quotes;
			start = i + 1;
		}
	}
	end = str.size();
	std::string sub_end = str.substr(start, end - start);
	if (!sub_end.empty())
		tokens.push_back(Token::make_string(str.substr(start, end - start), inside_quotes));

	std::vector<Token> final;
	// then it splits the non-quotes into words
	for(size_t i = 0; i < tokens.size(); i++){
		if(tokens[i].type != Token::UNKNOWN){
			final.push_back(tokens[i]);
			continue;
		}

		const std::string& str = tokens[i].value;
		std::string buffer = "";
		u64 start = 0;
		u64 end = 0;
		for(auto c : str){
			if(c == ' '){
				if(!buffer.empty()){
					final.push_back(Token{Token::UNKNOWN, buffer});
					buffer.clear();
				}
				continue;
			}
			buffer.push_back(c);
		}
		if(!buffer.empty())
			final.push_back(Token{Token::UNKNOWN, buffer});
	}

	for(auto& token : final)
		token.set_label_if();

	return final;
}

std::vector<u8> assemble(const std::string& path, Instructions &instructions){
	std::vector<u8> binary;

	std::ifstream file(path);

	// Read file, and clean lines
	std::vector<std::string> lines;
	std::string line;
	while(std::getline(file, line)){
		if (line.empty())
			continue;
		std::string cleaned = clean_string(line);
		lines.push_back(cleaned);
	}

	// Tokenize lines
	std::vector<std::vector<Token>> tokens;
	for(auto& line : lines)
		tokens.push_back(tokenize(line));

	// Find labels
	std::vector<Token*> labels= {};
	for(auto& token_line : tokens){
		for(auto& token : token_line){
			if(token.type == Token::LABEL)
				labels.push_back(&token);
		}
	}

	

	for(auto& token_line : tokens){
		for(auto& token : token_line)
			std::cout << token << " ";
		std::cout << std::endl;
	}




	return binary;
}
