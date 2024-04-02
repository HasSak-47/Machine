#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

namespace wlog{

template<typename T>
inline std::string format_f(std::string fmt, const T& t){
	std::stringstream ss;
	ss << t;
	return ss.str();
}

template<>
inline std::string format_f(std::string fmt, char* const& t){
	return t;
}

class Writer;

class Writer{
private:
	inline static void __write(Writer& writter, std::string format){
		writter.log(format);
	};
	
	template<typename T, typename... Args>
	inline static void __write(Writer& writter, std::string format, const T& t, Args... args){
		auto beg = format.find("{");
		auto end = format.find("}");
		if(beg == std::string::npos){
			writter.log(format.c_str());
			return;
		}
		auto sub = format.substr(beg, end - beg);
		format.replace(beg, end - beg, wlog::format_f(sub, t));
		__write(writter, format, args...);
	}
public:
	virtual ~Writer() = default;
	virtual void log(std::string message) = 0;

	template<typename FmtT, typename T, typename... Args>
	void write(FmtT format, const T& first, Args... args) {
		Writer::__write(*this, format, first, args...);
	}

	void write(std::string format) {
		Writer::__write(*this, format);
	}
};


class Write{
private:
	std::unordered_map<std::string, std::unique_ptr<Writer>> _writters;
public:
	void add(const std::string& name, std::unique_ptr<Writer> writter){
		_writters[name].swap(writter);
	}

	Writer& operator[](const std::string& name){
		return *_writters[name];
	}
};

class Stdout: public Writer{
public:
	void log(const std::string message) override;
	Stdout() {};
	~Stdout() override {};
};

class Debug: public Writer{
private:
	bool debug = true;
public:
	void set_debug(){ debug = true; }
	void unset_debug(){ debug = false; }

	void log(const std::string message) override;

	Debug() {};
	~Debug() override {};
};

extern Write write;

}

#endif
