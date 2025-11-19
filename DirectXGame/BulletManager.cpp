#include "BulletManager.h"

void BulletManager::Initialize(Model* model) { 
	model_ = model;
}

void BulletManager::Update(std::vector<Enemy*>& enemies) {

	//弾のリストをループして更新
	for (auto it = bullets_.begin(); it != bullets_.end();) {
		Bullet* bullet = *it;
		bullet->Update();

		bool erased = false;

		// --- 敵との当たり判定 ---
		if (!bullet->IsDead()) {
			for (Enemy* enemy : enemies) {

				 // 敵の弾なら敵との判定をスキップ
				if (bullet->GetOwner() == Bullet::Owner::kEnemy) {
					continue;
				}

				if (enemy->IsDead()) {
					continue;
				}
				
				// 弾同士の当たり判定
				float dist = Length(enemy->GetPosition() - bullet->GetPosition());
				if (dist < (enemy->GetRadius() + bullet->GetRadius())) {
					// 当たった場合の処理
					enemy->Damage(10); // 例: 敵にダメージを与える
					// 弾を削除
					delete bullet;
					it = bullets_.erase(it);
					erased = true;
					break; // この弾はもう削除されたので、次の弾へ
				}
			}
		
		}
		// 弾が削除済みなら次へ
		if (erased) {
			continue;
		}

		// --- 寿命で消える場合 ---
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

void BulletManager::Fire(const Vector3& position, const Vector3& direction, Bullet::Owner owner) { 

	//新しい弾を生成
	Bullet* newBullet = new Bullet();

	//発射方向を速度として設定
	Vector3 velocity = direction * 1.0f;

	newBullet->Initialize(model_, position, velocity);
	newBullet->SetOwner(owner);

	bullets_.push_back(newBullet);

}


