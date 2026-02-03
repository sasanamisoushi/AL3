#include "WingSword.h"
#include "MyMath.h"
#include <numbers>


using namespace KamataEngine;

void WingSword::Initialize(Model *model) {
	model_ = model;
	worldTransform_.Initialize();
	worldTransform_.scale_ = {0.15f, 0.15f, 0.15f};
	WorldTransformUpdate(worldTransform_);
}


void WingSword::Update(Player* player) {
	switch (state_) {

	case WingSwordState::Standby:
		// Boss が位置をセットする
		break;
	case WingSwordState::Rising:
		worldTransform_.translation_.y += riseSpeed_;

		// 上昇しながら少し立てる
		worldTransform_.rotation_.x = -std::numbers::pi_v<float> * 0.25f;

		if (worldTransform_.translation_.y >= riseHeight_) {
			// 上昇完了 → プレイヤーへ飛ぶ
			Vector3 dir = Normalize(targetPos_ - worldTransform_.translation_);
			velocity_ = dir * 0.4f;
			state_ = WingSwordState::Flying;
		}
		break;
	case WingSwordState::Flying:
		worldTransform_.translation_ += velocity_;

		 // 飛行方向を向かせる
		Vector3 dir = Normalize(velocity_);

		worldTransform_.rotation_.y = std::atan2(dir.x, dir.z);
		worldTransform_.rotation_.x = -std::atan2(dir.y, std::sqrt(dir.x * dir.x + dir.z * dir.z));

		

		// 地面に刺さる
		if (worldTransform_.translation_.y <= 1.0f) {
			worldTransform_.translation_.y = 2.0f;
			state_ = WingSwordState::Stuck;

			// 刺さる向き
			worldTransform_.rotation_.x = -std::numbers::pi_v<float>/1.0f ;
			worldTransform_.rotation_.y = -std::atan2(dir.x, dir.z);
		}
		break;

	case WingSwordState::Stuck:
		// クールタイム減少
		if (damageCooldown_ > 0) {
			damageCooldown_--;
		}

		// プレイヤーとの位置の差分を計算
		Vector3 diff = player->GetPosition() - worldTransform_.translation_;

		// ★重要：高さ（Y）の差を 0 にして無視する！
		// これにより、剣が浮いていても埋まっていても、近くにいれば当たるようになります
		diff.y = 0.0f;

		// XとZだけの距離を計算
		float dist = Length(diff);
		// 使用 <= so touching at exact radius counts
		if (dist <= damageRadius_) {

			if (damageCooldown_ <= 0) {

				player->Damage(damage_);
				damageCooldown_ = 30; // 0.5秒ごとにダメージ
			}
		}
		break;
	}

	

	WorldTransformUpdate(worldTransform_);
}

void WingSword::Draw(Camera* camera) { model_->Draw(worldTransform_, *camera); }

void WingSword::Launch(const Vector3& targetPos) {
	

	Vector3 dir = Normalize(targetPos - worldTransform_.translation_);
	velocity_ = dir * 0.4f;
	targetPos_ = targetPos;
	state_ = WingSwordState::Rising;
}

void WingSword::SetStandbyParam(float angle, float radius) {
	standbyAngle_ = angle;
	standbyRadius_ = radius;
}

void WingSword::SetStandbyPosition(const Vector3& bossPos, float bossYaw) {

	if (state_ != WingSwordState::Standby) {
		return;
	}

	 
	// Boss の向きを基準にした円
	float angle = standbyAngle_ + bossYaw;

	// 円のオフセット（Boss背中基準）
	Vector3 offset{std::cos(angle) * standbyRadius_, 1.5f, std::sin(angle) * standbyRadius_};

	worldTransform_.translation_ = bossPos + offset;


	//外向きにする
	worldTransform_.rotation_.y = angle + std::numbers::pi_v<float>;

}

void WingSword::AddStandbyAngle(float delta) { standbyAngle_ += delta; }

void WingSword::ResetToStandby(const Vector3& centerPos, float bossYaw) {
	state_ = WingSwordState::Standby;

	// 速度リセット
	velocity_ = {0.0f, 0.0f, 0.0f};

	// Boss背中基準で待機位置を即座に再計算
	float angle = standbyAngle_ + bossYaw;

	Vector3 offset{std::cos(angle) * standbyRadius_, 1.5f, std::sin(angle) * standbyRadius_};

	worldTransform_.translation_ = centerPos + offset;

	// 外向き
	worldTransform_.rotation_.x = 0.0f;
	worldTransform_.rotation_.y = angle + std::numbers::pi_v<float>;
	worldTransform_.rotation_.z = 0.0f;

	WorldTransformUpdate(worldTransform_);
}

void WingSword::SetTargetPosition(const KamataEngine::Vector3& position) {
	// 剣の現在の座標を直接書き換える
	worldTransform_.translation_ = position;

	// 行列の更新を忘れずに行う
	WorldTransformUpdate(worldTransform_);
}

void WingSword::AddPosition(const KamataEngine::Vector3& velocity) {
	// 突撃の速度（velocity）分だけ座標を進める
	worldTransform_.translation_.x += velocity.x;
	worldTransform_.translation_.y += velocity.y;
	worldTransform_.translation_.z += velocity.z;

	// 行列の更新
	WorldTransformUpdate(worldTransform_);
}
