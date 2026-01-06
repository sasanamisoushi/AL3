#include "TitleScene.h"

using namespace KamataEngine;

void TitleScene::Initialize() {

titleSprite_ = Sprite::Create(TextureManager::Load("./Resources/Title.png"), {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});

}

void TitleScene::Update() {
	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		isFinish = true;
	}

#ifdef _DEBUG
	ImGui::Begin("Title Scene");
	ImGui::Text("Press SPACE to Start");
	ImGui::End();
#endif
}

void TitleScene::Draw() {
	// ===== 2D描画（Sprite）=====
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Sprite::PreDraw(dxCommon->GetCommandList());
	if (titleSprite_) {
		titleSprite_->Draw();
	}
	Sprite::PostDraw();
}
