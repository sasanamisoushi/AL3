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


	//ロックオン時の注視点制御
	if (isLockOn_ && lockOnTarget_) {
		//敵の位置方向を注視
		lookAt_ = *lockOnTarget_;

		//敵の方向を向くように回転角度を計算
		Vector3 toEnemy = *lockOnTarget_ - *target_;
		float angleY = std::atan2(toEnemy.x, toEnemy.z);
		camera->rotation_.y = angleY;
	} else {
		//通常はプレイヤーの前方を見る
		lookAt_ = targetPos;
		camera->rotation_.y = targetRotation_->y - std::numbers::pi_v<float> / 2.0f;
	}

	// カメラへの反転
	camera->translation_ = translation_;
	camera->UpdateMatrix();
}


