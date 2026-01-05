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
	player->Initialize(playerModel_, &camera_, {0.0f, 0.5f, 0.0f});

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

	bossModel_ = Model::CreateFromOBJ("boss", true);
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
}

void GameScene::Update() {

	// プレイヤーの更新
	player->Update(bulletManager_, enemyManager_.GetEnemyPointers());

	// 弾の更新
	bulletManager_->Update(enemyManager_.GetEnemyPointers(), player);

	// ===== ボスの更新（常に）=====
	if (boss_) {
		boss_->Update(player->GetPosition());
	}

	// ロックオン対象クリア
	lockOnEnemies_.clear();
	// ================ 雑魚敵 ================
	for (Enemy* enemy : enemyManager_.GetEnemyPointers()) {
		if (!enemy->IsDead()) {
			lockOnEnemies_.push_back(enemy);
		}
	}

	// 敵の更新
	enemyManager_.Update(bulletManager_);

	// 　雑魚全滅チェック
	if (!bossSpawned_ && enemyManager_.IsAllDead()) {

		boss_ = new Boss();
		boss_->Initialize(bossModel_, bossSwordModel_, &camera_, {0.0f, 0.0f, 0.0f}, bulletManager_, player);

		bossSpawned_ = true;
	}

	  

	// 雑魚もボスもいなければロック不可
	if (lockOnEnemies_.empty() && boss_ == nullptr) {
		lockOnIndex = -1;
		player->SetLockOn(false);
		followCamera_.SetLockOn(false);
		return;
	}

	// Lキーでロックオン切り替え
	if (Input::GetInstance()->TriggerKey(DIK_L)) {
		if (lockOnIndex == -1) {
			// ロックオン開始
			lockOnIndex = 0;
			player->SetLockOn(true);
			followCamera_.SetLockOn(true);
		} else {
			// ロック解除
			lockOnIndex = -1;
			player->SetLockOn(false);
			followCamera_.SetLockOn(false);
		}
	}

	// --- ロックオン中に矢印で敵を切り替える ---
	if (lockOnIndex != -1) {

		// → 次の敵
		if (Input::GetInstance()->TriggerKey(DIK_RIGHT)) {
			lockOnIndex++;
			if (lockOnIndex >= lockOnEnemies_.size()) {
				lockOnIndex = 0; // ループ
			}
		}

		// ← 前の敵
		if (Input::GetInstance()->TriggerKey(DIK_LEFT)) {
			lockOnIndex--;
			if (lockOnIndex < 0) {
				lockOnIndex = (int)lockOnEnemies_.size() - 1; // ループ
			}
		}

		// ================ ボス ================
		// ボスの更新
		if (lockOnEnemies_.empty() && boss_) {

			// Lキーでボスをロックオン
			if (Input::GetInstance()->TriggerKey(DIK_L)) {
				player->SetLockOn(true);
				followCamera_.SetLockOn(true);
				player->SetLockOnTarget(&boss_->GetPosition());
				followCamera_.SetLockOnTarget(&boss_->GetPosition());
			}
			return;
		}

		if (lockOnIndex < 0 || lockOnIndex >= (int)lockOnEnemies_.size()) {
			lockOnIndex = -1;
			player->SetLockOn(false);
			followCamera_.SetLockOn(false);
			return;
		}

		// ロックオン対象をセットする
		player->SetLockOnTarget(&lockOnEnemies_[lockOnIndex]->GetPosition());
		followCamera_.SetLockOnTarget(&lockOnEnemies_[lockOnIndex]->GetPosition());
	}

	// ================ UI ================
	attackAlert_.Update();

#ifdef _DEBUG

	// キーを押したらクリア画面に
	if (Input::GetInstance()->TriggerKey(DIK_1)) {
		isFinish = true;
	}

	ImGui::Begin("Game Scene");
	ImGui::Text("R choiceWeapon");
	ImGui::Text("L lock on");
	ImGui::Text("E Reload");
	ImGui::Text("O Jump");
	ImGui::Text("SPACE attack");
	ImGui::End();

	ImGui::Begin("Game Scene Debug");

	// ===== ボス =====
	ImGui::Checkbox("Boss Spawned", &bossSpawned_);

	// ボス位置表示
	if (boss_) {
		Vector3 posB = boss_->GetPosition();
		ImGui::Text("Boss Pos: %.2f, %.2f, %.2f", posB.x, posB.y, posB.z);
	} else {
		ImGui::Text("Boss : nullptr");
	}

	// ===== 敵 =====
	static bool enemyEnable = true;
	ImGui::Checkbox("Enemy Enable", &enemyEnable);

	if (!enemyEnable) {
		enemyManager_.ClearEnemies();
	}

	ImGui::Text("Enemy Count: %d", (int)enemyManager_.GetEnemies().size());

	ImGui::End();
#endif
}

void GameScene::Draw() {

	// ===== 3D描画 =====
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());
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

	Sprite::PostDraw();
}

//void GameScene::ShowAttackAlert(const Vector3& enemyPos) {
	/*Vector3 playerPos = player->GetPosition();
	Vector3 toEnemy = Normalize(enemyPos - playerPos);

	Vector3 forward = player->GetForward();
	Vector3 right = player->GetRight();

	float f = Dot(forward, toEnemy);
	float r = Dot(right, toEnemy);

	AlertDir dir;

	if (f > 0.7f)
		dir = AlertDir::Front;
	else if (f < -0.7f)
		dir = AlertDir::Back;
	else if (r > 0.0f)
		dir = AlertDir::Right;
	else
		dir = AlertDir::Left;

	attackAlert_.Trigger(dir);*/
//}

GameScene::~GameScene() {

	// フィールドの解放
	delete field;
	field = nullptr;
	delete fieldModel_;
	fieldModel_ = nullptr;

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
}
