#include "BulletManager.h"

void BulletManager::Initialize(Model* model) { 
	model_ = model;
}

void BulletManager::Update() {

	//弾のリストをループして更新
	for (auto it = bullets_.begin(); it != bullets_.end();) {
		Bullet* bullet = *it;
		bullet->Update();

		//死亡していたら削除
		if (bullet->IsDead()) {
			delete bullet;
			it = bullets_.erase(it);
		} else {
			++it;
		}
	}

}

void BulletManager::Draw(Camera* camera) {

	for (Bullet* bullet : bullets_) {
		bullet->Draw(camera);
	}


}

void BulletManager::Fire(const Vector3& position, const Vector3& direction) { 

	//新しい弾を生成
	Bullet* newBullet = new Bullet();

	//発射方向を速度として設定
	Vector3 velocity = direction * 1.0f;

	newBullet->Initialize(model_, position, velocity);
	bullets_.push_back(newBullet);

}


