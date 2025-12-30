#pragma once
#include "KamataEngine.h"
#include "MeleeWeapon.h"
#include "Player.h"

class EnemySword : public MeleeWeapon {
public:
	void Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position);
	bool CheckHit(const Player* player);

	void Update() override;

	void Draw(KamataEngine::Camera* camera);

};
