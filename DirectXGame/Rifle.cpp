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

	model_->Draw(worldTransform_, *camera_);
}
