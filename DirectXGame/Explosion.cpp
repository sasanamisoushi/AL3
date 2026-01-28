#include "Explosion.h"
#include "MyMath.h"
#include "Player.h"

using namespace KamataEngine;

void Explosion::Initialize(Model* model,const Vector3& position, Camera* camera) {
	position_ = position;
	camera_ = camera;

	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
}

void Explosion::Update() {
	timer_++;
	float t = (float)timer_ / duration_;

	// 拡大
	float size = Lerp(5.0f, 30.0f, t);
	worldTransform_.scale_ = {size, size, size};

	// ビルボード処理（常にカメラの角度と同じ向きにする）
	if (camera_) {
		worldTransform_.rotation_ = camera_->rotation_;
	}

	//回転
	if (camera_) {
		worldTransform_.rotation_ = camera_->rotation_;
	}

	// フェードアウト
	if (model_) {
		model_->SetAlpha(1.0f - t);
	}

	// 行列の更新
	WorldTransformUpdate(worldTransform_);

	// 座標の更新（移動する場合）
	worldTransform_.translation_ = position_;
	// 転送
	worldTransform_.TransferMatrix();

	if (timer_ >= duration_) {
		isFinished_ = true;
	}

}

void Explosion::Draw() {
	if (model_) {
		model_->Draw(worldTransform_, *camera_);
	}
}
