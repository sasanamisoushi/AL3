#include "GameScene.h"

using namespace KamataEngine;

void GameScene::Initialize() {

	// カメラの初期化
	camera_.Initialize();

	 // カメラ位置と注視点を設定
	camera_.translation_ = {0.0f, 10.0f, -30.0f};
	camera_.rotation_ = {0.3f, 0.0f, 0.0f};
	camera_.UpdateMatrix();  

	//フィールドオブジェクト
	fieldModel_ = Model::CreateFromOBJ("field");

	//フィールドの生成
	field = new Field();
	field->Initialize(fieldModel_, &camera_, {0.0f, -5.0f, 0.0f});
	
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
	field->Draw();
}

GameScene::~GameScene() { 
	
	//フィールドの解放
	delete field; 
}


