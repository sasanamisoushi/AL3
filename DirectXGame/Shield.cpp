#include "Shield.h"
#include "MyMath.h"
#include <numbers>


using namespace KamataEngine;

void Shield::Initialize(Model* model, Camera* camera, const Vector3& position) {
	// NULLポインタチェック
	assert(model);
	// モデル
	model_ = model;

	// カメラ
	camera_ = camera;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = {1.2f, 1.2f, 1.2f};
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float>;

	WorldTransformUpdate(worldTransform_);

}

void Shield::Update() {

	

}

void Shield::Draw() {
	if (isBroken_)
		return; // 壊れている時は描画しない

	if (model_) {
		model_->Draw(worldTransform_, *camera_);
	}
}

void Shield::SetPosition(const Vector3& pos, const Vector3& rot) {
	worldTransform_.translation_ = pos;
	worldTransform_.rotation_ = rot;
	WorldTransformUpdate(worldTransform_);
}

void Shield::SetGuarding(bool guarding) {
	if (!isBroken_) {
		isGuarding_ = guarding;
	}
}

void Shield::TakeDamage(int amount) {
	if (isBroken_)
		return;

	durability_ -= amount;

	if (durability_ <= 0) {
		durability_ = 0;
		isBroken_ = true; // 盾が破壊される！
	}
}
