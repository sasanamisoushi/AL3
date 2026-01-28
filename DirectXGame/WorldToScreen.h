#pragma once
#include "MyMath.h"

namespace KamataEngine {

inline Vector2 WorldToScreen(const Vector3& worldPos, const Camera* camera, float screenWidth = 1280.0f, float screenHeight = 720.0f) {
	// ViewProjection
	Matrix4x4 vp = Multiply(camera->matView, camera->matProjection);

	// Vector3 用 Transform（KamataEngine標準）
	Vector3 clipPos = Transform(worldPos, vp);

	// Z が 0 以下 → カメラ後方
	if (clipPos.z <= 0.0f) {
		return {-1000.0f, -1000.0f};
	}

	// NDC（KamataEngine の Transform は w=1 前提なのでそのままでOK）
	float ndcX = clipPos.x;
	float ndcY = clipPos.y;

	Vector2 screen;
	screen.x = (ndcX * 0.5f + 0.5f) * screenWidth;
	screen.y = (1.0f - (ndcY * 0.5f + 0.5f)) * screenHeight;

	return screen;
}

} // namespace KamataEngine