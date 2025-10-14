#pragma once
#include "KamataEngine.h"
#include "MyMath.h"

class FollowCamera {
public:
	// 初期化
	void Initialize(Camera* camera);

	// 更新
	void Update();

	// プレイヤーを設定
	void SetTarget(const Vector3* target) { target_ = target; }
	void SetTargetRotation(const Vector3* targetRotation) { targetRotation_ = targetRotation; }

	// 注視オフセット（少し上を見せたい時など）
	void SetTargetOffset(const Vector3& offset) { targetOffset_ = offset; }

	// 距離
	void SetDistance(float distance) { distance_ = distance; }

	//ロックオンターゲット設定
	void SetLockOnTarget(Vector3* lockOnTarget) { lockOnTarget_ = lockOnTarget; }

	

private:
	// プレイヤー位置へのポインタ
	const Vector3* target_ = nullptr; 

	const Vector3* targetRotation_ = nullptr;

	// 敵ターゲット
	Vector3* lockOnTarget_ = nullptr;
	
	//ロックオン状態
	bool isLockOn_ = false;

	//注視店のオフセット
	Vector3 targetOffset_ = {0.0f, 2.0f, 0.0f};
	//プレイヤーとの距離
	float distance_ = 10.0f;
	//カメラ
	Camera *camera=nullptr;

	//カメラの位置
	Vector3 translation_ = {0, 0, 0};

	//カメラの回転
	Vector3 rotation_ = {0, 0, 0};

	//注視点
	Vector3 lookAt_ = {0, 0, 0};

	
};
