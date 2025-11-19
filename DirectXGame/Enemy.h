#pragma once
#include "KamataEngine.h"
#include "MyMath.h"

using namespace KamataEngine;


class Enemy {
public:
	// 初期化
	void Initialize(Model* model, Camera* camera, const Vector3& position);

	// 更新
	void Update();

	

	// 描画
	void Draw();

	const Vector3& GetPosition() const { return position_; }
	float GetRadius() const { return radius_; }

	// あたり判定取得
	bool IsDead() const { return hp_ <= 0; }
	void Damage(int damage) { hp_ -= damage; }

	//弾の当たり判定
	bool HitChek(const Vector3& point, float r);

private:

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;

	// カメラ
	Camera* camera_ = nullptr;

	Vector3 position_;

	//あたり判定
	float radius_ = 0.5f;

	//HP
	int hp_ = 100;
};
