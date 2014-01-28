#ifndef _LOG_H_
#define _LOG_H_

#ifndef ANDROID
#include "../concurrent/ThreadPool.h"
#endif

#include <string>
#include <boost/chrono.hpp>
#include <boost/format.hpp>

#include <sstream>

namespace rocket { namespace util {
	enum class LogLevel {
		DEBUG,
		WARNING,
		ERROR
	};

	class Logger {
	public:
		~Logger() = default;
		void log(LogLevel const& level, std::string const& tag, std::string const& message);
		void log(LogLevel const& level, std::string const& message);

		void flush();
		void setDefaultTag(std::string const& defaultTag);

		static Logger& getLogger();
	private:
		class LoggerTask {
		public:
			LoggerTask(LogLevel const& level, std::string const& tag, std::string const& message);
			void operator()();
		private:
			LogLevel level;
			std::string tag;
			std::string message;
#ifndef ANDROID
			boost::chrono::system_clock::time_point timePoint;
#endif
		};

		Logger();
		Logger(Logger const& logger) = delete;
		Logger& operator=(Logger const& logger) = delete;

#ifndef ANDROID
		rocket::concurrent::ThreadPool messagePool;
#endif
		std::string defaultTag;
	};

	inline void Logger::setDefaultTag(std::string const& defaultTag) {
		this->defaultTag = defaultTag;
	}

	inline void logd(std::string const& tag, std::string const& message) {
		Logger::getLogger().log(LogLevel::DEBUG, tag, message);
	}

	/*
	inline void logd(std::string const& tag, boost::format const& format) {
		logd(tag, format.str());
	}
	*/

	inline void logw(std::string const& tag, std::string const& message) {
		Logger::getLogger().log(LogLevel::WARNING, tag, message);
	}

	/*
	inline void logw(std::string const& tag, boost::format const& format) {
		logw(tag, format.str());
	}
	*/

	inline void loge(std::string const& tag, std::string const& message) {
		Logger::getLogger().log(LogLevel::ERROR, tag, message);
	}

	/*
	inline void loge(std::string const& tag, boost::format const& format) {
		loge(tag, format.str());
	}
	*/

	inline void logd(std::string const& message) {
		Logger::getLogger().log(LogLevel::DEBUG, message);
	}

	/*
	inline void logd(boost::format const& format) {
		logd(format.str());
	}
	*/

	inline void logw(std::string const& message) {
		Logger::getLogger().log(LogLevel::WARNING, message);
	}

	/*
	inline void logw(boost::format const& format) {
		logw(format.str());
	}
	*/

	inline void loge(std::string const& message) {
		Logger::getLogger().log(LogLevel::ERROR, message);
	}

	/*
	inline void loge(boost::format const& format) {
		loge(format.str());
	}
	*/
}}

// And finally some horrible horrible macros...
#ifndef LOG_DISABLED
	#define LOGD(MESSAGE) { std::ostringstream oss; oss << MESSAGE; rocket::util::logd(oss.str()); }
	#define LOGW(MESSAGE) { std::ostringstream oss; oss << MESSAGE; rocket::util::logw(oss.str()); }
	#define LOGE(MESSAGE) { std::ostringstream oss; oss << MESSAGE; rocket::util::loge(oss.str()); }
	#define LOGDT(TAG, MESSAGE) { std::ostringstream oss; oss << MESSAGE; rocket::util::logd((TAG), oss.str()); }
	#define LOGWT(TAG, MESSAGE) { std::ostringstream oss; oss << MESSAGE; rocket::util::logw((TAG), oss.str()); }
	#define LOGET(TAG, MESSAGE) { std::ostringstream oss; oss << MESSAGE; rocket::util::loge((TAG), oss.str()); }
	
	// #define LOGD(MESSAGE) rocket::util::logd((MESSAGE))
	// #define LOGW(MESSAGE) rocket::util::logw((MESSAGE))
	// #define LOGE(MESSAGE) rocket::util::loge((MESSAGE))
	// #define LOGTD(TAG, MESSAGE) rocket::util::logd((TAG), (MESSAGE))
	// #define LOGTW(TAG, MESSAGE) rocket::util::logw((TAG), (MESSAGE))
	// #define LOGTE(TAG, MESSAGE) rocket::util::loge((TAG), (MESSAGE))
#else
	#define LOGD(MESSAGE)
	#define LOGW(MESSAGE)
	#define LOGE(MESSAGE)
	#define LOGDT(TAG, MESSAGE)
	#define LOGWT(TAG, MESSAGE)
	#define LOGET(TAG, MESSAGE)
#endif // Horrible horrible macros

#endif // _LOGGER_H_
