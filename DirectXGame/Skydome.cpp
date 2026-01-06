#include "Skydome.h"
#include <cassert>

using namespace KamataEngine;

void Skydome::Initialize(Model* model, Camera* camera) {
	// NULLポインタチェック
	assert(model);
	// モデル
	model_ = model;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	camera_ = camera;
}

void Skydome::Update() {
	//行列を定義バッファに転送
	worldTransform_.TransferMatrix();
}


void Skydome::Draw() {
	if (!model_ || !camera_)
		return; // 安全策

	model_->Draw(worldTransform_, *camera_);
}