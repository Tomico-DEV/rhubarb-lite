#include "logging/formatters.h"
#include <fmt/core.h>
#include "logging/Entry.h"
#include "tools/tools.h"

using std::string;

namespace logging {

	string SimpleConsoleFormatter::format(const Entry& entry) {
		return fmt::format("[{}] {}", entry.level, entry.message);
	}

	string SimpleFileFormatter::format(const Entry& entry) {
		return fmt::format(
			"[{0}] {1} {2}",
			formatTime(entry.timestamp, "%F %H:%M:%S"),
			entry.threadCounter,
			consoleFormatter.format(entry)
		);
	}

}
