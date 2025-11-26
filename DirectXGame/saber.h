#pragma once
#include "KamataEngine.h"
#include "Enemy.h"
#include <unordered_set>

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
	void StartAttack();

	//攻撃中かどうか
	bool IsAttacking() const { return isAttacking_; }

	Vector3 GetPosition() const { return worldTransform_.translation_; }

	float GetRadius() const { return 1.5f; }

	bool CheckHit(const Enemy* enemy);

	//デストラクタ
	~saber();

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;

	//カメラ
	Camera* camera_ = nullptr;

	//攻撃モーション用
	std::unordered_set<const Enemy*> hitEnemies_;
	bool isAttacking_ = false;
	int attackTimer_ = 0;

	//何フレームで1振り
	const int attackDuration_ = 40;
	
	//通常の回転を保持
	Vector3 baseRotation_ = {};

};
