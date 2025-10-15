#include "GameScene.h"

using namespace KamataEngine;

void GameScene::Initialize() {

	// カメラの初期化
	camera_.Initialize();

	 // カメラ位置と注視点を設定
	camera_.translation_ = {0.0f, 10.0f, -30.0f};
	camera_.rotation_ = {0.3f, 0.0f, 0.0f};
	camera_.UpdateMatrix();  

	//------------フェールド-----------------

	//フィールドオブジェクト
	fieldModel_ = Model::CreateFromOBJ("field");

	//フィールドの生成
	field = new Field();
	field->Initialize(fieldModel_, &camera_, {0.0f, -5.0f, 0.0f});

	//------------プレイヤー-----------------

	//プレイヤーオブジェクト
	playerModel_ = Model::CreateFromOBJ("Enemy1");

	//プレイヤーの生成
	player = new Player();
	player->Initialize(playerModel_, &camera_, {0.0f, 0.5f, 0.0f});

	//------------敵-----------------

	//敵のオブジェクト
	enemyModel = Model::CreateFromOBJ("Enemy2");

	//敵の生成
	enemy = new Enemy();

	enemy->Initialize(enemyModel, &camera_, {-5.0f, 0.5f, 5.0f});

	
}

void GameScene::Update() {

	player->Update(&bulletManager_);

	
	//Lキーでロックオン切り替え
	if (Input::GetInstance()->TriggerKey(DIK_L)) {
		bool now = player->GetLockOn();
		player->SetLockOn(!now);
		followCamera_.SetLockOn(!now);
	}

	//敵の座標を登録
	player->SetLockOnTarget(&enemy->GetPosition());
	followCamera_.SetLockOnTarget(&enemy->GetPosition());

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
	field->Draw();
	player->Draw();
	enemy->Draw();
	Model::PostDraw();
}

GameScene::~GameScene() { 
	
	//フィールドの解放
	delete field;
	delete fieldModel_;

	//プレイヤーの解放
	delete player;
	delete playerModel_;

	//敵の解放
	delete enemy;
	delete enemyModel;
}


