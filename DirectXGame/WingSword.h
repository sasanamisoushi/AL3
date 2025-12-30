#pragma once
#include "KamataEngine.h"



enum class WingSwordState {
	Standby, // 背中で待機（羽）
	Rising,  //飛び上がり中
	Flying,  // 飛翔中
	Stuck    // 地面に刺さった
};

class WingSword {
public:

	 //WingSword() = default;

	//初期化
	void Initialize(KamataEngine::Model* model);
	//更新
	void Update();

	//描画
	void Draw(KamataEngine::Camera* camera);

	//発射
	void Launch( const KamataEngine::Vector3& targetPos);
	
	//刺さっているかどうかを取得
	bool IsStuck() const { return state_ == WingSwordState::Stuck; }
	
	// 待機位置の設定
	void SetStandbyParam(float angle, float radius);
	void SetStandbyPosition(const KamataEngine::Vector3& bossPos, float bossYaw);
	void AddStandbyAngle(float delta);
	
	void ResetToStandby(const KamataEngine::Vector3& centerPos, float bossYaw);

private:

	//ワールド情報の保持
	KamataEngine::WorldTransform worldTransform_;

	//モデル
	KamataEngine::Model* model_ = nullptr;

	//状態
	WingSwordState state_ = WingSwordState::Standby;

	//速度
	KamataEngine::Vector3 velocity_;

	//待機位置パラメータ
	float standbyAngle_ = 0.0f;
	float standbyRadius_ = 3.0f;

	KamataEngine::Vector3 targetPos_;
	float riseHeight_ = 5.0f;
	float riseSpeed_ = 0.2f;

	

	
};

