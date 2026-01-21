#pragma once
#include "KamataEngine.h"
#include "Enemy.h"
#include "Player.h"
#include <unordered_set>
#include "MyMath.h"


class saber {
public:

	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	// 更新
	void Update();

	// 描画
	void Draw();

	
	void SetPosition(const KamataEngine::Vector3& position, const KamataEngine::Vector3& rotation);

	//攻撃開始
	void StartAttack();
	bool CheckHitPlayer(const Player* player);

	//攻撃中かどうか
	bool IsAttacking() const { return isAttacking_; }

	KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }

	float GetRadius() const { return 3.0f; }

	bool CheckHit(const Enemy* enemy);

	//デストラクタ
	~saber();

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	//カメラ
	KamataEngine::Camera* camera_ = nullptr;

	bool isDebugDraw_ = true;

	//攻撃モーション用
	std::unordered_set<const Enemy*> hitEnemies_;
	bool isAttacking_ = false;
	int attackTimer_ = 0;

	//何フレームで1振り
	const int attackDuration_ = 40;
	
	//通常の回転を保持
	KamataEngine::Vector3 baseRotation_ = {};

	std::unordered_set<const Player*> hitPlayers_;

};
