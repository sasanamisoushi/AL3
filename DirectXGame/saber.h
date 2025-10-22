#pragma once
#include "KamataEngine.h"

using namespace KamataEngine;

class saber {
public:

	// 初期化
	void Initialize(Model* model, Camera* camera, const Vector3& position);

	// 更新
	void Update();

	// 描画
	void Draw();

	
	void SetPosition(const Vector3& position, const Vector3& rotation);

	//攻撃開始
	//void StartAttack(const Vector3& playerPos, const Vector3& forward);

	//攻撃中かどうか
	bool IsAttacking() const { return isAttacking_; }

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;

	//カメラ
	Camera* camera_ = nullptr;

	bool isAttacking_ = false;
	


};
