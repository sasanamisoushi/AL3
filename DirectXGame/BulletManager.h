#pragma once
#include "KamataEngine.h"
#include "Bullet.h"
#include <list>
#include "Enemy.h"

class BulletManager {
public:

	//初期化
	void Initialize(Model* model);

	//更新
	void Update(std::vector<Enemy*>& enemies);

	//描画
	void Draw(Camera* camera);

	//発射処理
	void Fire(const Vector3& position, const Vector3& direction, Bullet::Owner owner);

	

private:
	std::list<Bullet*> bullets_;
	Model* model_ = nullptr;


};
