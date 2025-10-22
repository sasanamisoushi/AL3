#include "Bullet.h"
#include <numbers>

void Bullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {

	//NULLポインタチェック
	assert(model);

	model_ = model;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	//発射方向へスピード設定
	velocity_ = velocity*1.0f;

	//60フレーム生存
	lifeTime_ = 60.0f;

	WorldTransformUpdate(worldTransform_);

}

void Bullet::Update() { 
	
	worldTransform_.translation_ += velocity_;

	lifeTime_--;
	if (lifeTime_ <= 0) {
	
		IsDead_ = true;
	}

	WorldTransformUpdate(worldTransform_);
}

void Bullet::Draw(Camera* camera) {
	if (!IsDead_) {
		model_->Draw(worldTransform_, *camera);
	}
}

