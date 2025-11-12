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

void Rifle::Update() {
	// リロード処理
	if (isReloading_) {
		reloadTimer_ -= 1.0f / 60.0f; // 60FPS想定
		if (reloadTimer_ <= 0.0f) {
			isReloading_ = false;
			ammo_ = maxAmmo_;
		}
	}

}

void Rifle::Draw() {

	model_->Draw(worldTransform_, *camera_); }

void Rifle::SetPosition(const Vector3& position, const Vector3& rotation) { 
	// 座標と回転を設定
	worldTransform_.translation_ = position;
	worldTransform_.rotation_ = rotation;
	WorldTransformUpdate(worldTransform_);
}

Vector3 Rifle::GetForwardVector() const { 

	// 銃の前方向ベクトル（Z軸正方向）
	Vector3 forward = {0.0f, 0.0f, 1.0f};
	// 銃のY回転を反映
	Matrix4x4 rotMat = MakeRotateYMatrix(worldTransform_.rotation_.y);
	return TransformNormal(forward, rotMat);
}

Vector3 Rifle::GetMuzzlePosition() const { 
	// 銃口の相対位置（銃の先端）
	Vector3 muzzleoffset = {0.0f, 0.0f, 1.0f};
	// 銃のY回転を反映してワールド座標を計算
	Matrix4x4 rotMat = MakeRotateYMatrix(worldTransform_.rotation_.y);
	return worldTransform_.translation_+TransformNormal(muzzleoffset,rotMat); 
}

void Rifle::Fire(BulletManager* bulletManager) { 
	// リロード中は発射できない
	if (isReloading_) {
		return;
	}

	// 弾数が0なら発射できない
	if (ammo_ <= 0) {
		Reload();
		return;
	}

	// 弾の発射処理	
	Vector3 pos = GetMuzzlePosition();
	Vector3 dir = GetForwardVector();
	bulletManager->Fire(pos, dir);

	ammo_--;
}

void Rifle::Reload() { 
	// 既にリロード中、または弾が満タンならリロードしない
	if (!isReloading_ && ammo_ < maxAmmo_) {
	
		isReloading_ = true;
		reloadTimer_ = reloadTime_;
	} 

}
