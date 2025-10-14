#pragma once
#include "KamataEngine.h"
#include "MyMath.h"
#include "FollowCamera.h"

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

	

};
