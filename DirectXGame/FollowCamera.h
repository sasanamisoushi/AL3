#pragma once
#include "KamataEngine.h"
#include "MyMath.h"

class FollowCamera {
public:
	// 初期化
	void Initialize(KamataEngine::Camera* camera);

	// 更新
	void Update();

	// プレイヤーを設定
	void SetTarget(const KamataEngine::Vector3* target) { target_ = target; }
	void SetTargetRotation(const KamataEngine::Vector3* targetRotation) { targetRotation_ = targetRotation; }

	// 注視オフセット（少し上を見せたい時など）
	void SetTargetOffset(const KamataEngine::Vector3& offset) { targetOffset_ = offset; }

	// 距離
	void SetDistance(float distance) { distance_ = distance; }

	//ロックオンターゲット設定
	void SetLockOnTarget(const KamataEngine::Vector3* lockOnTarget) { lockOnTarget_ = lockOnTarget; }

	void SetLockOn(bool flag) { isLockOn_ = flag;}

	bool GetLockOn() const { return isLockOn_; }

	KamataEngine::Vector3 GetRotation() const { return rotation_; }

	KamataEngine::Vector3 GetForward() const;
	KamataEngine::Vector3 GetRight() const;
	float GetYaw() const { return yaw_; };


	//シェイク
	void startShake(float power, int duration) { 
		isShaking_ = true;
		shakePower_ = power;
		shakeDuration_ = duration;
		shakeTimer_ = 0;
	}

	
	

private:
	// カメラ
	KamataEngine::Camera* camera = nullptr;

	// プレイヤー位置へのポインタ
	const KamataEngine::Vector3* target_ = nullptr; 

	const KamataEngine::Vector3* targetRotation_ = nullptr;

	// 敵ターゲット
	const KamataEngine::Vector3* lockOnTarget_ = nullptr;
	
	//ロックオン状態
	bool isLockOn_ = false;

	//注視店のオフセット
	KamataEngine::Vector3 targetOffset_ = {0.0f, 2.0f, 0.0f};

	//カメラの位置
	KamataEngine::Vector3 translation_ = {0, 0, 0};

	//カメラの回転
	KamataEngine::Vector3 rotation_ = {0, 0, 0};

	//注視点
	KamataEngine::Vector3 lookAt_ = {0, 0, 0};


	//カメラシェイク
	bool isShaking_ = false;
	int shakeTimer_ = 0;
	int shakeDuration_ = 0;

	float shakePower_ = 0.0f;
	KamataEngine::Vector3 shakeOffset_{};


	float yaw_ = 0.0f;   // 水平方向（左右）
	float pitch_ = 0.0f; // 垂直方向（上下）
	float distance_ = 10.0f; // プレイヤーとの距離
	float sensitivity_ = 0.0025f;
	
};
