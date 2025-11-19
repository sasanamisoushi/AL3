#pragma once
#include "KamataEngine.h"
#include "MyMath.h"

class Bullet {
public:

	// 初期化
	void Initialize(Model* model, const Vector3& position,const Vector3& velocity);

	// 更新
	void Update();

	// 描画
	void Draw(Camera* camera);

	
	bool IsDead() const { return IsDead_; }

	Vector3 GetPosition() const { return worldTransform_.translation_; }
	float GetRadius() const { return 0.5f; }

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;

	//速度
	Vector3 velocity_ = {};

	bool IsDead_ = false;

	//生存タイマー
	float lifeTime_ = 0.0f;

	bool isAlive_ = true;

};
