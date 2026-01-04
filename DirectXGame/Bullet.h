#pragma once
#include "KamataEngine.h"
#include "MyMath.h"

class Bullet {
public:

	enum class Owner {
		kPlayer,
		kEnemy,	
	};

	// 初期化
	void Initialize(KamataEngine::Model* model, const KamataEngine::Vector3& position, const KamataEngine::Vector3& velocity);

	// 更新
	void Update();

	// 描画
	void Draw(KamataEngine::Camera* camera);

	
	bool IsDead() const { return IsDead_; }

	//ゲッター
	KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }
	float GetRadius() const { return 0.5f; }
	Owner GetOwner() const { return owner_; }

	//セッター
	void SetOwner(Owner owner) { owner_ = owner; }

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	//速度
	KamataEngine::Vector3 velocity_ = {};

	bool IsDead_ = false;

	//生存タイマー
	float lifeTime_ = 0.0f;

	bool isAlive_ = true;

	//デフォプレイヤー
	Owner owner_ = Owner::kPlayer;

};
