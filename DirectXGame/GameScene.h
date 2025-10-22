#pragma once
#include "Field.h"
#include "KamataEngine.h"
#include "Player.h"
#include "MyMath.h"
#include "Enemy.h"
#include "FollowCamera.h"
#include "BulletManager.h"

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
	
	//地面
	Field* field = nullptr;
	WorldTransform worldTransformField_;

	//プレイヤー
	Player* player = nullptr;
	Model* playerModel_ = nullptr;

	
	//敵
	Enemy* enemy = nullptr;
	Model* enemyModel = nullptr;

	//弾の管理
	BulletManager* bulletManager_=nullptr;
	Model* bulletModel = nullptr;


	// 終了フラグ
	bool isFinish = false;
};
