#pragma once
#include "KamataEngine.h"
#include "MyMath.h"


class Player;
class BulletManager;


class Enemy {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position, Player* player);

	// 更新
	void Update(BulletManager* bulletManager);

	

	// 描画
	void Draw();

	const KamataEngine::Vector3& GetPosition() const { return position_; }
	float GetRadius() const { return radius_; }

	// あたり判定取得
	bool IsDead() const { return hp_ <= 0; }
	void Damage(int damage) { hp_ -= damage; }

	//弾の当たり判定
	bool HitChek(const KamataEngine::Vector3& point, float r);

	void ResolveCollisionWithPlayer();

private:

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// プレイヤーへの参照
	Player* player_ = nullptr;

	KamataEngine::Vector3 position_;

	//あたり判定
	float radius_ = 0.5f;

	//HP
	int hp_ = 100;

	//攻撃
	float attackRange_ = 3.0f; //サーベル距離
	float shootRange_ = 20.0f; // 射撃距離
	int attackCoolTime_ = 0;
};
