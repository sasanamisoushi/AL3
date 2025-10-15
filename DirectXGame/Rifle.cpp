#include "Rifle.h"
#include <numbers>

void Rifle::Initialize(Model* model, Camera* camera, const Vector3& position) {

	// NULLポインタチェック
	assert(model);
	// モデル
	model_ = model;

	// カメラ
	camera_ = camera;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float>;

	WorldTransformUpdate(worldTransform_);

}

void Rifle::Update() {}

void Rifle::Draw() {

	model_->Draw(worldTransform_, *camera_); }

void Rifle::SetPosition(const Vector3& position, const Vector3& rotation) { 
	worldTransform_.translation_ = position;
	worldTransform_.rotation_ = rotation;
	WorldTransformUpdate(worldTransform_);
}

Vector3 Rifle::GetForwardVector() const { 
	Vector3 forward = {0.0f, 0.0f, 1.0f};
	Matrix4x4 rotMat = MakeRotateYMatrix(worldTransform_.rotation_.y);
	return TransformNormal(forward, rotMat);
}

Vector3 Rifle::GetMuzzlePosition() const { 
	Vector3 muzzleoffset = {0.0f, 0.0f, 1.0f};
	Matrix4x4 rotMat = MakeRotateYMatrix(worldTransform_.rotation_.y);
	return worldTransform_.translation_+TransformNormal(muzzleoffset,rotMat); 
}

void Rifle::Fire(BulletManager* bulletManager) { 
	Vector3 pos = GetMuzzlePosition();
	Vector3 dir = GetForwardVector();
	bulletManager->Fire(pos, dir);
}
