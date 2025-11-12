#pragma once
#include "KamataEngine.h"
#include "MyMath.h"
#include "BulletManager.h"

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

	//弾の発射処理
	void Fire(BulletManager* bulletManager);

	//弾のリロード
	void Reload();
	int GetAmmo() const { return ammo_; }
   

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;

	// カメラ
	Camera* camera_ = nullptr;

	//弾数
	int ammo_ = 10;
	
	//最大弾数
	int maxAmmo_ = 10;

	//リロード時間
	float reloadTime_ = 2.0f;
	bool isReloading_ = false;
	float reloadTimer_ = 0.0f;
};
