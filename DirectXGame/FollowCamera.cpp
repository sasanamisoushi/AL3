#include "FollowCamera.h"
#include <numbers>
#include <algorithm>

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

	// --- 距離調整（ロックオン時） ---
	if (isLockOn_ && lockOnTarget_) {
		float dist = Length(*lockOnTarget_ - *target_);
		distance_ = std::clamp(dist * 0.6f, 8.0f, 20.0f);
	} else {
		// 通常時は一定距離
		distance_ = 10.0f;
	}

	// カメラの回転を反転した距離方向
	Vector3 offset = {0.0f, 0.0f, -distance_};

	Vector3 rotatedoffset = TransformNormal(offset, rotateY);
	
	// カメラの位置
	Vector3 desiredPos = targetPos + rotatedoffset;

	// ---- プレイヤーを中心に保つための補間追従 ----
	float followSpeed = 0.15f;

	// 現在位置から目標位置に保管してスムーズに移動
	translation_.x = Lerp(translation_.x, desiredPos.x, followSpeed);
	translation_.z = Lerp(translation_.z, desiredPos.z, followSpeed);

	// 高さだけ別のスピードで追従（ちょっと遅めにすると自然）
	float heightFollowSpeed = 0.25f;
	translation_.y = Lerp(translation_.y, desiredPos.y, heightFollowSpeed);

	// ---- 注視点設定 ----
	Vector3 desiredLookAt;

	if (isLockOn_ && lockOnTarget_) {
		// 敵との中点を注視
		desiredLookAt = (*target_ + *lockOnTarget_) * 0.5f;
	} else {
		// 通常はプレイヤーを注視
		desiredLookAt = targetPos;
	}

	// 見やすさのため、注視点を少し上にずらす
	desiredLookAt.y += 1.5f;

	// スムーズに注視点を補間
	lookAt_ = Lerp(lookAt_, desiredLookAt, 0.25f);
	
	// ---- カメラ回転を自動補正 ----
	float desiredY;
	if (isLockOn_ && lockOnTarget_) {
		Vector3 toEnemy = *lockOnTarget_ - *target_;
		desiredY = std::atan2(toEnemy.x, toEnemy.z);
	} else {
		
		desiredY = targetRotation_->y - std::numbers::pi_v<float> / 2.0f;
	}

	// --- カメラ回転を補間 ---
	camera->rotation_.y = LerpAngle(camera->rotation_.y, desiredY, 0.15f);
 
	// カメラへの反転
	camera->translation_ = translation_;
	camera->UpdateMatrix();
}


