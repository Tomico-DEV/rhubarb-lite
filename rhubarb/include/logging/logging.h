#pragma once

#include "tools/EnumConverter.h"
#include "Sink.h"
#include "Level.h"
#include <fmt/core.h>
#include <sstream>

#include <memory>

namespace logging {

	bool addSink(std::shared_ptr<Sink> sink);

	bool removeSink(std::shared_ptr<Sink> sink);

	void log(const Entry& entry);

	void log(Level level, const std::string& message);

	template<typename... Args>
	void logFormat(Level level, std::string_view fmt_str, const Args&... args) {
		log(level, fmt::vformat(fmt_str,  fmt::make_format_args(args...)));
	}

#define LOG_WITH_LEVEL(levelName, levelEnum) \
	inline void levelName(const std::string& message) { \
		log(Level::levelEnum, message); \
	} \
	template <typename... Args> \
	void levelName ## Format(std::string_view format, const Args&... args) { \
		logFormat(Level::levelEnum, format, args...); \
	}

	LOG_WITH_LEVEL(trace, Trace)
	LOG_WITH_LEVEL(debug, Debug)
	LOG_WITH_LEVEL(info, Info)
	LOG_WITH_LEVEL(warn, Warn)
	LOG_WITH_LEVEL(error, Error)
	LOG_WITH_LEVEL(fatal, Fatal)
}
