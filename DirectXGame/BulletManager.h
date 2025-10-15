#pragma once
#include "KamataEngine.h"
#include "Bullet.h"
#include <list>

class BulletManager {
public:

	//初期化
	void Initialize(Model* model);

	//更新
	void Update();

	//描画
	void Draw(Camera* camera);

	//発射処理
	void Fire(const Vector3& position, const Vector3& direction);

private:
	std::list<Bullet*> bullets_;
	Model* model_ = nullptr;


};
