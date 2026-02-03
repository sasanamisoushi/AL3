#pragma once
#include "Field.h"
#include "KamataEngine.h"
#include "Player.h"
#include "MyMath.h"
#include "Enemy.h"
#include "FollowCamera.h"
#include "BulletManager.h"
#include "EnemyManager.h"
#include "AttackAlert.h"
#include "Skydome.h"
#include "Boss.h"
#include "ExplosionManager.h"




class GameScene {
public:
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// デスフラグのgetter
	bool IsFinished() const { return isGameClear_ || isGameOver_; }


	bool IsGameClear() const { return isGameClear_; }
	bool IsGameOver() const { return isGameOver_; }

	~GameScene();

private:
	// カメラ
	KamataEngine::Camera camera_;
	FollowCamera followCamera_;

	//フィールドモデル
	KamataEngine::Model* fieldModel_ = nullptr;
	
	//地面
	Field* field = nullptr;
	KamataEngine::WorldTransform worldTransformField_;

	//プレイヤー
	Player* player = nullptr;
	KamataEngine::Model* playerModel_ = nullptr;

	//敵のモデル
	KamataEngine::Model* enemyModel = nullptr;
	KamataEngine::Model* enemyDown = nullptr;

	//弾の管理
	BulletManager* bulletManager_=nullptr;
	KamataEngine::Model* bulletModel = nullptr;

	//敵の管理
	EnemyManager enemyManager_;


	// 終了フラグ
	bool isFinish = false;

	// ロックオン
	//std::vector<const Vector3*> lockOnTargets_;
	std::vector<Enemy*> lockOnEnemies_;
	int lockOnIndex = -1;

	//ボス
	Boss* boss_ = nullptr;
	bool bossSpawned_ = false;

	KamataEngine::Model* bossModel_ = nullptr;
	KamataEngine::Model* bossSwordModel_ = nullptr;
	KamataEngine::Model* rifleModel_ = nullptr;

	AttackAlert attackAlert_;

	 bool isGameClear_ = false;
	bool isGameOver_ = false;

	//天球
	Skydome* skydome_ = nullptr;
	KamataEngine::Model* skydomeModel_ = nullptr;

	ExplosionManager* explosionManager_ = nullptr;

	bool isPaused_ = false;                       // ポーズフラグ
	KamataEngine::Sprite* pauseSprite_ = nullptr; // ポーズ画面用の画像（あれば）
	uint32_t pauseTextureHandle_ = 0;
	
};
