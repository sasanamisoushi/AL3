#pragma once
#include "AttackAlert.h"
#include "KamataEngine.h"
#include "MeleeWeapon.h"
#include "MyMath.h"
#include "StageBounds.h"
#include <functional>

class Player;
class BulletManager;
class Rifle;
class saber;

enum class EnemyState {
	Alive, // 通常
	Down   // ダウン
};

class Enemy {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position, Player* player, KamataEngine::Model* downModel);

	// 更新
	void Update(Player* player, BulletManager* bulletManager);

	// 描画
	void Draw();

	//--------------------ゲッター--------------------
	const KamataEngine::Vector3& GetPosition() const { return worldTransform_.translation_; }
	float GetRadius() const { return radius_; }
	void AddPosition(const KamataEngine::Vector3& v) { worldTransform_.translation_ += v; }
	int GetHp() const { return hp_; }
	int GetMaxHp() const { return maxHp_; }
	// KamataEngine::Vector3 GetWorldPosition() const { return worldTransform_.translation_; }

	//--------------------セッター--------------------
	void SetSurroundAngle(float angle) { surroundAngle_ = angle; }

	// あたり判定取得
	bool IsDead() const { return isDead_; }
	void Damage(int damage);
	void Die();

	// 弾の当たり判定
	bool HitChek(const KamataEngine::Vector3& point, float r);

	void ResolveCollisionWithPlayer();

	// ダウン
	void Down();

	// ダウンからの復帰
	void Recover();

	// デストラクタ
	~Enemy();

	bool IsDown() const { return state_ == EnemyState::Down; }

	void SetOnAttack(std::function<void(const KamataEngine::Vector3&)> func) { onAttack_ = func; }

	bool IsJustDied() {
		if (justDied_) {
			justDied_ = false; // 1回反応したらOFFにする
			return true;
		}
		return false;
	}

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* downModel_ = nullptr;
	KamataEngine::Model* normalModel_ = nullptr;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// プレイヤーへの参照
	Player* player_ = nullptr;

	KamataEngine::Vector3 position_;

	// あたり判定
	float radius_ = 0.8f;

	// HP
	int hp_ = 100;
	int maxHp_ = 100;

	// 攻撃
	float attackRange_ = 5.0f;  // サーベル距離
	float shootRange_ = 200.0f; // 射撃距離
	int attackCoolTime_ = 0;

	// 移動速度
	float moveSpeed_ = 0.1f;

	// ラシアン
	float surroundAngle_ = 0.0f;

	// 武器
	Rifle* rifle_ = nullptr;
	saber* saber_ = nullptr;

	KamataEngine::Vector3 rifleOffset_ = {1.0f, 1.2f, -0.4f};   // 敵の手元
	KamataEngine::Vector3 muzzleOffset_ = {0.0f, -0.35f, 0.8f}; // 銃口

	EnemyState state_ = EnemyState::Alive;
	int downCount_ = 0;     // 攻撃を受けた回数
	int downThreshold_ = 3; // 何回でダウンするか
	int downTimer_ = 0;     // ダウン用タイマー
	const int downTime_ = 180;
	float downHeightOffset_ = 0.5f;

	// 死亡フラグ
	bool isDead_ = false;

	StageBounds* stageBounds_ = nullptr;

	// 攻撃時のコールバック関数
	std::function<void(const KamataEngine::Vector3&)> onAttack_;

	AttackAlert* attackAlert_ = nullptr;

	// 待機タイマー
	int waitTimer_ = 0; // スポーン後の待機タイマー
	const int kWaitTime = 60;

	bool choiceSaber_ = true;

	bool justDied_ = false;
};
