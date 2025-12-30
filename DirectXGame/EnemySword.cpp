#include "EnemySword.h"
#include "MyMath.h"
#include <numbers>

using namespace KamataEngine;

void EnemySword::Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position) {

	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.scale_ = {0.3f, 0.3f, 0.3f};
	worldTransform_.translation_ = position;

	// 敵用なので向きは適当でOK（後で SetPosition で上書き）
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};

	// 通常姿勢を保存
	baseRotation_ = worldTransform_.rotation_;

	WorldTransformUpdate(worldTransform_);

}

bool EnemySword::CheckHit(const Player* player) {
	if (!isAttacking_) {
		return false;
	}

	// プレイヤーとの距離
	float dist = Length(player->GetPosition() - worldTransform_.translation_);

	bool hit = dist < (player->GetRadius() + radius_);

	return hit;
}

void EnemySword::Update() { MeleeWeapon::Update(); }

void EnemySword::Draw(Camera* camera) { MeleeWeapon::Draw(camera); }
