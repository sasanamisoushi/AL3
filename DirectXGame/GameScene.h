#pragma once
#include "Field.h"
#include "KamataEngine.h"
#include "Player.h"
#include "MyMath.h"
#include "Enemy.h"
#include "FollowCamera.h"

using namespace KamataEngine;

class GameScene {
public:
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// デスフラグのgetter
	bool IsFinished() const { return isFinish; }

	~GameScene();

private:
	// カメラ
	Camera camera_;
	FollowCamera followCamera_;

	//フィールドモデル
	Model *fieldModel_ = nullptr;
	
	Field* field = nullptr;
	WorldTransform worldTransformField_;

	Player* player = nullptr;
	Model* playerModel_ = nullptr;

	Enemy* enemy = nullptr;
	Model* enemyModel = nullptr;


	// 終了フラグ
	bool isFinish = false;
};
