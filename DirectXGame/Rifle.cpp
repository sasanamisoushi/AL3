#define NOMINMAX
#include <windows.h>
#include "Rifle.h"
#include <numbers>
#include <algorithm> 
#include <cassert>

using namespace KamataEngine;

void Rifle::Initialize(Model* model, Camera* camera, const Vector3& position) {

	// NULLポインタチェック
	assert(model);
	// モデル
	model_ = model;

	// カメラ
	camera_ = camera;

	// 初期弾数
	ammo_ = maxAmmo_;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float>;

	WorldTransformUpdate(worldTransform_);
}

void Rifle::Update() {

	//発射クールタイム処理
	if (fireTimer_ > 0.0f) {
		fireTimer_ -= 1.0f / 60.0f;
	}

	// リロード処理
	if (isReloading_) {
		reloadTimer_ -= 1.0f / 60.0f; // 60FPS想定
		if (reloadTimer_ <= 0.0f) {
			isReloading_ = false;
			// 残弾の欠け分を計算
			int need = maxAmmo_ - ammo_;

			// 実際に装填できる弾数（予備弾が足りない場合あり）
			int load = std::min(need, reserveAmmo_);

			ammo_ += load;
			reserveAmmo_ -= load;
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
	// 銃のX,Y回転を反映
	Matrix4x4 rotX = MakeRotateXMatrix(worldTransform_.rotation_.x);
	Matrix4x4 rotY = MakeRotateYMatrix(worldTransform_.rotation_.y);

	//回転合成
	Matrix4x4 rotMat = Multiply(rotX, rotY);
	return TransformNormal(forward, rotMat);
}

Vector3 Rifle::GetMuzzlePosition() const { 
	// 銃口の相対位置（銃の先端）
	Vector3 muzzleoffset = {0.0f, 0.0f, 1.0f};
	// 銃のY回転を反映してワールド座標を計算
	Matrix4x4 rotMat = MakeRotateYMatrix(worldTransform_.rotation_.y);
	return worldTransform_.translation_+TransformNormal(muzzleoffset,rotMat); 
}

void Rifle::Fire(BulletManager* bulletManager, Bullet::Owner owner) { 
	// リロード中は発射できない
	if (isReloading_) {
		return;
	}

	// 弾数が0なら発射できない
	if (ammo_ <= 0) {
		return;
	}

	//クールタイム中は発射できない
	if(fireTimer_ > 0.0f) {
		return;
	}

	// 弾の発射処理	
	Vector3 pos = GetMuzzlePosition();
	Vector3 dir = GetForwardVector();

	bulletManager->Fire(pos, dir, owner);

	ammo_--;
	fireTimer_ = fireInterval_;
}

void Rifle::Reload() { 

	// 既にリロード中、または弾が満タンならリロードしない
	if (isReloading_ || ammo_ == maxAmmo_) {
		return;
	}


	// 予備弾が０ならリロードしない
	if (reserveAmmo_ <= 0) {
		return;
	}

	
		isReloading_ = true;
		reloadTimer_ = reloadTime_;
	

}

Rifle::~Rifle() { model_ = nullptr; }
