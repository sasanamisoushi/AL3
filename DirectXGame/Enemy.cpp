#include "Enemy.h"
#include "BulletManager.h"
#include "Player.h"
#include <numbers>
#include <cassert>

using namespace KamataEngine;

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position, Player* player, Model* downModel) {
	// NULLポインタチェック
	assert(model);
	// モデル
	model_ = model;
	downModel_ = downModel;
	normalModel_ = model;

	// カメラ
	camera_ = camera;

	// プレイヤー
	player_ = player;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float>;

	//surroundAngle_ = 0.0f;

	// 自身の座標を保持
	position_ = position;

	//銃
	rifle_ = new Rifle();
	rifle_->Initialize(Model::CreateFromOBJ("Raifl"), camera_, position);

	//剣
	saber_ = new saber();
	saber_->Initialize(Model::CreateFromOBJ("saber"), camera_, position);

	// スポーン時にタイマーをセット
	waitTimer_ = kWaitTime;

	WorldTransformUpdate(worldTransform_);
}

void Enemy::Update(Player* player,BulletManager* bulletManager) {

	// 座標更新
	position_ = worldTransform_.translation_;

	if (isDead_) {
		return;
	}

	// タイマーが残っている場合はカウントダウンして何もしない
	if (waitTimer_ > 0) {
		waitTimer_--;
		return;
	}

	//ダウン処理
	if (state_ == EnemyState::Down) {
		downTimer_--;

		if (downTimer_ <= 0) {
			Recover();
		}

		WorldTransformUpdate(worldTransform_);
		return;
	}
	

	// ----------プレイヤー情報----------
	Vector3 playerPos = player->GetPosition();
	Vector3 toPlayer = playerPos - position_;
	float dist = Length(toPlayer);

	// クールタイム（攻撃間隔）
	if (attackCoolTime_ > 0) {
		attackCoolTime_--;
	}
	
	//----------移動----------
	
	// プレイヤーからの距離
	float surroundRadius = 4.0f;

	Vector3 targetPos = playerPos + Vector3{std::cos(surroundAngle_) * surroundRadius, 0.0f, std::sin(surroundAngle_) * surroundRadius};

	Vector3 toTarget = targetPos - position_;
	float distToTarget = Length(toTarget);

	// 向き更新
	worldTransform_.rotation_.y = std::atan2(toPlayer.x, toPlayer.z) + std::numbers::pi_v<float> / 2.0f;

	if (distToTarget > 0.1f) {
		Vector3 moveDir = Normalize(toTarget);
		worldTransform_.translation_ += moveDir * moveSpeed_;
	}

	// 敵のY回転のみ反映
	Matrix4x4 rotY = MakeRotateYMatrix(worldTransform_.rotation_.y - std::numbers::pi_v<float> / 2.0f);
	Vector3 offset = TransformNormal(rifleOffset_, rotY);

	Vector3 riflePos = worldTransform_.translation_ + offset;
	Vector3 rifleRot = worldTransform_.rotation_;
	rifleRot.y -= std::numbers::pi_v<float> / 2.0f;

	// 銃の位置・向きを敵に追従
	rifle_->SetPosition(riflePos, rifleRot);

	rifle_->Update();

	saber_->SetPosition(riflePos, rifleRot);
	saber_->Update();

	playerPos = player->GetPosition();
	dist = Length(playerPos - worldTransform_.translation_);

	if (dist < attackRange_) {
		choiceSaber_ = true;
		// ----- 近距離（サーベル攻撃） -----
		if (attackCoolTime_ <= 0) {

			saber_->StartAttack();
			attackCoolTime_ = 60; // 60フレーム攻撃間隔
		}

		
	} else if (dist < shootRange_) {
		choiceSaber_ = false;
		// ----- 遠距離（ライフル） -----

		if (attackCoolTime_ <= 0) {
			// 射撃
			if (rifle_->GetAmmo() > 0) {
				// 敵がプレイヤーを向いている方向を作る
				Vector3 dir = Normalize(playerPos - worldTransform_.translation_);

				Vector3 muzzle = riflePos + TransformNormal(muzzleOffset_, rotY);

				bulletManager->Fire(muzzle, dir, Bullet::Owner::kEnemy);

				
				// ===== アラート発火 =====
				
				Vector3 toEnemy = worldTransform_.translation_ - playerPos;
				toEnemy.y = 0.0f;

				if (Length(toEnemy) > 0.001f) {
					toEnemy = Normalize(toEnemy);

					Vector3 camForward = player->GetFollowCamera()->GetForward();
					Vector3 camRight = player->GetFollowCamera()->GetRight();

					float forwardDot = KamataEngine::MathUtility::Dot(camForward, toEnemy);
					float rightDot = KamataEngine::MathUtility::Dot(camRight, toEnemy);

					AttackAlert::AlertDir alertDir;

					if (fabs(rightDot) > fabs(forwardDot)) {
						alertDir = (rightDot > 0) ? AttackAlert::AlertDir::Right : AttackAlert::AlertDir::Left;
					} else {
						alertDir = (forwardDot > 0) ? AttackAlert::AlertDir::Front : AttackAlert::AlertDir::Back;
					}

					player->GetAttackAlert()->Trigger(alertDir);
				}

				rifle_->ConsumeAmmo();
				attackCoolTime_ = 90;
			} else if (!rifle_->IsReloading()) {
				rifle_->Reload();
			}
		}
		
	}

	if (saber_->IsAttacking()) {
		if (saber_->CheckHitPlayer(player)) {
			player->Damage(35);
			// 重複ダメージを防ぎたい場合は、ここで saber_->FinishAttack() などで判定を止める
		}
	}

	// --- プレイヤーとの衝突（重なり防止） ---
	ResolveCollisionWithPlayer();

	stageBounds_->ClampToStage(worldTransform_.translation_, GetRadius());

	// 行列更新
	WorldTransformUpdate(worldTransform_);
#ifdef _DEBUG
	ImGui::Begin("Enemy");
	ImGui::Text("HP:%d", hp_);
	ImGui::End();
#endif
}

void Enemy::Draw() {
	if (isDead_) {
		return;
	}

	model_->Draw(worldTransform_, *camera_);
	if (!choiceSaber_) {
		if (rifle_) {
			rifle_->Draw();
		}
	}

	if (choiceSaber_) {
		if (saber_) {
			saber_->Draw();
		}
	}
}


void Enemy::Damage(int damage) {

	if (isDead_) {
		return;
	}

	hp_ -= damage;

	if (hp_ <= 0) {
	
		Die();
		return;
	}

	if (state_ == EnemyState::Down) {
		return;
	}

	
	// ダウン判定（死亡してない場合のみ）
	if (state_ != EnemyState::Down) {
		downCount_++;
		if (downCount_ >= downThreshold_) {
			Down();
		}
	}
}

void Enemy::Die() {
	isDead_ = true;
	justDied_ = true;

	// 行動停止
	state_ = EnemyState::Down;
	attackCoolTime_ = INT_MAX;

	// 表示を消す or 死亡モデルに切り替え
	// model_ = deadModel_; ← あれば

	// 銃を消す
	if (rifle_) {
		delete rifle_;
		rifle_ = nullptr;
	}

}


bool Enemy::HitChek(const Vector3& point, float r) {
	float dist = Length(position_ - point);
	return dist < (radius_ + r);
}

void Enemy::ResolveCollisionWithPlayer() {
	Vector3 currentPos = worldTransform_.translation_;
	Vector3 playerPos = player_->GetPosition();

	// 敵からプレイヤーへのベクトル
	Vector3 diff = currentPos - player_->GetPosition();
	diff.y = 0.0f;
	float dist = Length(diff);
	float minDist = radius_ + player_->GetRadius();

	// 接触している（距離 < 半径の和）かつ、完全に重なっていない（dist > 0）
	if (dist < minDist && dist > 0.001f) {

		// 正規化（方向）
		Vector3 dir = diff / dist;
		// 重なっている量
		float push = (minDist - dist);

		// 50%ずつ押し戻す
		worldTransform_.translation_.x += dir.x * (push * 0.5f);
		worldTransform_.translation_.z += dir.z * (push * 0.5f);
		// プレイヤーの位置をずらす
		Vector3 newPlayerPos = playerPos;
		newPlayerPos.x -= dir.x * (push * 0.5f);
		newPlayerPos.z -= dir.z * (push * 0.5f);
		player_->SetPosition(newPlayerPos);
	}
}

void Enemy::Down() {
	state_ = EnemyState::Down;

	downTimer_ = downTime_;

	// 攻撃完全停止
	attackCoolTime_ = INT_MAX;

	// ダウン用モデルに切り替え
	model_ = downModel_;

	// 必要なら回転や姿勢を変える
	worldTransform_.rotation_.x = std::numbers::pi_v<float> / 2.0f;

	// 地面にめり込まないように持ち上げる
	worldTransform_.translation_.y += downHeightOffset_;
}

void Enemy::Recover() {
	state_ = EnemyState::Alive;

	model_ = normalModel_; // 元のモデル
	worldTransform_.rotation_.x = 0.0f;

	  // ★ Yを地面に戻す
	worldTransform_.translation_.y -= downHeightOffset_;

	attackCoolTime_ = 60; // 起き上がり後の硬直
	downCount_ = 0;       // 再びダウン可能
}

Enemy::~Enemy() {
	delete rifle_;
	rifle_ = nullptr;

	delete saber_;
	saber_ = nullptr;

}
