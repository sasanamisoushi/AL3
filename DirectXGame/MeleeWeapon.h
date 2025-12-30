#pragma once
#include "KamataEngine.h"
class MeleeWeapon {
public:
	virtual void Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position);
	virtual void Update();
	virtual void Draw(KamataEngine::Camera* camera);

	void StartAttack();
	bool IsAttacking() const { return isAttacking_; }

	float GetRadius() const { return radius_; }
	KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }

protected:
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Model* model_ = nullptr;

	bool isAttacking_ = false;
	int attackTimer_ = 0;
	int attackDuration_ = 40;

	float radius_ = 1.5f;
	KamataEngine::Vector3 baseRotation_{};

};
