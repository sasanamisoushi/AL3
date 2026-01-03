#pragma once
#include "KamataEngine.h"
#include "MyMath.h"
#include "FollowCamera.h"
#include "Rifle.h"
#include "saber.h"
#include "Enemy.h"
#include "Shield.h"
#include <vector>
#include <memory>


class Player {
public:

	//上下左右
	enum class Direction {
		kForward,
		kRetreat,
		kRight,
		kLeft,
	};


	// 初期化
	void Initialize(Model* model, Camera* camera, const Vector3& position);

	// 更新
	void Update(BulletManager* bulletManager, const std::vector<Enemy*>& enemies);

	//移動用更新
	void UpdateMovement();

	//重力と地面判定
	void ApplyGravity();

	// 描画
	void Draw();

	void DrawDamageEffect();

	//解放
	~Player();

	void SetLockOnTarget(const Vector3* target) { lockOnTarget_ = target; }
	void SetLockOn(bool flag) { isLockOn_ = flag; }
	void SetPosition(const Vector3& pos) {worldTransform_.translation_ = pos;WorldTransformUpdate(worldTransform_);} 
	bool GetLockOn() const { return isLockOn_; }
	Vector3 GetPosition() const { return worldTransform_.translation_; }

	// あたり判定
	float GetRadius() const { return radius_; }
	// ダメージ処理
	void Damage(int damage);

	Shield* GetShield() const { return shield_; }
	int GetHP() const { return hp_; }
	int GetMaxHP() const { return 100; }

	
	

private:

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;

	// カメラ
	Camera* camera_ = nullptr;

	//速度
	Vector3 velocity_ = {};

	//------------地上------------

	//加速度
	const float KAcceleration = 0.3f;
	//減速率
	const float KAttenuation = 0.2f;
	//最大速度
	const float KLimitRunSpeed = 0.4f;

	//------------空中------------
	//加速度
	const float KAirAcceleration = 0.1f;
	//減速率
	const float KAirAttenuation = 0.1f;
	//最大速度
	const float KLimitAirSpeed = 0.4f;

	// 前後左右向き
	Direction direction_ = Direction::kRight;

	//追従カメラ
	FollowCamera followCamera_;

	const Vector3* lockOnTarget_ = nullptr;
	bool isLockOn_ = false;

	//------------------武器------------------

	//武器(銃)
	Rifle* rifle_ = nullptr;
	//銃を装備しているか
	bool choiceRifle_ = false;

	//武器(剣)
	saber* saber_ = nullptr;
	//剣を装備しているか
	bool choiceSaber_ = true;

	//盾
	Shield* shield_ = nullptr;

	// 地面にいるかどうか
	bool isOnGround_ = false;

	// ジャンプ時間管理
	float jumpTime_ = 0.0f;
	const float kMaxJumpTime = 600.0f;

	//半径
	float radius_ = 1.0f;
	
	//hp
	int hp_ = 100;
	float maxHp_ = 100.0f;
	float currentHp_ = 100.0f;

	// HPバー
	KamataEngine::Sprite* spriteHPBack_ = nullptr;
	KamataEngine::Sprite* spriteHp_ = nullptr;
	uint32_t textureHandle_ = 0u;

	KamataEngine::Vector2 hpBarBaseSize_ = {876.0f, 67.0f};
	KamataEngine::Vector2 hpBarPos_ = {0.0f, 0.0f};

	//ダメージエフェクト
	float damageEffectTimer_ = 0.0f;
	const float damageEffectDuration_ = 0.3f;
	std::unique_ptr<KamataEngine::Sprite> damageSprite_ = nullptr;

	//無敵タイマー
	float invincibleTimer_ = 0.0f;
	const float invincibleTime_ = 0.6f;

};
