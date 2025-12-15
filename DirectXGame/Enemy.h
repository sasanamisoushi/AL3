#pragma once
#include "KamataEngine.h"
#include "MyMath.h"


class Player;
class BulletManager;
class Rifle;


class Enemy {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position, Player* player);

	// 更新
	void Update(BulletManager* bulletManager);

	

	// 描画
	void Draw();

	//--------------------ゲッター--------------------
	const KamataEngine::Vector3& GetPosition() const { return position_; }
	float GetRadius() const { return radius_; }
	void AddPosition(const KamataEngine::Vector3& v) { worldTransform_.translation_ += v; }

	//--------------------セッター--------------------
	void SetSurroundAngle(float angle) { surroundAngle_ = angle; }

	// あたり判定取得
	bool IsDead() const { return hp_ <= 0; }
	void Damage(int damage) { hp_ -= damage; }

	//弾の当たり判定
	bool HitChek(const KamataEngine::Vector3& point, float r);

	void ResolveCollisionWithPlayer();


	//デストラクタ
	~Enemy();

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
	float radius_ = 0.8f;

	//HP
	int hp_ = 100;

	//攻撃
	float attackRange_ = 5.0f; //サーベル距離
	float shootRange_ = 200.0f; // 射撃距離
	int attackCoolTime_ = 0;

	//移動速度
	float moveSpeed_ = 0.1f;

	//ラシアン
	float surroundAngle_ = 0.0f;

	//武器
	Rifle* rifle_ = nullptr;

	KamataEngine::Vector3 rifleOffset_ = {1.0f, 1.2f, -0.4f}; // 敵の手元
	KamataEngine::Vector3 muzzleOffset_ = {0.0f, -0.35f, 0.8f}; // 銃口
};
