#pragma once
#include "KamataEngine.h"
#include "MyMath.h"

using namespace KamataEngine;

class Rifle {
public:
	// 初期化
	void Initialize(Model* model, Camera* camera, const Vector3& position);

	// 更新
	void Update();

	// 描画
	void Draw();

	void SetPosition(const Vector3& position, const Vector3& rotation);

	//銃の前方向を取得
	Vector3 GetForwardVector() const;

	//銃口位置を取得
	Vector3 GetMuzzlePosition() const;

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;

	// カメラ
	Camera* camera_ = nullptr;
};
