#include "SkeletonData.h"

namespace spine21 {

static void readSteppedCurve(const Json& frameJson, TimelineFrame& frame) {
    if (frameJson.contains("curve") && frameJson["curve"] == "stepped") {
        frame.curveType = CurveType::CURVE_STEPPED;
    }
}

static void readTimeline(const Json& j, Timeline& timeline, int valueNum, const std::string& key1, const std::string& key2, float defaultValue) {
    for (const auto& frameJson : j) {
        TimelineFrame frame;
        frame.time = frameJson.value("time", 0.0f);
        frame.value1 = frameJson.value(key1, defaultValue);
        if (valueNum > 1) frame.value2 = frameJson.value(key2, defaultValue);
        readSteppedCurve(frameJson, frame);
        timeline.push_back(frame);
    }
}

SkeletonData readJsonData(const Json& j) {
    SkeletonData skeletonData;

    if (j.contains("skeleton") && j["skeleton"].is_object()) {
        const auto& skeleton = j["skeleton"];
        if (skeleton.contains("hash") && skeleton["hash"].is_string()) {
            skeletonData.hashString = skeleton["hash"].get<std::string>();
            skeletonData.hash = base64ToUint64(skeletonData.hashString.value());
        }
        if (skeleton.contains("spine")) skeletonData.version = skeleton["spine"].get<std::string>();
        skeletonData.x = skeleton.value("x", 0.0f);
        skeletonData.y = skeleton.value("y", 0.0f);
        skeletonData.width = skeleton.value("width", 0.0f);
        skeletonData.height = skeleton.value("height", 0.0f);
    } else {
        skeletonData.version = std::string("2.1.27");
    }

    if (j.contains("bones")) {
        for (const auto& boneJson : j["bones"]) {
            BoneData boneData;
            boneData.name = boneJson.value("name", "");
            if (boneJson.contains("parent")) boneData.parent = boneJson["parent"].get<std::string>();
            boneData.length = boneJson.value("length", 0.0f);
            boneData.x = boneJson.value("x", 0.0f);
            boneData.y = boneJson.value("y", 0.0f);
            boneData.rotation = boneJson.value("rotation", 0.0f);
            boneData.scaleX = boneJson.value("scaleX", 1.0f);
            boneData.scaleY = boneJson.value("scaleY", 1.0f);
            skeletonData.bones.push_back(boneData);
        }
    }

    if (j.contains("slots")) {
        for (const auto& slotJson : j["slots"]) {
            SlotData slotData;
            slotData.name = slotJson.value("name", "");
            if (slotJson.contains("bone")) slotData.bone = slotJson["bone"].get<std::string>();
            if (slotJson.contains("attachment") && !slotJson["attachment"].is_null()) {
                slotData.attachmentName = slotJson["attachment"].get<std::string>();
            }
            skeletonData.slots.push_back(slotData);
        }
    }

    if (j.contains("skins") && j["skins"].is_object()) {
        for (const auto& [skinName, skinAttachmentsJson] : j["skins"].items()) {
            Skin skinData;
            skinData.name = skinName;
            for (const auto& [slotName, slotAttachments] : skinAttachmentsJson.items()) {
                for (const auto& [attachmentName, attachmentJson] : slotAttachments.items()) {
                    Attachment attachment;
                    attachment.name = attachmentJson.value("name", attachmentName);
                    attachment.path = attachmentJson.value("path", attachment.name);
                    attachment.type = AttachmentType_Region;
                    RegionAttachment region;
                    region.x = attachmentJson.value("x", 0.0f);
                    region.y = attachmentJson.value("y", 0.0f);
                    region.rotation = attachmentJson.value("rotation", 0.0f);
                    region.scaleX = attachmentJson.value("scaleX", 1.0f);
                    region.scaleY = attachmentJson.value("scaleY", 1.0f);
                    region.width = attachmentJson.value("width", 32.0f);
                    region.height = attachmentJson.value("height", 32.0f);
                    attachment.data = region;
                    skinData.attachments[slotName][attachmentName] = attachment;
                }
            }
            skeletonData.skins.push_back(skinData);
        }
    }

    if (j.contains("animations")) {
        for (const auto& [animationName, animationJson] : j["animations"].items()) {
            Animation animation;
            animation.name = animationName;

            if (animationJson.contains("ik") || animationJson.contains("ffd") || animationJson.contains("deform")) {
                std::cerr << "Warning: Spine 2.1 feature is not fully supported; skipped during 2.1 -> 3.8 conversion.\n";
            }

            if (animationJson.contains("slots")) {
                for (const auto& [slotName, slotJson] : animationJson["slots"].items()) {
                    if (slotJson.contains("attachment")) {
                        Timeline timeline;
                        for (const auto& frameJson : slotJson["attachment"]) {
                            TimelineFrame frame;
                            frame.time = frameJson.value("time", 0.0f);
                            if (frameJson.contains("name") && !frameJson["name"].is_null()) {
                                frame.str1 = frameJson["name"].get<std::string>();
                            }
                            timeline.push_back(frame);
                        }
                        animation.slots[slotName]["attachment"] = timeline;
                    }
                }
            }

            if (animationJson.contains("bones")) {
                for (const auto& [boneName, boneJson] : animationJson["bones"].items()) {
                    if (boneJson.contains("rotate")) {
                        readTimeline(boneJson["rotate"], animation.bones[boneName]["rotate"], 1, "angle", "", 0.0f);
                    }
                    if (boneJson.contains("translate")) {
                        readTimeline(boneJson["translate"], animation.bones[boneName]["translate"], 2, "x", "y", 0.0f);
                    }
                    if (boneJson.contains("scale")) {
                        readTimeline(boneJson["scale"], animation.bones[boneName]["scale"], 2, "x", "y", 1.0f);
                    }
                }
            }

            if (animationJson.contains("drawOrder")) {
                Timeline timeline;
                for (const auto& frameJson : animationJson["drawOrder"]) {
                    TimelineFrame frame;
                    frame.time = frameJson.value("time", 0.0f);
                    timeline.push_back(frame);
                }
                animation.drawOrder = timeline;
            }

            skeletonData.animations.push_back(animation);
        }
    }

    return skeletonData;
}

} // namespace spine21
