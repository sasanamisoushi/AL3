#pragma once
#include <vector>
#include "Enemy.h"


class EnemyManager {
public:
	//初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, Player* player);

	//更新
	void Update(BulletManager* bulletManager);

	//描画
	void Draw();

	std::vector<Enemy*>& GetEnemies()  { return enemies_; }

private:

	//指定数の敵を生成
	void SpawnEnemies(int count);

	std::vector<Enemy*> enemies_;
	KamataEngine::Model* model_;
	KamataEngine::Camera* camera_;
	Player* player_;

	int wave_ = 0;
};
