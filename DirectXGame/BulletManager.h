#pragma once
#include "KamataEngine.h"
#include "Bullet.h"
#include <list>
#include "Enemy.h"


class BulletManager {
public:

	//初期化
	void Initialize(KamataEngine::Model* model);

	//更新
	void Update(std::vector<Enemy*>& enemies, Player* player);

	//描画
	void Draw(KamataEngine::Camera* camera);

	//発射処理
	void Fire(const KamataEngine::Vector3& position, const KamataEngine::Vector3& direction, Bullet::Owner owner);

	//デストラクタ
	~BulletManager();

private:
	std::list<Bullet*> bullets_;
	KamataEngine::Model* model_ = nullptr;


};
