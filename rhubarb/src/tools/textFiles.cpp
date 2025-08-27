#include "tools/textFiles.h"
#include <fmt/core.h>
#include <fstream>
#include "tools/stringTools.h"

using std::string;
using std::filesystem::path;

string readUtf8File(path filePath) {
	if (!exists(filePath)) {
		throw std::invalid_argument(fmt::format("File {} does not exist.", filePath.string()));
	}
	try {
		std::ifstream file;
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		file.open(filePath);
		string text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		if (!isValidUtf8(text)) {
			throw std::runtime_error("File encoding is not ASCII or UTF-8.");
		}

		return text;
	} catch (...) {
		std::throw_with_nested(std::runtime_error(fmt::format("Error reading file {0}.", filePath.string())));
	}
}

