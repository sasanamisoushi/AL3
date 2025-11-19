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
	
	enemies_.push_back(new Enemy());
	enemies_.back()->Initialize(enemyModel, &camera_, {-5.0f, 0.5f, 5.0f},player);

	//------------弾-----------------

	//弾のオブジェクト
	bulletModel = Model::CreateFromOBJ("Bullet");

	bulletManager_ = new BulletManager();

	//弾の生成
	bulletManager_->Initialize(bulletModel);
}

void GameScene::Update() {

	//弾の更新
	bulletManager_->Update(enemies_);

	//プレイヤーの更新
	player->Update(bulletManager_,enemies_);

	for (auto* enemy : enemies_) {
		enemy->Update(bulletManager_);
	}
	
	//Lキーでロックオン切り替え
	if (Input::GetInstance()->TriggerKey(DIK_L)) {
		bool now = player->GetLockOn();
		player->SetLockOn(!now);
		followCamera_.SetLockOn(!now);
	}

	//敵の座標を登録
	/*player->SetLockOnTarget(&enemy->GetPosition());
	followCamera_.SetLockOnTarget(&enemy->GetPosition());*/

	// キーを押したらクリア画面に
	if (Input::GetInstance()->TriggerKey(DIK_1)) {
		isFinish = true;
	}

#ifdef _DEBUG
	ImGui::Begin("Game Scene");
	ImGui::Text("R choiceWeapon");
	ImGui::Text("L lock on");
	ImGui::Text("E Reload");
	ImGui::Text("SPACE attack");
	ImGui::End();
#endif
}

void GameScene::Draw() {

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());
	field->Draw();
	bulletManager_->Draw(&camera_);
	player->Draw();
	for (auto* enemy : enemies_) {
		enemy->Draw();
	}
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
	for (auto* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();

	delete enemyModel;

	//弾の解放
	delete bulletManager_;
	delete bulletModel;

}


