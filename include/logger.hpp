#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <memory>
#include <string>
#include <unordered_map>

namespace wlog{

template<typename T>
inline std::string format_f(std::string fmt, const T& t){
	return std::to_string(t);
}

template<>
inline std::string format_f(std::string fmt, char* const& t){
	return t;
}


class Writter;

class Writter{
private:
	inline static void __write(Writter& writter, std::string format){
		writter.log(format);
	};
	
	template<typename T, typename... Args>
	inline static void __write(Writter& writter, std::string format, const T& t, Args... args){
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
	virtual void log(std::string message) = 0;
	virtual ~Writter() = default;

	template<typename FmtT, typename T, typename... Args>
	void write(FmtT format, const T& first, Args... args) {
		Writter::__write(*this, format, first, args...);
	}

	void write(std::string format) {
		Writter::__write(*this, format);
	}
};


class Write{
private:
	std::unordered_map<std::string, std::unique_ptr<Writter>> _writters;
public:
	void add(const std::string& name, std::unique_ptr<Writter> writter){
		_writters[name].swap(writter);
	}

	Writter& operator[](const std::string& name){
		return *_writters[name];
	}
};

class Logger : public Writter{
public:
	void log(const std::string message) override;
};

}

#endif
