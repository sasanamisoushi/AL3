#include "Enemy.h"
#include "BulletManager.h"
#include "Player.h"
#include <numbers>

using namespace KamataEngine;

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position, Player* player) {
	// NULLポインタチェック
	assert(model);
	// モデル
	model_ = model;

	// カメラ
	camera_ = camera;

	// プレイヤー
	player_ = player;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float>;

	// 自身の座標を保持
	position_ = position;

	WorldTransformUpdate(worldTransform_);
}

void Enemy::Update(BulletManager* bulletManager) {

	if (hp_ <= 0) {
		return;
	}

	// 座標更新
	position_ = worldTransform_.translation_;

	// クールタイム（攻撃間隔）
	if (attackCoolTime_ > 0) {
		attackCoolTime_--;
	}

	//----------移動----------
	Vector3 playerPos = player_->GetPosition();
	Vector3 toPlayer = playerPos - position_;
	float dist = Length(toPlayer);

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


	

	playerPos = player_->GetPosition();
	dist = Length(playerPos - position_);

	if (dist < attackRange_) {
		// ----- 近距離（サーベル攻撃） -----
		if (attackCoolTime_ <= 0) {
			player_->Damage(10);  // 例：プレイヤーにダメージ
			attackCoolTime_ = 60; // 60フレーム攻撃間隔
		}
	} else {
		// ----- 遠距離（ライフル） -----
		if (attackCoolTime_ <= 0) {

			Vector3 dir = Normalize(playerPos - position_);

			bulletManager->Fire(position_, dir, Bullet::Owner::kEnemy); // プレイヤーに向けて発射

			attackCoolTime_ = 45; // 射撃間隔
		}
	}

	// --- プレイヤーとの衝突（重なり防止） ---
	ResolveCollisionWithPlayer();

	// 行列更新
	WorldTransformUpdate(worldTransform_);
#ifdef _DEBUG
	ImGui::Begin("Enemy");
	ImGui::Text("HP:%d", hp_);
	ImGui::End();
#endif
}

void Enemy::Draw() {
	if (hp_ <= 0) {
		return;
	}

	model_->Draw(worldTransform_, *camera_);
}

bool Enemy::HitChek(const Vector3& point, float r) {
	float dist = Length(position_ - point);
	return dist < (radius_ + r);
}

void Enemy::ResolveCollisionWithPlayer() {
	Vector3 diff = position_ - player_->GetPosition();
	float dist = Length(diff);
	float minDist = radius_ + player_->GetRadius();

	if (dist < minDist && dist > 0.001f) {

		Vector3 dir = diff / dist;
		float push = (minDist - dist);

		// 50%ずつ押し戻す
		worldTransform_.translation_ += dir * (push * 0.5f);
		player_->SetPosition(player_->GetPosition() - dir * (push * 0.5f));
	}
}
