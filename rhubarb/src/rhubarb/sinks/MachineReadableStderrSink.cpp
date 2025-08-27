#include "rhubarb/sinks/MachineReadableStderrSink.h"
#include "rhubarb/semanticEntries.h"
#include "tools/stringTools.h"
#include <iostream>
#include <optional>

using std::string;
using logging::Level;

MachineReadableStderrSink::MachineReadableStderrSink(Level minLevel) :
	minLevel(minLevel)
{
}

string formatLogProperty(const logging::Entry& entry) {
	return std::format(
		R"("log": {{ "level": "{}", "message": "{}" }})",
		entry.level,
		escapeJsonString(entry.message)
	);
}

void MachineReadableStderrSink::receive(const logging::Entry& entry) {
	std::optional<string> line;
	if (dynamic_cast<const SemanticEntry*>(&entry)) {
		if (const auto* startEntry = dynamic_cast<const StartEntry*>(&entry)) {
			const string file = escapeJsonString(startEntry->getInputFilePath().string());
			line = std::format(
				R"({{ "type": "start", "file": "{}", {} }})",
				file,
				formatLogProperty(entry)
			);
		}
		else if (const auto* progressEntry = dynamic_cast<const ProgressEntry*>(&entry)) {
			const int progressPercent = static_cast<int>(progressEntry->getProgress() * 100);
			if (progressPercent > lastProgressPercent) {
				line = std::format(
					R"({{ "type": "progress", "value": {:.2f}, {} }})",
					progressEntry->getProgress(),
					formatLogProperty(entry)
				);
				lastProgressPercent = progressPercent;
			}
		}
		else if (dynamic_cast<const SuccessEntry*>(&entry)) {
			line = std::format(R"({{ "type": "success", {} }})", formatLogProperty(entry));
		}
		else if (const auto* failureEntry = dynamic_cast<const FailureEntry*>(&entry)) {
			const string reason = escapeJsonString(failureEntry->getReason());
			line = std::format(
				R"({{ "type": "failure", "reason": "{}", {} }})",
				reason,
				formatLogProperty(entry)
			);
		}
		else {
			throw std::runtime_error("Unsupported type of semantic entry.");
		}
	}
	else {
		if (entry.level >= minLevel) {
			line = std::format(R"({{ "type": "log", {} }})", formatLogProperty(entry));
		}
	}

	if (line) {
		std::cerr << *line << std::endl;
		// Make sure the stream is flushed so that applications listening to it get the line immediately
		fflush(stderr);
	}
}
