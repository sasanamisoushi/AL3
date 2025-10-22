#include "Bullet.h"
#include <numbers>

void Bullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {

	//NULLポインタチェック
	assert(model);

	model_ = model;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	//弾の向き
	worldTransform_.rotation_.y =std::atan2(velocity.x,velocity.z);

	//発射方向へスピード設定
	velocity_ = velocity*1.0f;

	//60フレーム生存
	lifeTime_ = 60.0f;

	WorldTransformUpdate(worldTransform_);

}

void Bullet::Update() { 
	
	//現在の速度ベクトル文だけ前進
	worldTransform_.translation_ += velocity_;

	lifeTime_--;

	//寿命が尽きたら削除フラグON
	if (lifeTime_ <= 0) {
	
		IsDead_ = true;
	}

	//行列を更新
	WorldTransformUpdate(worldTransform_);
}

void Bullet::Draw(Camera* camera) {
	if (!IsDead_) {
		model_->Draw(worldTransform_, *camera);
	}
}

