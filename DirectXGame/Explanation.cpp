#include "Explanation.h"

using namespace KamataEngine;


void Explanation::Initialize(){ explanation_ = Sprite::Create(TextureManager::Load("./Resources/explanation.png"), {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}); }


void Explanation::Update() {
	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		isFinish = true;
	}
}

void Explanation::Draw() {
	// ===== 2D描画（Sprite）=====
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Sprite::PreDraw(dxCommon->GetCommandList());
	if (explanation_) {
		explanation_->Draw();
	}
	Sprite::PostDraw();
}

Explanation::~Explanation() {

	delete explanation_;
	explanation_ = nullptr;

}
