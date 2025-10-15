#pragma once
#include "KamataEngine.h"
#include "MyMath.h"
#include "FollowCamera.h"
#include "Rifle.h"

using namespace KamataEngine;


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
	void Update();

	//移動用更新
	void UpdateMovement();

	// 描画
	void Draw();

	//解放
	~Player();

	void SetLockOnTarget(const Vector3* target) { lockOnTarget_ = target; }
	void SetLockOn(bool flag) { isLockOn_ = flag; }
	bool GetLockOn() const { return isLockOn_; }
	

private:

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;

	// カメラ
	Camera* camera_ = nullptr;

	//速度
	Vector3 velocity_ = {};

	//加速度
	const float KAcceleration = 0.3f;

	//減速率
	const float KAttenuation = 0.2f;

	//最大速度
	const float KLimitRunSpeed = 0.4f;

	// 前後左右向き
	Direction direction_ = Direction::kRight;

	//追従カメラ
	FollowCamera followCamera_;

	const Vector3* lockOnTarget_ = nullptr;
	bool isLockOn_ = false;

	//武器
	Rifle* rifle_ = nullptr;
	//銃を装備しているか
	bool choiceRifle_ = false;

};
