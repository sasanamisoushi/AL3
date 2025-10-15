#include "Bullet.h"

void Bullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {

	//NULLポインタチェック
	assert(model);

	model_ = model;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	//発射方向へスピード設定
	velocity_ = velocity*1.0f;

	WorldTransformUpdate(worldTransform_);

}

void Bullet::Update() {}

void Bullet::Draw(Camera* camera) {
	if (!IsDead_) {
		model_->Draw(worldTransform_, *camera);
	}
}

