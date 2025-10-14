#include "FollowCamera.h"
#include <numbers>

void FollowCamera::Initialize(Camera* camera_) {
	camera = camera_;
	camera->Initialize();
}

void FollowCamera::Update() {

	if (!target_) {
		return;
	}

	// プレイヤーの位置＋オフセット
	Vector3 targetPos = *target_ + targetOffset_;

	Matrix4x4 rotateY = MakeRotateYMatrix(targetRotation_->y - std::numbers::pi_v<float> / 2.0f);

	// カメラの回転を反転した距離方向
	Vector3 offset = {0.0f, 0.0f, -distance_};

	Vector3 rotatedoffset = TransformNormal(offset, rotateY);

	// カメラの位置
	translation_ = targetPos + rotatedoffset;

	// 注視点を設定
	lookAt_ = targetPos;

	// カメラへの反転
	camera->translation_ = translation_;
	camera->rotation_.y = targetRotation_->y - std::numbers::pi_v<float> / 2.0f;
	camera->UpdateMatrix();
}
