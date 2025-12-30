#include "EnemyManager.h"
#include <random>
#include <numbers>

using namespace KamataEngine;

void EnemyManager::Initialize(Model* model, Camera* camera, Player* player, Model* downModel) {
	model_ = model;
	camera_ = camera;
	player_ = player;
	downModel_ = downModel;


	wave_ = 1;
	SpawnEnemies(3); // 初期は3体
}

void EnemyManager::Update(BulletManager* bulletManager) {
	// 敵更新
	for (auto& enemy : enemies_) {
		enemy->Update(bulletManager);
	}

	// 敵同士の衝突
	ResolveEnemyCollisions();

	// 死んでいる敵を削除（メモリを解放してからベクタから消す）
	for (auto it = enemies_.begin(); it != enemies_.end();) {
		if ((*it)->IsDead()) {
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
	for (auto& enemy : enemies_) {
		enemy->Draw();
	}
}

EnemyManager::~EnemyManager() {
	//enemies_.clear();
}

std::vector<Enemy*> EnemyManager::GetEnemyPointers() const {
	std::vector<Enemy*> result;
	for (const auto& e : enemies_) {
		result.push_back(e.get());
	}
	return result;
}

void EnemyManager::ResolveEnemyCollisions() {
	for (size_t i = 0; i < enemies_.size(); i++) {
		for (size_t j = i + 1; j < enemies_.size(); j++) {

			Enemy* a = enemies_[i].get();
			Enemy* b = enemies_[j].get();

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

bool EnemyManager::IsAllDead() const {
	for (const auto& enemy : enemies_) {
		if (!enemy->IsDead()) {
			return false;
		}
	}
	return true;
}

void EnemyManager::SpawnEnemies(int count) {

	// 乱数生成器
	static std::random_device rd;
	static std::mt19937 gen(rd());

	 // 出現範囲
	std::uniform_real_distribution<float> distX(-20.0f, 20.0f);
	std::uniform_real_distribution<float> distZ(-30.0f, 20.0f);

	
	
	for (int i = 0; i < count; i++) {
		auto enemy = std::make_unique<Enemy>();
		Vector3 pos = {distX(gen), 0, distZ(gen)}; // 適当に配置
		//円周上に配置
		float angle = (2.0f * std::numbers::pi_v<float> * i) / count;
		std::uniform_real_distribution<float> offset(-0.3f, 0.3f);

		enemy->SetSurroundAngle(angle + offset(gen));
		enemy->Initialize(model_, camera_, pos, player_, downModel_);
		enemies_.push_back(std::move(enemy));
	}
}

void EnemyManager::ClearEnemies() { enemies_.clear(); }
