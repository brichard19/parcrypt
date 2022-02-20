#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <string>
#include <sstream>
#include <functional>

namespace LogLevel {
	enum Level {
		Info = 1,
		Error = 2,
		Debug = 4,
        Warning = 8
	};
};

class LogObj {

private:
	std::function<void(int, const std::string&)> _log_function;
	std::ostringstream _string_stream;
	int _log_level;

public:
	explicit LogObj(int log_level, std::function<void(int, const std::string&)> log_function) : _log_level(log_level), _log_function(log_function) {}
	~LogObj() {_log_function(_log_level, _string_stream.str());}
	std::ostringstream& get_stream()
	{
		return _string_stream;
	}
};

class Logger {

private:
	static std::string _logFile;

	static std::string format_log(int logLevel, std::string msg);

	static std::string get_date_time();

public:

	Logger()
	{
	}

	static void log(int logLevel, std::string msg);

	static void set_log_file(std::string path);
};

#define LOG(log_level) LogObj(log_level, Logger::log).get_stream()

#endif