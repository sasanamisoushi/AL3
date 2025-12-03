#pragma once
#include "KamataEngine.h"

class Shield {
public:

	//初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	//更新
	void Update();

	//描画
	void Draw();

	//位置と回転の設定
	void SetPosition(const KamataEngine::Vector3& pos, const KamataEngine::Vector3& rot);

	//ガード判定
	bool IsGuarding() const { return !isBroken_ && isGuarding_; }
	void SetGuarding(bool guarding);

	//ダメージ処理
	void TakeDamage(int amount);
	
	//状態確認
	bool IsBroken() const { return isBroken_; }
	float GetRadius() const { return radius_; }
	KamataEngine::Vector3 GetPosition() const { return worldTransform_.translation_; }

	int GetShieldHp() const { return durability_; }


	// デストラクタの宣言を追加
	~Shield();

private:

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	//耐久値
	bool isGuarding_ = false;

	//壊れているか？
	bool isBroken_ = false;

	//盾の耐久値
	int durability_ = 100;

	//半径
	float radius_ = 1.0f;

};
