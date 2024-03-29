#include "generic.hpp"
#include <Computer.hpp>
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <ostream>
#include <sstream>
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
	std::optional<u64> pos = {};

	BaseToken(): type(UNKNOWN){}
	BaseToken(const std::string& str, Type type): name(str), type(type){}
	BaseToken(const std::string& str): name(str), type(UNKNOWN){}
	virtual void parse(const std::string& str) {};
	virtual ~BaseToken() = default;

	virtual void print(std::ostream& os){
		os << this->name;
	}
	virtual u8 get_size(){ return 0; }
	virtual void write_byte(std::vector<u8>& v){
		// std::cout << "write_byte not implemented for: " << this->name << std::endl;
	}
};

std::ostream& operator<<(std::ostream& os, BaseToken* token){
	if (token->pos.has_value())
		os << "0x" << std::hex << token->pos.value() << std::dec << " ";
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


class SectionToken : public BaseToken{
public:
	std::vector<Token> sub_tokens;
	SectionToken(const std::string& str){
		let p_pos = str.find('.');
		if(p_pos == std::string::npos){
			throw std::runtime_error("LabelToken: invalid label");
		}
		std::string sub = str.substr(p_pos + 1);
		this->name = sub;
		this->type = BaseToken::SECTION;
	}

	void print(std::ostream& os) override {
		os << "." << this->name << '\n';
		for (auto& token : this->sub_tokens){
			os << token << '\n';
		}
	};

	void write_byte(std::vector<u8>& v) override {
		for(auto& token : this->sub_tokens){
			token->write_byte(v);
		}
	}
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

	u8 get_size() override {
		return this->name.size();
	}

	void write_byte(std::vector<u8>& v) override {
		for(auto& c : this->name)
			v.push_back(c);
	}
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
		try{
			this->value = std::stoi(str);
		}
		catch(std::exception& e){
			std::cout << "failed to convert number: " << str << std::endl;
		}
	}

	u8 get_size() override { return (value & 0xff) > 0 ? 2 : 1; }

	void print(std::ostream& os) override {
		os << this->value;
	};

	void write_byte(std::vector<u8>& v) override {
		v.push_back(this->value & 0xff);
		if((this->value & 0xff00) > 0)
			v.push_back((this->value & 0xff00) >> 8);
	}
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

	u8 get_size() override { return 1; }

	void write_byte(std::vector<u8>& v) override {
		v.push_back(this->value);
	}
};

class InstructionToken : public BaseToken{
public:
	std::vector<Token> arguments;
	Instruction* instruction;
	InstructionToken(const std::string& str){
		this->name = str;
		this->type = BaseToken::INSTRUCTION;
	}

	void print(std::ostream& os) override{
		os << this->name << "\033[m";
		for(auto& arg : this->arguments){
			os << " " << arg;
		}
	};

	u8 get_size() override {
		return this->instruction->get_size();
	}

	void write_byte(std::vector<u8>& v) override {
		v.push_back(this->instruction->get_code());
		auto& sig = this->instruction->get_signature();

		for (u64 i = 0; i < sig.params; i++){
			auto& arg = this->arguments[i];
			switch(arg->type){
				case BaseToken::REGISTER:
					v.push_back(std::static_pointer_cast<RegisterToken>(arg)->value);
					break;
				case BaseToken::NUMBER:
					v.push_back(std::static_pointer_cast<NumberToken>(arg)->value & 0xff);
					if((this->instruction->get_signature().args[i] & 0xf0) == Word)
						v.push_back((std::static_pointer_cast<NumberToken>(arg)->value & 0xff00) >> 8);
					break;
				case BaseToken::LABEL:{
					u16 value = std::static_pointer_cast<LabelToken>(arg)->pos.value();
					v.push_back(value & 0xff);
					v.push_back((value & 0xff00) >> 8);
				} break;
				default:
					std::cout << "unknown argument type: " << arg->type << std::endl;
			}
		}

	}
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

void report_no_match(std::vector<Token>& tokens, u64 index, InstructionFamily& family){
	std::stringstream ss;
	ss << "no match found for instruction: " << tokens[index];
	
	ss << " with arguments: ";

	for(u64 i = 0; i < family.instructions[0]->get_signature().params; i++){
		ss << tokens[i + index + 1] << " ";
	}


	throw std::runtime_error(ss.str());
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
	if(!matched){
		report_no_match(tokens, index, family);
	}

	auto* instruction = new InstructionToken(matched->get_signature().name);
	instruction->instruction = matched;
	u64  params = matched->get_signature().params;
	instruction->arguments.insert(instruction->arguments.end(), tokens.begin() + index + 1, tokens.begin() + index + 1 + params);
	tokens.erase(tokens.begin() + index + 1, tokens.begin() + index + 1 + params);

	tokens[index] = std::shared_ptr<BaseToken>(instruction);

}

void locate_labels(std::vector<Token>& tokens, std::vector<std::shared_ptr<LabelToken>>& labels){
	for(u64 i = 0; i < tokens.size() - 1; i++){
		if(tokens[i]->type != BaseToken::UNKNOWN)
			continue;
		if(tokens[i]->name.find(':') != std::string::npos){
			tokens[i] = std::make_shared<LabelToken>(tokens[i]->name);
			labels.push_back(std::static_pointer_cast<LabelToken>(tokens[i]));
		}
	}
}

void set_labels(std::vector<Token>& tokens, std::vector<std::shared_ptr<LabelToken>>& labels){
	for(u64 i = 0; i < tokens.size(); i++){
		if(tokens[i]->type != BaseToken::UNKNOWN)
			continue;
		for (auto& label : labels){
			if(tokens[i]->name == label->name){
				tokens[i] = label;
				break;
			}
		}
	}
}


void expand_sections(std::vector<Token>& tokens, Instructions& instructions, std::vector<std::shared_ptr<LabelToken>>& labels, std::vector<InstructionFamily>& instruction_names){
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

}

// match instructions and take arguments
void collapse_instructions(std::vector<Token>& tokens, std::vector<InstructionFamily>& instruction_names){
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
				if(sub.empty()){
					start = i + 1;
					continue;
				}
				final.push_back(std::make_shared<BaseToken>(sub, BaseToken::UNKNOWN));
				start = i + 1;
			}
		}
		end = str.size();
		std::string sub_end = str.substr(start, end - start);
		if (!sub_end.empty())
			final.push_back(std::make_shared<BaseToken>(sub_end, BaseToken::UNKNOWN));
	}

	return final;
}
 
std::vector<u8> assemble(const std::string& path, Instructions &instructions){

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

	std::vector<std::shared_ptr<SectionToken>> sections;
	for(u64 i = 0; i < tokens.size(); i++){
		auto& token = tokens[i];
		if(token->name == "section"){
			sections.push_back(std::make_shared<SectionToken>(tokens[i + 1]->name));
			i++;
		}
		else{
			sections.back()->sub_tokens.push_back(std::move(token));
		}
	}

	// print sections
	for(auto& section : sections){
		std::cout << "Printing section: " << section->name << std::endl;
		std::cout << section << std::endl;
	}

	std::vector<std::shared_ptr<LabelToken>> labels;
	for(auto& section : sections)
		locate_labels(section->sub_tokens, labels);

	for(auto& section : sections)
		set_labels(section->sub_tokens, labels);

	std::cout << "labels:" << std::endl;
	for(auto& label : labels)
		std::cout << label->name << std::endl;

	for(auto& section : sections)
		expand_sections(section->sub_tokens, instructions, labels, instruction_names);

	for(auto& section : sections)
		collapse_instructions(section->sub_tokens, instruction_names);


	u32 pos = 0;
	for(auto& section : sections){
		section->pos = pos;
		pos += section->get_size();
		for (auto& token : section->sub_tokens){
			token->pos = pos;
			pos += token->get_size();
		}
	}

	if(pos < 0x40){
		u32 pos2 = 0;
		for(auto& token: sections[1]->sub_tokens){
			token->pos = pos2;
			pos2 += token->get_size();
		}

	}

	std::vector<u8> binary;
	for (auto& section : sections){
		std::cout << section << std::endl;
		section->write_byte(binary);
	}

	return binary;
} 
