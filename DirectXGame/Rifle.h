#pragma once
#include "KamataEngine.h"
#include "MyMath.h"
#include "BulletManager.h"


class Rifle {
public:
	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

	// 更新
	void Update();

	// 描画
	void Draw();

	void SetPosition(const KamataEngine::Vector3& position, const KamataEngine::Vector3& rotation);

	//銃の前方向を取得
	KamataEngine::Vector3 GetForwardVector() const;

	//銃口位置を取得
	KamataEngine::Vector3 GetMuzzlePosition() const;

	//弾の発射処理
	void Fire(BulletManager* bulletManager);

	//弾のリロード
	void Reload();
	int GetAmmo() const { return ammo_; }
	int GetReserveAmmo() const { return reserveAmmo_; }

	//デストラクタ
	~Rifle();

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	//弾数
	int ammo_ = 10;
	
	//最大弾数
	int maxAmmo_ = 10;

	//総弾数
	int reserveAmmo_ = 200;

	//リロード時間
	float reloadTime_ = 1.5f;
	bool isReloading_ = false;
	float reloadTimer_ = 0.0f;
};