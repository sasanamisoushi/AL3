#include "BulletManager.h"

void BulletManager::Initialize(Model* model) { 
	model_ = model;
}

void BulletManager::Update() {

	for (auto it = bullets_.begin(); it != bullets_.end();) {
		Bullet* bullet = *it;
		bullet->Update();
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
	Bullet* newBullet = new Bullet();
	//速度倍率
	Vector3 velocity = direction * 1.0f;
	newBullet->Initialize(model_, position, velocity);
	bullets_.push_back(newBullet);

}


