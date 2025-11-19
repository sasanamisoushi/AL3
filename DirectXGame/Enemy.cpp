#include "Enemy.h"
#include <numbers>

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
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
	worldTransform_.rotation_.y = std::numbers::pi_v<float> ;

	//自身の座標を保持
	position_ = position;

	WorldTransformUpdate(worldTransform_);
}

void Enemy::Update() {
#ifdef _DEBUG
	ImGui::Begin("Enemy");
	ImGui::Text("HP:%d", hp_);
	ImGui::End();
#endif


}

void Enemy::Draw() {

	
	model_->Draw(worldTransform_, *camera_); }

bool Enemy::HitChek(const Vector3& point, float r) {
	float dist = Length(position_ - point);
	return dist < (radius_ + r);
}
