#include "generic.hpp"
#include <Computer.hpp>
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <ostream>
#include <unordered_map>
#include <vector>

const char* register_names[] = {
	"r0", "r1", "r2", "r3",
	"r4", "r5", "r6", "r7",
	"r8", "r9", "ra", "rb",
	"rc", "rd", "re", "rf",
};


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

class BaseToken;
using Token = std::shared_ptr<BaseToken>;

class BaseToken{
public:
	enum Type{
		REGISTER,
		NUMBER,
		LABEL,
		INSTRUCTION,
		STRING,
		SECTION,
		UNKNOWN,
	} type;
	std::string name;
	std::vector<Token> sub_tokens;

	BaseToken(): type(UNKNOWN){}
	BaseToken(const std::string& str, Type type): name(str), type(type){}
	BaseToken(const std::string& str): name(str), type(UNKNOWN){}
	virtual void parse(const std::string& str) {};
	virtual ~BaseToken() = default;

	virtual void print(std::ostream& os){
	}
};

std::ostream& operator<<(std::ostream& os, BaseToken* token){
	switch(token->type){
		case BaseToken::REGISTER:
			os << "\033[1;32m";
			break;
		case BaseToken::NUMBER:
			os << "\033[1;34m";
			break;
		case BaseToken::INSTRUCTION:
			os << "\033[1;36m";
			break;
		case BaseToken::LABEL:
			os << "\033[1;35m";
			break;
		case BaseToken::STRING:
			os << "\033[1;33m";
			break;
		case BaseToken::SECTION:
			os << "\033[1;31m";
			break;
		case BaseToken::UNKNOWN:
			break;
	}
	token->print(os);
	os<< "\033[0m";
	return os;
}


class SectionIdentifier : public BaseToken{
public:
	SectionIdentifier(const std::string& str){
		let p_pos = str.find('.');
		if(p_pos == std::string::npos){
			throw std::runtime_error("LabelToken: invalid label");
		}
		std::string sub = str.substr(p_pos + 1);
		this->name = sub;
		this->type = BaseToken::LABEL;
	}

	void print(std::ostream& os) override {
		os << "." << this->name;
	};
};

class StringToken : public BaseToken{
public:
	StringToken(const std::string& str){
		this->name = str;
		this->type = BaseToken::STRING;
	}

	void print(std::ostream& os) override {
		os << "\"" << this->name << "\"";
	};
};

class LabelToken : public BaseToken{
public:
	LabelToken(const std::string& str){
		let c_pos = str.find(':');
		if(c_pos == std::string::npos){
			throw std::runtime_error("LabelToken: invalid label");
		}
		std::string sub = str.substr(0, c_pos);
		this->name = sub;
		this->type = BaseToken::LABEL;
	}

	void print(std::ostream& os) override {
		os << this->name << ":";
	};
};

class NumberToken : public BaseToken{
public:
	u16 value;
	NumberToken(const std::string& str){
		this->name = str;
		this->type = BaseToken::NUMBER;
	}

	void print(std::ostream& os) override {
		os << this->value;
	};
};

class RegisterToken : public BaseToken{
public:
	u8 value;
	RegisterToken(const std::string& str){
		this->name = str;
		this->type = BaseToken::REGISTER;
	}

	void print(std::ostream& os) override {
		os << register_names[this->value];
	};
};

class InstructionToken : public BaseToken{
public:
	std::vector<Token> arguments;
	InstructionToken(const std::string& str){
		this->name = str;
		this->type = BaseToken::INSTRUCTION;
	}

	void print(std::ostream& os) override{
		os << this->name;
	};
};


// should probably be a map
struct InstructionFamily{
	std::string name;
	std::vector<std::shared_ptr<Instruction>> instructions;
};

InstructionParamType param_t_from_bt_type(BaseToken::Type type){
	using IPT = InstructionParamType;
	switch(type){
		case BaseToken::REGISTER:
			return (IPT)(IPT::Reg | IPT::Byte);
		case BaseToken::NUMBER:
			return (IPT)(IPT::Val | IPT::Byte);
		case BaseToken::LABEL:
			return (IPT)(IPT::Mem | IPT::Word);
		default:
			return IPT::ERROR;
	}
}

void match_signature(std::vector<Token>& tokens, InstructionFamily& family, u64 index){
	Instruction* matched = nullptr;
	for(auto& instruction : family.instructions){
		u64 search_ahead = instruction->get_signature().params;
		bool ok = true;
		for(u64 i = 0; i < search_ahead; ++i){
			auto& token = tokens[i + index + 1];
			auto param = instruction->get_signature().args[i];
			InstructionParamType token_type = param_t_from_bt_type(token->type);
			if (token_type == InstructionParamType::ERROR || token_type != param){
				ok = false;
				break;
			}
		}
		if(ok){
			matched = instruction.get();
			break;
		}
	}
	if(!matched)
		throw std::runtime_error("no match found for instruction");

	auto* instruction = new InstructionToken(matched->get_signature().name);
	u64  params = matched->get_signature().params;
	instruction->arguments.insert(instruction->arguments.end(), tokens.begin() + index + 1, tokens.begin() + index + 1 + params);

	tokens[index] = std::shared_ptr<BaseToken>(instruction);

}


std::vector<Token> expand_sections(std::vector<Token>& tokens, Instructions& instructions){
	std::vector<Token> sections;
	std::vector<Token> labels;
	// expand labels
	for(u64 i = 0; i < tokens.size() - 1; i++){
		if(tokens[i]->type != BaseToken::UNKNOWN)
			continue;
		if(tokens[i]->name.find(':') != std::string::npos){
			tokens[i] = std::make_shared<LabelToken>(tokens[i]->name);
			labels.push_back(tokens[i]);
		}
	}

	for(u64 i = 0; i < tokens.size(); i++){
		if(tokens[i]->type != BaseToken::UNKNOWN)
			continue;

		for(auto& label : labels){
			if(tokens[i]->name == label->name){
				tokens[i] = label;
				break;
			}
		}
	}

	// get instruction names
	std::vector<InstructionFamily> instruction_names;
	for(auto& instruction : instructions){
		auto it = std::find_if(instruction_names.begin(), instruction_names.end(), [&](InstructionFamily& family){
			return family.name == instruction->get_signature().name;
		});
		if(it == instruction_names.end())
			instruction_names.push_back({instruction->get_signature().name, {instruction}});
		else
			it->instructions.push_back(instruction);
	}

	// mark instructions
	for(u64 i = 0; i < tokens.size(); i++){
		if(tokens[i]->type != BaseToken::UNKNOWN)
			continue;

		auto it = std::find_if(instruction_names.begin(), instruction_names.end(), [&](InstructionFamily& family){
			return family.name == tokens[i]->name;
		});
		if(it != instruction_names.end()){
			tokens[i] = std::make_shared<InstructionToken>(tokens[i]->name);
		}
	}

	// expand numbers and registers
	for(u64 i = 0; i < tokens.size(); i++){
		if(tokens[i]->type != BaseToken::UNKNOWN)
			continue;

		bool reg = tokens[i]->name[0] == 'r';
		bool num = tokens[i]->name[0] == '0' && tokens[i]->name[1] == 'x';
		auto start = tokens[i]->name.begin() + reg * 1 + num * 2;
		bool is_number = std::all_of(start, tokens[i]->name.end(), isxdigit);
		if(is_number && !reg)
			tokens[i] = std::make_shared<NumberToken>(tokens[i]->name);
		else
		if(is_number && reg)
			tokens[i] = std::make_shared<RegisterToken>(tokens[i]->name);
	}


	// match instructions and take arguments
	for(u64 i = 0; i < tokens.size(); i++){
		if(tokens[i]->type != BaseToken::INSTRUCTION)
			continue;

		auto& instruction = tokens[i];
		auto family = std::find_if(instruction_names.begin(), instruction_names.end(), [&](InstructionFamily& family){
			return family.name == instruction->name;
		});
		if (family == instruction_names.end())
			continue;
		// match instruction
		match_signature(tokens, *family, i);
	}

	// print tokens
	for(Token& token : tokens){
		std::cout << token << std::endl;
	}

	return sections;
}

/* 
 * Tokenizes a line into a vector of tokens
 * 
 * @param str the line to tokenize
 * @return a vector of tokens
 */
std::vector<Token> expand_strings(const std::string& str){
	std::vector<Token> tokens;

	// first it splits string into quotes and non-quotes
	bool inside_quotes = false;
	u64 start = 0;
	u64 end = 0;
	for(size_t i = 0; i < str.size(); i++){
		const char& c = str[i];
		if(c == '"'){
			end = i;
			std::string sub = str.substr(start, end - start);
			if(inside_quotes)
				tokens.push_back(std::make_shared<StringToken>(sub));
			else
				tokens.push_back(std::make_shared<BaseToken>(sub));
			inside_quotes = !inside_quotes;
			start = i + 1;
		}
	}
	end = str.size();
	std::string sub_end = str.substr(start, end - start);
	if (!sub_end.empty()){
		if (inside_quotes)
			tokens.push_back(std::make_unique<StringToken>(sub_end));
		else
			tokens.push_back(std::make_unique<BaseToken>(sub_end, BaseToken::UNKNOWN));
	}

	// then it splits the non-quotes into lines
	std::vector<Token> final;
	for(size_t i = 0; i < tokens.size(); i++){
		Token& token = tokens[i];
		if(token->type == BaseToken::STRING){
			final.push_back(token);
			continue;
		}

		std::string& str = token->name;
		u64 start = 0;
		u64 end = 0;
		for(size_t i = 0; i < str.size(); i++){
			const char& c = str[i];
			if(c == ' '){
				end = i;
				std::string sub = str.substr(start, end - start);
				final.push_back(std::make_unique<BaseToken>(sub, BaseToken::UNKNOWN));
				start = i + 1;
			}
		}
		end = str.size();
		std::string sub_end = str.substr(start, end - start);
		if (!sub_end.empty())
			final.push_back(std::make_unique<BaseToken>(sub_end, BaseToken::UNKNOWN));
	}

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

	// divides tokens into sections
	std::vector<Token> tokens;
	for(auto& line : lines){
		std::vector<Token> expanded = expand_strings(line);
		tokens.insert(tokens.end(), expanded.begin(), expanded.end());
	}

	// Parse tokens
	u64 start = 0;
	u64 end = 0;

	std::vector<Token> sections;
	for(u64 i = 0; i < tokens.size() - 1; i++){
		auto& token = tokens[i];
		if(token->name == "section"){
			std::cout << "section found" << std::endl;
			sections.push_back(std::make_unique<SectionIdentifier>(tokens[i + 1]->name));
			i++;
		}
		else{
			sections.back()->sub_tokens.push_back(std::move(token));
		}
	}

	for(auto& section : sections){
		expand_sections(section->sub_tokens, instructions);
	}


	return binary;
}
