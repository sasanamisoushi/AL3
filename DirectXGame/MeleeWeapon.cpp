#include "MeleeWeapon.h"
#include "MyMath.h"

void MeleeWeapon::Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position) {
	// モデルのセット
	model_ = model;
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	// 基本回転角の保存
	baseRotation_ = worldTransform_.rotation_;
	// ワールド変換行列の更新
	WorldTransformUpdate(worldTransform_);
}


void MeleeWeapon::Update() {
	// 何もしなくてOK（空でいい）
}

void MeleeWeapon::Draw(KamataEngine::Camera* camera) {
	//モデルの描画
	model_->Draw(worldTransform_, *camera);
}