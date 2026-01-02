#pragma once
#include "KamataEngine.h"
#include "WingSword.h"
#include "BulletManager.h"
#include "Rifle.h"
#include <vector>
#include <memory>


enum class BossPhase {
	DecideAction,  //次の行動を決める
	SwordRing,     //羽剣攻撃
	FunnelAttack,  //ファンネル攻撃
	MeleeAttack,   //近距離攻撃
	Cooldown,      //クールタイム
};

class Boss {
public:

	//初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Model* swordModel, KamataEngine::Camera* camera, const KamataEngine::Vector3& position, BulletManager* bulletManager);

	//更新
	void Update(const KamataEngine::Vector3& playerPos);

	//描画
	void Draw();

	const KamataEngine::Vector3& GetPosition() const { return worldTransform_.translation_; }

	//背中の位置を取得
	KamataEngine::Vector3 GetBackPosition() const;

	//距離で行動決定
	void DecideNextAction(const KamataEngine::Vector3& playerPos);
	// フェーズ変更
	void ChangePhase(BossPhase next);

	//羽攻撃更新
	void UpdateFunnelAttack(const KamataEngine::Vector3& playerPos);

	//近接攻撃
	void UpdateMeleeAttack(const KamataEngine::Vector3& playerPos);

	//羽剣リング攻撃更新
	void UpdateSwordRing();

	//羽剣リセット
	void ResetWingSwords();


private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	
	//モデル
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* swordModel_ = nullptr;
	//カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// 羽剣
	std::vector<std::unique_ptr<WingSword>> wingSwords_;

	// 行動管理
	int timer_ = 0;

	

	//ボスの行動フェーズ
	BossPhase phase_ = BossPhase::DecideAction;
	int phaseTimer_ = 0;

	//全ての羽剣が刺さっているかどうか
	bool AreAllSwordsStuck() const;

	//次に発射する剣
	int nextLaunchIndex_ = 0;

	// 何フレームおきに撃つか
	int launchInterval_ = 10;

	BulletManager* bulletManager_ = nullptr;

	
	float idealDistance_ = 20.0f; 

	//移動速度
	float moveSpeed_ = 0.1f;

	//射撃クールタイム
	int shootCoolTime_ = 0;

	//射撃間隔
	int shootInterval_ = 30;

	//銃
	Rifle* rifle_ = nullptr;

	KamataEngine::Vector3 rifleOffset_ = {0.0f, 1.5f, 0.8f};
};
