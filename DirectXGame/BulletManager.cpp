#include "BulletManager.h"
#include "Player.h"

using namespace KamataEngine;

void BulletManager::Initialize(Model* model) { model_ = model; }


void BulletManager::Update(const std::vector<Enemy*>& enemies, Player* player) {

	// 弾のリストをループして更新
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

		// --- プレイヤーとの当たり判定 ---
		// 敵弾だけプレイヤーに当たる
		if (bullet->GetOwner() == Bullet::Owner::kEnemy) {

			// 盾が存在し、壊れておらず、ガード中の場合のみ判定
			if (player->GetShield() && player->GetShield()->IsGuarding() && !player->GetShield()->IsBroken()) {

				float distShield = Length(player->GetShield()->GetPosition() - bullet->GetPosition());

				if (distShield < (player->GetShield()->GetRadius() + bullet->GetRadius())) {

					player->GetShield()->TakeDamage(10); // 盾がダメージを受ける

					// 弾を消す
					delete bullet;
					it = bullets_.erase(it);
					erased = true;
					continue;
				}
			}

			// --- プレイヤーに当たる ---
			float dist = Length(player->GetPosition() - bullet->GetPosition());
			if (dist < (player->GetRadius() + bullet->GetRadius())) {

				player->Damage(10); // 好きなダメージ値に変更可能

				delete bullet;
				it = bullets_.erase(it);
				erased = true;
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

	// 新しい弾を生成
	Bullet* newBullet = new Bullet();

	// 発射方向を速度として設定
	Vector3 velocity = direction * 1.0f;

	newBullet->Initialize(model_, position, velocity);
	newBullet->SetOwner(owner);

	bullets_.push_back(newBullet);
}

BulletManager::~BulletManager() {

	// コンテナ内の全てのBullet*を解放する
	for (Bullet* bullet : bullets_) {
		delete bullet;
	}

	// コンテナ自体をクリア
	bullets_.clear();
}
