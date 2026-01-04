#include "StageBounds.h"
#include <algorithm>

constexpr float kStageMinX = -100.0f;
constexpr float kStageMaxX = 100.0f;
constexpr float kStageMinZ = -100.0f;
constexpr float kStageMaxZ = 100.0f;

void StageBounds::ClampToStage(KamataEngine::Vector3& pos, float radius) {
	pos.x = std::clamp(pos.x, kStageMinX + radius, kStageMaxX - radius);
	pos.z = std::clamp(pos.z, kStageMinZ + radius, kStageMaxZ - radius);
}
