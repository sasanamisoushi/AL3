#include "saber.h"
#include "MyMath.h"
#include <numbers>
#include <cmath>
#include "MeleeWeapon.h"

using namespace KamataEngine;

void saber::Initialize(Model* model, Camera* camera, const Vector3& position) {
	model_ = model;
	camera_ = camera;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = {0.3f, 0.3f, 0.3f};
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float>;

	// 通常の回転を記憶
	baseRotation_ = worldTransform_.rotation_;

	WorldTransformUpdate(worldTransform_);
}

void saber::Update() {

	// 攻撃中ならモーションを再生
	if (isAttacking_) {

		// 経過フレームを進める
		attackTimer_++;

		// 攻撃モーションの進行率
		float t = static_cast<float>(attackTimer_) / attackDuration_;

		// サイン波で滑らかな動きを作る
		float swing = std::sin(t * std::numbers::pi_v<float>);

		//-------------モーション設定-------------

		// X軸方向縦に振る
		worldTransform_.rotation_.x = baseRotation_.x + swing * 1.7f;

		// Z軸方向に右から左に傾ける
		worldTransform_.rotation_.z = baseRotation_.z - swing * 0.4f;

		// Y軸に少し回転を加える
		worldTransform_.rotation_.y = baseRotation_.y - swing * 0.4f;

		WorldTransformUpdate(worldTransform_);

		// 攻撃が終わったらリセット
		if (attackTimer_ >= attackDuration_) {

			isAttacking_ = false;

			// 元の角度に戻す
			worldTransform_.rotation_ = baseRotation_;
			WorldTransformUpdate(worldTransform_);
		}
	}
}

void saber::Draw() {
	model_->Draw(worldTransform_, *camera_);

}

void saber::SetPosition(const Vector3& position, const Vector3& rotation) {
	worldTransform_.translation_ = position;
	worldTransform_.rotation_ = rotation;

	// 攻撃終了時に戻る角度を更新
	baseRotation_ = rotation;

	WorldTransformUpdate(worldTransform_);
}



void saber::StartAttack() {

	// 攻撃中出なければ攻撃開始
	if (!isAttacking_) {

		isAttacking_ = true;
		attackTimer_ = 0;

		// ヒットした敵リストをクリア
		hitEnemies_.clear();
	}
}

bool saber::CheckHitPlayer(const Player* player) {
	if (!isAttacking_) {
		return false;
	}

	// すでに当たっていたら無効
	if (hitPlayers_.count(player) > 0) {
		return false;
	}

	// プレイヤー中心との距離
	float dist = Length(player->GetPosition() - GetPosition());
	bool hit = dist < (player->GetRadius() + GetRadius());

	if (hit) {
		hitPlayers_.insert(player); // 記録
		return true;
	}

	return false;
}

bool saber::CheckHit(const Enemy* enemy)  {
	if (!isAttacking_) {
		return false;
	}

	// すでに当たっていればダメージ無し
	if (hitEnemies_.count(enemy) > 0) {
		return false;
	}

	 // 距離判定
	Vector3 toEnemyVec = enemy->GetPosition() - GetPosition();
	float dist = Length(toEnemyVec);
	if (dist > enemy->GetRadius() + GetRadius()) {
		return false;
	}

	// 前方判定
	Vector3 saberForward = {std::sin(worldTransform_.rotation_.y), 0.0f, std::cos(worldTransform_.rotation_.y)};

	Vector3 toEnemy = Normalize(Vector3{toEnemyVec.x, 0.0f, toEnemyVec.z});
	float dot = Dot(saberForward, toEnemy);

	const float kAttackAngleCos = std::cos(std::numbers::pi_v<float> / 4.0f);
	if (dot < kAttackAngleCos) {
		return false;
	}

	hitEnemies_.insert(enemy);

	return true;
}

saber::~saber() { delete model_; }
