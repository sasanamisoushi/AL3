#pragma once
#include "KamataEngine.h"
#include "BulletManager.h"
#include "Rifle.h"
#include "Player.h"
#include <vector>
#include <memory>

class WingSword;

enum class BossPhase {
	DecideAction,  //次の行動を決める
	SwordRing,     //羽剣攻撃
	FunnelAttack,  //ファンネル攻撃
	MeleeAttack,   //近距離攻撃
	DrillDash,    // ドリル突撃
	Cooldown,      //クールタイム
};

class Boss {
public:

	//初期化
	void Initialize(
	    KamataEngine::Model* model, KamataEngine::Model* swordModel, KamataEngine::Model* rifle, KamataEngine::Camera* camera, const KamataEngine::Vector3& position, BulletManager* bulletManager,
	    Player* player);

	//更新
	void Update(const KamataEngine::Vector3& playerPos);

	//描画
	void Draw();

	void DrawUI();

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

	void UpdateDrillDash(const Vector3& playerPos);

	int GetHP() const { return int(currentHp_); }
	bool IsDead() const { return currentHp_ <= 0; }

	void Damage(int damage);

	float GetRadius() const { return 2.0f; }

	//コンストラクタ
	Boss();
	//デストラクタ
	~Boss();

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

	KamataEngine::Vector3 rifleOffset_ = {0.0f, -0.35f, 0.8f};

	Player* player_ = nullptr;

	

	// --- HPバー用 ---
	KamataEngine::Sprite* spriteHPBack_ = nullptr; // 背景
	KamataEngine::Sprite* spriteHp_ = nullptr;     // HP本体

	float maxHp_ = 300.0f; // ボスの最大HP
	float currentHp_ = 300.0f; // ボスの現在HP

	// HPバーの基準サイズ
	KamataEngine::Vector2 hpBarBaseSize_ = {850.0f, 65.0f};
	// 表示位置
	KamataEngine::Vector2 hpBarPos_ = {225.0f, 48.0f}; // 画面中央上部

	float drillSpeed_ = 1.5f; // 突撃速度
	KamataEngine::Vector3 dashVelocity_ = {0, 0, 0}; // 突撃の移動量
};
