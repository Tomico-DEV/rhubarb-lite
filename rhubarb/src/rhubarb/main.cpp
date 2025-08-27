#include <iostream>
#include <format>
#include <CLI/CLI.hpp>
#include "core/appInfo.h"
#include "logging/logging.h"
#include "logging/sinks.h"
#include "logging/formatters.h"
#include <gsl_util.h>
#include "exporters/Exporter.h"
#include "time/ContinuousTimeline.h"
#include "tools/stringTools.h"
#include <fstream>
#include "tools/parallel.h"
#include "tools/exceptions.h"
#include "tools/textFiles.h"
#include "lib/rhubarbLib.h"
#include "ExportFormat.h"
#include "exporters/DatExporter.h"
#include "exporters/TsvExporter.h"
#include "exporters/XmlExporter.h"
#include "exporters/JsonExporter.h"
#include "animation/targetShapeSet.h"
#include "tools/platformTools.h"
#include "sinks/MachineReadableStderrSink.h"
#include "sinks/NiceStderrSink.h"
#include "sinks/QuietStderrSink.h"
#include <sstream>
#include <ranges>
#include "rhubarb/semanticEntries.h"
#include "rhubarb/RecognizerType.h"
#include "recognition/PocketSphinxRecognizer.h"
#include "recognition/PhoneticRecognizer.h"

using std::exception;
using std::string;
using std::string;
using std::vector;
using std::unique_ptr;
using std::make_unique;
using std::shared_ptr;
using std::make_shared;
using std::filesystem::path;
using std::filesystem::u8path;
using std::views::transform;
using std::optional;

shared_ptr<logging::Sink> createFileSink(const path& path, logging::Level minLevel) {
	auto file = make_shared<std::ofstream>();
	file->exceptions(std::ifstream::failbit | std::ifstream::badbit);
	file->open(path);
	auto FileSink =
		make_shared<logging::StreamSink>(file, make_shared<logging::SimpleFileFormatter>());
	return make_shared<logging::LevelFilter>(FileSink, minLevel);
}

unique_ptr<Recognizer> createRecognizer(RecognizerType recognizerType) {
	switch (recognizerType) {
		case RecognizerType::PocketSphinx:
			return make_unique<PocketSphinxRecognizer>();
		case RecognizerType::Phonetic:
			return make_unique<PhoneticRecognizer>();
		default:
			throw std::runtime_error("Unknown recognizer.");
	}
}

unique_ptr<Exporter> createExporter(
	ExportFormat exportFormat,
	const ShapeSet& targetShapeSet,
	double datFrameRate,
	bool datUsePrestonBlair
) {
	switch (exportFormat) {
		case ExportFormat::Dat:
			return make_unique<DatExporter>(targetShapeSet, datFrameRate, datUsePrestonBlair);
		case ExportFormat::Tsv:
			return make_unique<TsvExporter>();
		case ExportFormat::Xml:
			return make_unique<XmlExporter>();
		case ExportFormat::Json:
			return make_unique<JsonExporter>();
		default:
			throw std::runtime_error("Unknown export format.");
	}
}

ShapeSet getTargetShapeSet(const string& extendedShapesString) {
	// All basic shapes are mandatory
	ShapeSet result(ShapeConverter::get().getBasicShapes());

	// Add any extended shapes
	for (char ch : extendedShapesString) {
		Shape shape = ShapeConverter::get().parse(string(1, ch));
		result.insert(shape);
	}
	return result;
}

template <typename T>
std::vector<std::string> vec2strvec(const std::vector<T>& vec) {
	std::vector<std::string> res;
	for (auto elem : vec) {
		std::ostringstream oss;
		oss << elem;
		res.push_back(oss.str());
	}
	return res;
}

int main(int platformArgc, char* platformArgv[]) {
	// Set up default logging so early errors are printed to stdout
	const logging::Level defaultMinStderrLevel = logging::Level::Error;
	shared_ptr<logging::Sink> defaultSink = make_shared<NiceStderrSink>(defaultMinStderrLevel);
	logging::addSink(defaultSink);

	// Make sure the console uses UTF-8 on all platforms including Windows
	useUtf8ForConsole();

	// Convert command-line arguments to UTF-8
	const vector<string> args = argsToUtf8(platformArgc, platformArgv);

	// Define command-line parameters
	CLI::App app {appName, "placeholder"};
	app.set_version_flag("--version", appVersion);

	std::string outputFileName;
	auto* outputFileOpt = app.add_option("-o,--output", outputFileName, "The output file path.");
	
	std::string logLevelStr { "Debug" };
	auto logLevels = vector<logging::Level>( logging::LevelConverter::get().getValues() );
	
	app.add_option("--logLevel", logLevelStr, "The minimum log level for log file")
	->check(CLI::IsMember(vec2strvec<logging::Level>(logLevels)))
	->description("The minimum log level that will be written to the log file");
	
	std::string logFileName;
	auto* logFileOpt = app.add_option("--logFile", logFileName, "The log file path.");
	
	logging::Level consoleLevel = defaultMinStderrLevel;
	app.add_option(
		"--consoleLevel", consoleLevel, 
		"The minimum log level that will be printed on the console (stderr)"
	);
	
	bool machineReadableMode = false;
	app.add_flag(
		"--machineReadable", machineReadableMode,
		"Formats all output to stderr in a structured JSON format."
	);

	bool quietMode = false;
	app.add_flag(
		"-q,--quiet", quietMode,
		"Suppresses all output to stderr except for warnings and error messages."
	);

	int maxThreadCount = getProcessorCoreCount();
	app.add_option(
		"--threads", maxThreadCount,
		"The maximum number of worker threads to use."
	);

	std::string extendedShapes { "GHX" };
	app.add_option(
		"--extendedShapes", extendedShapes,
		"All extended, optional shapes to use."
	);
	
	std::string dialogFile;
	auto* dialogOpt = app.add_option(
		"-d,--dialogFile", dialogFile,
		"A file containing the text of the dialog."
	);
	
	bool datUsePrestonBlair = false;
	app.add_flag(
		"--datUsePrestonBlair", datUsePrestonBlair,
		"Only for dat exporter: uses the Preston Blair mouth shape names."
	);
	
	double datFrameRate = 24.0;
	app.add_option(
		"--datFrameRate", datFrameRate,
		"Only for dat exporter: the desired frame rate."
	);
	
	std::string exportFormatStr { "tsv" };
	auto exportFormats = vector<ExportFormat>( ExportFormatConverter::get().getValues() );

	app.add_option("-f,--exportFormat", exportFormatStr, "The export format.")
	->check(CLI::IsMember(vec2strvec<ExportFormat>(exportFormats)))
	->description("The export format.");

	std::string recognizerTypeStr { "pocketSphinx" };
	auto recognizerTypes = RecognizerTypeConverter::get().getValues();

	app.add_option("-r,--recognizer", recognizerTypeStr, "The dialog recognizer.")
	->check(CLI::IsMember(vec2strvec<RecognizerType>(recognizerTypes)))
	->description("The dialog recognizer.");
	
	std::string inputFileName;
	app.add_option(
		"inputFile", inputFileName, 
		"The input file. Must be a sound file in WAVE format."
	)
	->required();
	
	try {
		
		// Parse command line
		CLI11_PARSE(app, platformArgc, platformArgv);

		auto maybeLevel = logging::LevelConverter::get().tryParse(logLevelStr);
		if (!maybeLevel.has_value()) {
			throw CLI::ValidationError("Invalid log level: " + logLevelStr);
		}

		logging::Level logLevel = maybeLevel.value();
		// Convert values
		auto maybeFormat = ExportFormatConverter::get().tryParse(exportFormatStr);
		if (!maybeFormat.has_value()) {
			throw CLI::ValidationError("Invalid export format: " + exportFormatStr);
		}
		
		ExportFormat exportFormat = maybeFormat.value();

		auto maybeRecogType = RecognizerTypeConverter::get().tryParse(recognizerTypeStr);
		if (!maybeLevel.has_value()) {
			throw CLI::ValidationError("Invalid recognizer: " + recognizerTypeStr);
		}

		RecognizerType recognizerType = maybeRecogType.value();

		
		// Set up logging
		// ... to stderr
		if (quietMode) {
			logging::addSink(make_shared<QuietStderrSink>(consoleLevel));
		} else if (machineReadableMode) {
			logging::addSink(make_shared<MachineReadableStderrSink>(consoleLevel));
		} else {
			logging::addSink(make_shared<NiceStderrSink>(consoleLevel));
		}
		logging::removeSink(defaultSink);
		// ... to log file
		if (*logFileOpt) {
			auto fileSink = createFileSink(logFileName, logLevel);
			logging::addSink(fileSink);
		}

		// Validate and transform command line arguments
		if (maxThreadCount < 1) {
			throw std::runtime_error("Thread count must be 1 or higher.");
		}
		path inputFilePath { inputFileName };
		ShapeSet targetShapeSet = getTargetShapeSet(extendedShapes);

		unique_ptr<Exporter> exporter = createExporter(
			exportFormat,
			targetShapeSet,
			datFrameRate,
			datUsePrestonBlair
		);

		logging::log(StartEntry(inputFilePath));
		logging::debugFormat("Command line: {}",
			join(args | transform([](string arg) { return std::format("\"{}\"", arg); }), " "));

		try {
			// On progress change: Create log message
			ProgressForwarder progressSink([](double progress) {
				logging::log(ProgressEntry(progress));
			});

			// Animate the recording
			logging::info("Starting animation.");
			JoiningContinuousTimeline<Shape> animation = animateWaveFile(
				inputFilePath,
				static_cast<bool>(*dialogOpt)
					? readUtf8File(dialogFile)
					: std::optional<string>(),
				*createRecognizer(recognizerType),
				targetShapeSet,
				maxThreadCount,
				progressSink);
			logging::info("Done animating.");

			// Export animation
			optional<std::ofstream> outputFile;
			if (*outputFileOpt) {
				outputFile.emplace(path { outputFileName });
				outputFile->exceptions(std::ifstream::failbit | std::ifstream::badbit);
			}
			ExporterInput exporterInput = ExporterInput(inputFilePath, animation, targetShapeSet);
			logging::info("Starting export.");
			exporter->exportAnimation(exporterInput, outputFile ? *outputFile : std::cout);
			logging::info("Done exporting.");

			logging::log(SuccessEntry());
		} catch (...) {
			std::throw_with_nested(
				std::runtime_error(std::format("Error processing file {}.", inputFilePath.string()))
			);
		}
	} catch (const exception& e) {
		// Generic error
		string message = getMessage(e);
		logging::log(FailureEntry(message));
		return 1;
	}
	return 0;
}
