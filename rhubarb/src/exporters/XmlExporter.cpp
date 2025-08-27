#include "exporters/XmlExporter.h"
#include "exporters/exporterTools.h"

#include "tools/utils.h"
#include "pugixml.hpp"

using namespace pugi;

void XmlExporter::exportAnimation(const ExporterInput& input, std::ostream& outputStream) {
    xml_document doc;

    auto root = doc.append_child("rhubarbResult");

    auto metadata = root.append_child("metadata");
    metadata.append_child("soundFile").text().set(absolute(input.inputFilePath).string().c_str());
    metadata.append_child("duration").text().set(formatDuration(input.animation.getRange().getDuration()).c_str());

    auto mouthCues = root.append_child("mouthCues");

    for (auto& timedShape : dummyShapeIfEmpty(input.animation, input.targetShapeSet)) {
        auto mouthCue = mouthCues.append_child("mouthCue");
        mouthCue.text().set(lexical_cast(timedShape.getValue()).c_str());
        mouthCue.append_attribute("start") = formatDuration(timedShape.getStart()).c_str();
        mouthCue.append_attribute("end") = formatDuration(timedShape.getEnd()).c_str();
    }

    doc.save(outputStream, "  ");  // pretty-print with 2 spaces
}
