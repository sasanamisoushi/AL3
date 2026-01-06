#pragma once
#include <vector>
#include <memory>
#include "Enemy.h"
#include "KamataEngine.h"

class BulletManager;

class EnemyManager {
public:
	//初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, Player* player, KamataEngine::Model* downModel);

	//更新
	void Update(Player* player, BulletManager* bulletManager);

	//描画
	void Draw();

	// デストラクタを追加
	~EnemyManager();

	std::vector<std::unique_ptr<Enemy>>& GetEnemies() { return enemies_; }

	std::vector<Enemy*> GetEnemyPointers() const;

	//敵同士の衝突
	void ResolveEnemyCollisions();

	bool IsAllDead() const;

	void ClearEnemies();

private:

	//指定数の敵を生成
	void SpawnEnemies(int count);

	std::vector<std::unique_ptr<Enemy>> enemies_;
	KamataEngine::Model* model_;
	KamataEngine::Model* downModel_;
	KamataEngine::Camera* camera_;
	Player* player_;

	int wave_ = 0;
};
