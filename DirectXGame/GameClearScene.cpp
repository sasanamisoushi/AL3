#include "GameClearScene.h"

using namespace KamataEngine;

void GameClearScene::Initialize() {

	gameClear_ = Sprite::Create(TextureManager::Load("./Resources/GAME CLEAR.png"), {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});

}

void GameClearScene::Update() {
	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		isFinish = true;
	}


}

void GameClearScene::Draw() {

	// ===== 2D描画（Sprite）=====
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	Sprite::PreDraw(dxCommon->GetCommandList());
	if (gameClear_) {
		gameClear_->Draw();
	}
	Sprite::PostDraw();

}

GameClearScene::~GameClearScene() {


	delete gameClear_;
	gameClear_ = nullptr;

}
