#include "FollowCamera.h"
#include <numbers>
#include <algorithm>
#include <Windows.h>

using namespace KamataEngine;

void FollowCamera::Initialize(Camera* camera_) {
	camera = camera_;
	camera->Initialize();

	 translation_ = {0.0f, 3.0f, -10.0f};
	yaw_ = 0.0f;
	pitch_ = 0.25f;
}

void FollowCamera::Update() {

	if (!target_ ) {
		return;
	}

	// プレイヤーの位置＋オフセット
	Vector3 targetPos = *target_ + targetOffset_;

	Matrix4x4 rotateY = MakeRotateYMatrix(targetRotation_->y - std::numbers::pi_v<float> / 2.0f);

	// --- 距離調整（ロックオン時） ---
	if (isLockOn_ && lockOnTarget_) {

		 // ---------- ロックオン中 ----------
		// 敵方向ベクトル
		Vector3 toEnemy = *lockOnTarget_ - *target_;
		toEnemy.y = 0.0f;

		// カメラのY回転を「敵方向」に固定
		yaw_ = std::atan2(toEnemy.x, toEnemy.z);

		// pitch は固定（見下ろしすぎ防止）
		pitch_ = 0.25f;

		float dist = Length(*lockOnTarget_ - *target_);
		distance_ = std::clamp(dist * 0.6f, 8.0f, 20.0f);
	} else {
		// ---------- 非ロックオン ----------
		Input* input = Input::GetInstance();
		auto mouse = input->GetMouseMove();

		// マウスでカメラ操作
		yaw_ += mouse.lX * sensitivity_;
		pitch_ += mouse.lY * sensitivity_;

		// 上下向きすぎ防止
		pitch_ = std::clamp(pitch_, -1.2f, 1.2f);

		// 通常時は一定距離
		distance_ = 10.0f;
	}

	// ===== カメラの相対位置 =====
	float heightOffset = 0.0f;

	// カメラの回転を反転した距離方向
	Vector3 offset = {0.0f, heightOffset, -distance_};

	 // 回転行列
	Matrix4x4 rotX = MakeRotateXMatrix(pitch_);
	Matrix4x4 rotY = MakeRotateYMatrix(yaw_);
	Matrix4x4 rot = Multiply(rotX, rotY);

	// 回転後オフセット
	Vector3 rotatedoffset = TransformNormal(offset, rot);
	
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
	if (isLockOn_ && lockOnTarget_) {
		yaw_ = LerpAngle(yaw_, desiredY, 0.15f);
	}
 
	// カメラへの反転
	camera->translation_ = translation_;
	camera->rotation_.x = pitch_;
	//camera->rotation_.y = yaw_;
	//camera->UpdateMatrix();


	// ===== カメラシェイク =====
	//if (isShaking_) {
	//	shakeTimer_++;

	//	float t = (float)shakeTimer_ / shakeDuration_;

	//	// 減衰（最初強く、すぐ弱く）
	//	float power = shakePower_ * (1.0f - t);

	//	shakeOffset_.x = ((rand() / (float)RAND_MAX) * 2.0f - 1.0f) * power;
	//	shakeOffset_.y = ((rand() / (float)RAND_MAX) * 2.0f - 1.0f) * power;
	//	shakeOffset_.z = 0.0f;

	//	if (shakeTimer_ >= shakeDuration_) {
	//		isShaking_ = false;
	//		shakeOffset_ = {0, 0, 0};
	//	}
	//}

	// 最後に足す
	const float kMinCameraHeight = 1.0f;
	if (translation_.y < kMinCameraHeight) {
		translation_.y = kMinCameraHeight;
	}

	camera->translation_ = translation_;
	//camera->UpdateMatrix();


	 // ===== マウス入力 =====
   /* Input* input = Input::GetInstance();
	auto mouse = input->GetMouseMove();*/

   // マウスで回転
	/*yaw_ += mouse.lX * sensitivity_;
	pitch_ += mouse.lY * sensitivity_;*/

    // 上下向きすぎ防止
    //pitch_ = std::clamp(pitch_, -1.2f, 1.2f);

	if (!isLockOn_) {
		camera->rotation_.y = yaw_;
		camera->rotation_.x = pitch_;
	}


	// カメラの相対位置（後ろ）
	offset = {0.0f, 0.0f, -distance_};
	
	Vector3 cameraOffset = TransformNormal(offset, rot);

	// カメラ位置 = プレイヤー + オフセット
	camera->translation_ = *target_ + cameraOffset;

	bool hitFloor = false;

	// カメラの高さ制限
	if (camera->translation_.y < kMinCameraHeight) {
		camera->translation_.y = kMinCameraHeight;
		hitFloor = true;
	}

	if (hitFloor) {
		pitch_ = max(pitch_, -0.1f); // 下を向きすぎない
	}

	// 注視点はプレイヤー
	camera->rotation_.x = pitch_;
	camera->rotation_.y = yaw_;

	camera->UpdateMatrix();
}

KamataEngine::Vector3 FollowCamera::GetForward() const {  
	float y = camera->rotation_.y;
	return {std::sin(y), 0.0f, std::cos(y)};
}

KamataEngine::Vector3 FollowCamera::GetRight() const {
	float y = camera->rotation_.y;
	return {std::cos(y), 0.0f, -std::sin(y)};
}


