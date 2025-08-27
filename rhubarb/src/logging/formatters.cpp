#include "logging/formatters.h"
#include <format>
#include "logging/Entry.h"
#include "tools/tools.h"

using std::string;

namespace logging {

	string SimpleConsoleFormatter::format(const Entry& entry) {
		return std::format("[{}] {}", entry.level, entry.message);
	}

	string SimpleFileFormatter::format(const Entry& entry) {
		return std::format(
			"[{}] {} {}",
			formatTime(entry.timestamp, "%F %H:%M:%S"),
			entry.threadCounter,
			consoleFormatter.format(entry)
		);
	}

}
