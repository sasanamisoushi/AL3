#include "EnemyManager.h"
#include <random>
#include <numbers>

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

	// 敵同士の衝突
	ResolveEnemyCollisions();

	// 死んでいる敵を削除（メモリを解放してからベクタから消す）
	for (auto it = enemies_.begin(); it != enemies_.end();) {
		Enemy* e = *it;
		if (e->IsDead()) {
			delete e;
			it = enemies_.erase(it);
		} else {
			++it;
		}
	}

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

void EnemyManager::ResolveEnemyCollisions() {
	for (size_t i = 0; i < enemies_.size(); i++) {
		for (size_t j = i + 1; j < enemies_.size(); j++) {

			Enemy* a = enemies_[i];
			Enemy* b = enemies_[j];

			Vector3 diff = a->GetPosition() - b->GetPosition();
			float dist = Length(diff);
			float minDist = a->GetRadius() + b->GetRadius();

			if (dist < minDist && dist > 0.0001f) {
				Vector3 dir = diff / dist;
				float push = (minDist - dist) * 0.5f;

				a->AddPosition(dir * push);
				b->AddPosition(-dir * push);
			}
		}
	}
}

void EnemyManager::SpawnEnemies(int count) {

	// 乱数生成器
	static std::random_device rd;
	static std::mt19937 gen(rd());

	 // 出現範囲
	std::uniform_real_distribution<float> distX(-20.0f, 20.0f);
	std::uniform_real_distribution<float> distZ(-30.0f, 20.0f);

	
	
	for (int i = 0; i < count; i++) {
		Enemy* enemy = new Enemy();
		Vector3 pos = {distX(gen), 0, distZ(gen)}; // 適当に配置
		//円周上に配置
		float angle = (2.0f * std::numbers::pi_v<float> * i) / count;
		std::uniform_real_distribution<float> offset(-0.3f, 0.3f);

		enemy->SetSurroundAngle(angle + offset(gen));
		enemy->Initialize(model_, camera_, pos, player_);
		enemies_.push_back(enemy);
	}
}
