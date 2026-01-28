#pragma once
#include "KamataEngine.h"

class Explosion {
public:

	//初期化
	void Initialize(KamataEngine::Model* model,const KamataEngine::Vector3& position, KamataEngine::Camera* camera);

	//更新
	void Update();

	//描画
	void Draw();

	bool IsFinished() const { return isFinished_; }

	// 座標を取得するゲッターを追加
	const KamataEngine::Vector3& GetPosition() const { return position_; }

private:

	KamataEngine::Vector3 position_;
	
	//モデル
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::WorldTransform worldTransform_;

	KamataEngine::Camera* camera_ = nullptr;

	int timer_ = 0;
	int duration_ = 30; //演出時間
	bool isFinished_ = false;


};
