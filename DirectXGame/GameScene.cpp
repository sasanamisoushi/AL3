#include "GameScene.h"

using namespace KamataEngine;

void GameScene::Initialize() {

	// カメラの初期化
	camera_.Initialize();

	
}

void GameScene::Update() {


	// キーを押したらクリア画面に
	if (Input::GetInstance()->TriggerKey(DIK_1)) {
		isFinish = true;
	}

#ifdef _DEBUG
	ImGui::Begin("Game Scene");
	ImGui::End();
#endif
}

void GameScene::Draw() {
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());
	
	Model::PostDraw();
}


