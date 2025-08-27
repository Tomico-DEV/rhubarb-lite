#pragma once

#include "audio/AudioClip.h"
#include "animation/targetShapeSet.h"
#include "core/Shape.h"
#include <filesystem>
#include "time/ContinuousTimeline.h"
#include "tools/progress.h"
#include "recognition/Recognizer.h"

JoiningContinuousTimeline<Shape> animateAudioClip(
	const AudioClip& audioClip,
	const std::optional<std::string>& dialog,
	const Recognizer& recognizer,
	const ShapeSet& targetShapeSet,
	int maxThreadCount,
	ProgressSink& progressSink);

JoiningContinuousTimeline<Shape> animateWaveFile(
	std::filesystem::path filePath,
	const std::optional<std::string>& dialog,
	const Recognizer& recognizer,
	const ShapeSet& targetShapeSet,
	int maxThreadCount,
	ProgressSink& progressSink);
