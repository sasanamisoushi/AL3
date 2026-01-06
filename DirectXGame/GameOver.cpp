#include "GameOver.h"

using namespace KamataEngine;

void GameOver::Initialize() {

	gameOver_ = Sprite::Create(TextureManager::Load("./Resources/GAME OVER.png"), {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});

}

void GameOver::Update() {
	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		isFinish = true;
	}

	#ifdef _DEBUG
	ImGui::Begin("Game Over Sene");
	ImGui::Text("Press SPACE to Start");
	ImGui::End();
#endif
}

void GameOver::Draw() {
	// ===== 2D描画（Sprite）=====
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Sprite::PreDraw(dxCommon->GetCommandList());
	if (gameOver_) {
		gameOver_->Draw();
	}
	Sprite::PostDraw();
}

GameOver::~GameOver() {

	delete gameOver_;
	gameOver_ = nullptr;

}
