#include "audio/audioFileReading.h"
#include <fmt/core.h>
#include "audio/WaveFileReader.h"
#include <ranges>
#include <algorithm>
#include <cctype>
#include "audio/OggVorbisFileReader.h"

using std::filesystem::path;
using std::string;
using std::runtime_error;

std::unique_ptr<AudioClip> createAudioFileClip(path filePath) {
	try {
		std::string extension = filePath.extension().string();
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
		if (extension == ".wav") {
			return std::make_unique<WaveFileReader>(filePath);
		}
		if (extension == ".ogg") {
			return std::make_unique<OggVorbisFileReader>(filePath);
		}
		throw runtime_error(fmt::format(
			"Unsupported file extension '{}'. Supported extensions are '.wav' and '.ogg'.",
			extension
		));
	} catch (...) {
		std::throw_with_nested(runtime_error(format("Could not open sound file {}.", filePath.string())));
	}
}
