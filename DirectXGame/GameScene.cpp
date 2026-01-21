#include "GameScene.h"

using namespace KamataEngine;

void GameScene::Initialize() {

	// カメラの初期化
	camera_.Initialize();

	// カメラ位置と注視点を設定
	camera_.translation_ = {0.0f, 10.0f, -30.0f};
	camera_.rotation_ = {0.3f, 0.0f, 0.0f};
	camera_.UpdateMatrix();

	followCamera_.Initialize(&camera_);
	followCamera_.SetTarget(&player->GetPosition());
	followCamera_.SetTargetRotation(&player->GetRotation());

	rifleModel_ = Model::CreateFromOBJ("Raifl");

	//------------フェールド-----------------

	// フィールドオブジェクト
	fieldModel_ = Model::CreateFromOBJ("field");

	// フィールドの生成
	field = new Field();
	field->Initialize(fieldModel_, &camera_, {0.0f, -5.0f, 0.0f});

	//------------プレイヤー-----------------

	// プレイヤーオブジェクト
	playerModel_ = Model::CreateFromOBJ("Enemy1");

	// プレイヤーの生成
	player = new Player();
	player->Initialize(playerModel_, rifleModel_,&camera_, {0.0f, 0.5f, 0.0f});

	//------------敵-----------------

	// 敵のオブジェクト
	enemyModel = Model::CreateFromOBJ("Enemy2");
	enemyDown = Model::CreateFromOBJ("Enemy2Down");

	// EnemyManager 初期化
	enemyManager_.Initialize(enemyModel, &camera_, player, enemyDown);



	////敵の生成
	// enemies_.push_back(new Enemy());
	// enemies_.back()->Initialize(enemyModel, &camera_, {-5.0f, 0.5f, 5.0f},player);

	//------------ボス-----------------

	bossModel_ = Model::CreateFromOBJ("boss");
	bossSwordModel_ = Model::CreateFromOBJ("saber", true);

	boss_ = nullptr;
	bossSpawned_ = false;

	//------------弾-----------------

	// 弾のオブジェクト
	bulletModel = Model::CreateFromOBJ("Bullet");

	bulletManager_ = new BulletManager();

	// 弾の生成
	bulletManager_->Initialize(bulletModel);

	//------------UI-----------------
	attackAlert_.Initialize();

	//------------天球-----------------
	//モデルの生成
	skydomeModel_ = Model::CreateFromOBJ("skydome", true);
	//天球の生成
	skydome_ = new Skydome();
	//天球の初期化
	skydome_->Initialize(skydomeModel_, &camera_);
}

void GameScene::Update() {

	if (boss_ && boss_->IsDead()) {
		isGameClear_ = true;
		return;
	}

	// プレイヤー死亡判定
	if (player->IsDead()) {
		isGameOver_ = true;
		return;
	}

	//天球の更新
	skydome_->Update();

	// プレイヤーの更新
	player->Update(bulletManager_, enemyManager_.GetEnemyPointers());

	// 弾の更新
	bulletManager_->Update(enemyManager_.GetEnemyPointers(), player,boss_);

	// ===== ボスの更新（常に）=====
	if (boss_) {
		boss_->Update(player->GetPosition());
	}

	// ロックオン対象クリア
	lockOnEnemies_.clear();
	// ================ 雑魚敵 ================
	for (Enemy* enemy : enemyManager_.GetEnemyPointers()) {
		if (enemy &&!enemy->IsDead()) {
			lockOnEnemies_.push_back(enemy);
		}
	}

	// 敵の更新
	enemyManager_.Update(player,bulletManager_);

	// 　雑魚全滅チェック
	if (!bossSpawned_ && enemyManager_.IsAllDead()) {

		boss_ = new Boss();
		boss_->Initialize(bossModel_, bossSwordModel_, rifleModel_, & camera_, {0.0f, 0.0f, 0.0f}, bulletManager_, player);

		bossSpawned_ = true;
	}

	  if (Input::GetInstance()->TriggerKey(DIK_L)) {
		if (lockOnIndex == -1) {
			// --- ロックオン開始処理 ---
			if (!lockOnEnemies_.empty()) {
				// 雑魚敵がいれば0番目をロック
				lockOnIndex = 0;
			} else if (boss_) {
				// 雑魚がいなくてボスがいれば、ボス専用の番号(999)にする
				lockOnIndex = 999;
			}
		} else {
			// すでにロックオン中なら解除
			lockOnIndex = -1;
		}
	}

	// 3. ロックオン状態の維持と更新（ここを安全にする）
	if (lockOnIndex != -1) {
		if (lockOnIndex == 999) {
			// --- ボスをロックオン中の場合 ---
			if (boss_ && !boss_->IsDead()) {
				player->SetLockOn(true);
				player->SetLockOnTarget(&boss_->GetPosition());
				followCamera_.SetLockOn(true);
				followCamera_.SetLockOnTarget(&boss_->GetPosition());
			} else {
				// ボスが死んだら解除
				lockOnIndex = -1;
			}
		} else {
			// --- 雑魚敵をロックオン中の場合 ---
			if (!lockOnEnemies_.empty()) {
				// 敵が死んで数が減った時、インデックスが範囲外にならないよう調整
				if (lockOnIndex >= (int)lockOnEnemies_.size()) {
					lockOnIndex = 0;
				}

				// 切り替え（DIK_Oキーなど）
				if (Input::GetInstance()->TriggerKey(DIK_O)) {
					lockOnIndex = (lockOnIndex + 1) % (int)lockOnEnemies_.size();
				}

				// 安全に確定したインデックスで要素にアクセス
				Enemy* targetEnemy = lockOnEnemies_[lockOnIndex];
				player->SetLockOn(true);
				player->SetLockOnEnemy(targetEnemy);
				player->SetLockOnTarget(&targetEnemy->GetPosition());

				followCamera_.SetLockOn(true);
				followCamera_.SetLockOnTarget(&targetEnemy->GetPosition());
			} else {
				// 雑魚敵が全滅したら、ボスがいればボスへ、いなければ解除
				if (boss_) {
					lockOnIndex = 999;
				} else {
					lockOnIndex = -1;
				}
			}
		}
	}

	// 4. ロックオン解除時の後処理
	if (lockOnIndex == -1) {
		player->SetLockOn(false);
		player->ClearLockOn();
		followCamera_.SetLockOn(false);
		player->SetLockOnTarget(nullptr);
	}

	// カメラ更新は最後
	followCamera_.SetTarget(&player->GetPosition());
	followCamera_.SetTargetRotation(&player->GetRotation());
	followCamera_.Update();

	// ================ UI ================
	attackAlert_.Update();


}

void GameScene::Draw() {

	// ===== 3D描画 =====
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());
	skydome_->Draw();
	field->Draw();
	bulletManager_->Draw(&camera_);
	player->Draw();
	enemyManager_.Draw();
	if (boss_) {
		boss_->Draw();
	}
	Model::PostDraw();

	// ===== 2D描画（Sprite）=====
	Sprite::PreDraw(dxCommon->GetCommandList());
	attackAlert_.Draw();
	player->DrawDamageEffect();

	if (boss_) {
		boss_->DrawUI();
	}

	Sprite::PostDraw();
}

GameScene::~GameScene() {

	// フィールドの解放
	delete field;
	field = nullptr;
	delete fieldModel_;
	fieldModel_ = nullptr;

	//天球
	delete skydome_;
	skydome_ = nullptr;

	// プレイヤーの解放
	delete player;
	player = nullptr;
	delete playerModel_;
	playerModel_ = nullptr;

	// 敵の解放
	delete enemyModel;
	enemyModel = nullptr;
	// 敵ダウンモデルを解放 (漏れを修正)
	delete enemyDown;
	enemyDown = nullptr;
	// ボスの解放
	delete boss_;
	boss_ = nullptr;
	delete bossModel_;
	bossModel_ = nullptr;
	delete bossSwordModel_;
	bossSwordModel_ = nullptr;

	// 弾の解放
	delete bulletManager_;
	bulletManager_ = nullptr;
	delete bulletModel;
	bulletModel = nullptr;

	delete rifleModel_;
	rifleModel_ = nullptr;
}
