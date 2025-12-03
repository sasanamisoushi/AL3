#include "EnemyManager.h"

using namespace KamataEngine;

void EnemyManager::Initialize(Model* model, Camera* camera, Player* player) {
	model_ = model;
	camera_ = camera;
	player_ = player;

	wave_ = 1;
	SpawnEnemies(3); // 初期は3体
}

void EnemyManager::Update(BulletManager* bulletManager) {
	// 敵更新
	for (Enemy* enemy : enemies_) {
		enemy->Update(bulletManager);
	}

	// 死んでいる敵を削除
	enemies_.erase(std::remove_if(enemies_.begin(), enemies_.end(), [](Enemy* e) { return e->IsDead(); }), enemies_.end());

	// 全滅したら次Waveへ
	if (enemies_.empty()) {
		wave_++;

		if (wave_ == 2) {
			SpawnEnemies(5); // 2wave目：5体
		} else if (wave_ == 3) {
			SpawnEnemies(8); // 3wave目：8体など
		}
	}
}

void EnemyManager::Draw() {
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}
}

EnemyManager::~EnemyManager() {

// 管理している全ての敵オブジェクトを解放
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();
}

void EnemyManager::SpawnEnemies(int count) {
	for (int i = 0; i < count; i++) {
		Enemy* enemy = new Enemy();
		Vector3 pos = {float(i * 3), 0, -10}; // 適当に配置
		enemy->Initialize(model_, camera_, pos, player_);
		enemies_.push_back(enemy);
	}
}
