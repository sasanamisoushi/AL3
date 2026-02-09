#include "Boss.h"
#include "MyMath.h"
#include "WingSword.h"
#include <numbers>
#include <algorithm>
#include "StageBounds.h"

using namespace KamataEngine;

#ifdef _DEBUG
static const char* BossPhaseToString(BossPhase phase) {
	switch (phase) {
	case BossPhase::DecideAction:
		return "DecideAction";
	case BossPhase::SwordRing:
		return "SwordRing";
	case BossPhase::FunnelAttack:
		return "FunnelAttack";
	case BossPhase::MeleeAttack:
		return "MeleeAttack";
	case BossPhase::Cooldown:
		return "Cooldown";
	default:
		return "Unknown";
	}
}
#endif

void Boss::Initialize(Model* model, Model* swordModel, Model* rifle, Camera* camera, const Vector3& position, BulletManager* bulletManager, Player* player) {
	model_ = model;
	swordModel_ = swordModel;
	camera_ = camera;
	bulletManager_ = bulletManager;
	player_ = player;
	// ワールドトランスフォーム初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};

	 // ===== Rifle生成 =====
	rifle_ = new Rifle();
	rifle_->Initialize(
	   rifle, 
	    camera_, position);


	WorldTransformUpdate(worldTransform_);

	// 羽剣生成
	const int wingSwordCount = 12;
	wingSwords_.clear();
	float centerOffset = std::numbers::pi_v<float>;
	float startAngle = -std::numbers::pi_v<float> * 0.75f + centerOffset;
	float endAngle = std::numbers::pi_v<float> * 0.75f + centerOffset;

	for (int i = 0; i < wingSwordCount; i++) {
		auto sword = std::make_unique<WingSword>();
		sword->Initialize(swordModel_);

		 float t = (float)i / (wingSwordCount - 1);
		float angle = std::lerp(startAngle, endAngle, t);
		sword->SetStandbyParam(angle, 3.5f); 

		wingSwords_.push_back(std::move(sword));
	}

	// HPバーのテクスチャ（Playerと同じものや、ボス用の赤い画像など）
	uint32_t whiteTex = TextureManager::Load("./Resources/white1x1.png");
	uint32_t hpBack = TextureManager::Load("./Resources/UI/BOSSHPBar.png");

	// 背景バーの生成 (少し大きめに作るか、色を変える)
	spriteHPBack_ = Sprite::Create(hpBack, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	spriteHPBack_->SetSize({1280.0f, 720.0f});


	// HP本体の生成 (赤色にするなど)
	spriteHp_ = Sprite::Create(whiteTex, hpBarPos_, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f});
	spriteHp_->SetSize(hpBarBaseSize_);
	
};


void Boss::Update(const Vector3& playerPos) {

	//HPが０なら何もしない
	if (currentHp_ <= 0) {
		return;
	}

	phaseTimer_++;

	switch (phase_) {
	case BossPhase::DecideAction:
		DecideNextAction(playerPos);
		break;
	case BossPhase::SwordRing:
		UpdateSwordRing();
		break;
	case BossPhase::FunnelAttack:
		UpdateFunnelAttack(playerPos);

		// 全部刺さったら近距離へ
		if (AreAllSwordsStuck()) {
			ChangePhase(BossPhase::MeleeAttack);
			phaseTimer_ = 0;
		}
		break;
	case BossPhase::MeleeAttack:
		UpdateMeleeAttack(playerPos);
		break;
	case BossPhase::DrillDash:
		UpdateDrillDash(playerPos);
		break;
	case BossPhase::Cooldown:
		if (phaseTimer_ > 60) {
			ChangePhase(BossPhase::DecideAction);
		}
		break;
	default:
		break;
	}


	// 向き
	if (phase_ != BossPhase::DrillDash) {
		Vector3 toPlayer = playerPos - worldTransform_.translation_;
		// 距離が近すぎると計算がおかしくなるのでチェック
		if (Length(toPlayer) > 0.1f) {
			Vector3 dir = Normalize(toPlayer);
			worldTransform_.rotation_.y = std::atan2(dir.x, dir.z);
		}
	}

	// BossのY回転のみ反映
	Matrix4x4 rotY = MakeRotateYMatrix(worldTransform_.rotation_.y);

	// Rifleの位置
	Vector3 riflePos = worldTransform_.translation_ + TransformNormal(rifleOffset_, rotY);

	// Rifleの回転
	Vector3 rifleRot = worldTransform_.rotation_;

	rifle_->SetPosition(riflePos, rifleRot);
	rifle_->Update();

	// ===== 全フェーズ共通：剣の更新 =====
	for (auto& sword : wingSwords_) {
		sword->Update(player_);
	}

	//float rotateSpeed = 0.01f;

	// HPが範囲外にならないようクランプ
	currentHp_ = std::clamp(currentHp_, 0.0f, maxHp_);

	// HPの割合を計算 (0.0 ～ 1.0)
	float hpRate = currentHp_ / maxHp_;

	// 横幅を割合に応じて変化させる
	spriteHp_->SetSize({hpBarBaseSize_.x * hpRate, hpBarBaseSize_.y});

	//timer_++;

	//// テスト：3秒後に攻撃
	//if (timer_ == 180) {
	//	float radius = 6.0f;

	//	for (int i = 0; i < wingSwords_.size(); i++) {
	//		float angle = (2.0f * std::numbers::pi_v<float> / wingSwords_.size()) * i;

	//		Vector3 target = playerPos + Vector3{std::cos(angle) * radius, 0.0f, std::sin(angle) * radius};

	//		wingSwords_[i]->Launch(target);
	//	}
	//}

	//フィールド外に出ないようにする
	StageBounds stageBounds;
	// ボスの半径（GetRadius() で 2.0f を返しているのでそれを使う）
	stageBounds.ClampToStage(worldTransform_.translation_, GetRadius());

	WorldTransformUpdate(worldTransform_);

	#ifdef _DEBUG
	ImGui::Begin("Boss Debug");

	ImGui::Text("Phase: %s", BossPhaseToString(phase_));

	ImGui::Separator();
	ImGui::Text("Phase Timer: %d", phaseTimer_);

	// デバッグ用：手動切り替え
	if (ImGui::Button("Sword")) {
		phase_ = BossPhase::SwordRing;
		phaseTimer_ = 0;
	}
	ImGui::SameLine();
	if (ImGui::Button("Funnel")) {
		phase_ = BossPhase::FunnelAttack;
		phaseTimer_ = 0;
	}
	ImGui::SameLine();
	if (ImGui::Button("Melee")) {
		phase_ = BossPhase::MeleeAttack;
		phaseTimer_ = 0;
	}

	ImGui::Text("HP:%d", currentHp_);

	ImGui::End();
	#endif
};

void Boss::Draw() {

	if (currentHp_ <= 0) {
		return;
	}

	model_->Draw(worldTransform_, *camera_);

	

	for (auto& sword : wingSwords_) {
		sword->Draw(camera_);
	}
}

void Boss::DrawUI() {
	if (spriteHPBack_) {
		spriteHPBack_->Draw();
	}
	if (spriteHp_) {
		spriteHp_->Draw();
	}
}

Vector3 Boss::GetBackPosition() const {
	Vector3 forward = {std::sin(worldTransform_.rotation_.y), 0.0f, std::cos(worldTransform_.rotation_.y)};

	return worldTransform_.translation_ - forward * 0.0f + Vector3{0.0f, 0.0f, 0.0f};
	;
}

void Boss::DecideNextAction(const KamataEngine::Vector3& playerPos) { 
	float dist = Length(playerPos - worldTransform_.translation_);
	if (dist < 5.0f) {
		ChangePhase(BossPhase::MeleeAttack);
	} else {
		ChangePhase(BossPhase::FunnelAttack);
	}

}

void Boss::ChangePhase(BossPhase next) { 
	// ===== フェーズ終了時の処理（必要なら） =====
	switch (phase_) {
	case BossPhase::FunnelAttack:
		// 何かあれば
		break;
	default:
		break;
	}

	phase_ = next;
	phaseTimer_ = 0;

	

	// 剣を戻すのは SwordRing に入る時だけ
	if (phase_ == BossPhase::FunnelAttack || phase_ == BossPhase::SwordRing) {
		ResetWingSwords();    // 剣を地面から回収し、IsStuckをfalseにする
		nextLaunchIndex_ = 0; // 発射カウントを最初に戻す
	}

}
void Boss::UpdateFunnelAttack(const KamataEngine::Vector3& playerPos) {

	Vector3 backPos = GetBackPosition();
	float bossYaw = worldTransform_.rotation_.y;

	// 待機位置更新
	for (auto& sword : wingSwords_) {
		sword->SetStandbyPosition(backPos, bossYaw);
		//sword->Update(player);

		 
	}

	// ===== 一本ずつ発射 =====
	if (phaseTimer_ >= 60) {

		// 一定間隔ごとに発射
		if ((phaseTimer_ - 60) % launchInterval_ == 0) {

			if (nextLaunchIndex_ < wingSwords_.size()) {

				wingSwords_[nextLaunchIndex_]->Launch(playerPos);
				nextLaunchIndex_++;
			}
		}
	}

	// 全て発射し終わり、なおかつ全剣が刺さったら終了
	if (nextLaunchIndex_ >= wingSwords_.size() && AreAllSwordsStuck()) {
		ChangePhase(BossPhase::Cooldown);
		nextLaunchIndex_ = 0;
	}
}
void Boss::UpdateMeleeAttack(const KamataEngine::Vector3& playerPos) {

	Vector3 toPlayer = playerPos - worldTransform_.translation_;
	float dist = Length(toPlayer);
	Vector3 dir = Normalize(toPlayer);

	// ===== 移動制御 =====
	if (dist < idealDistance_) {
		// 近すぎ → 離れる
		worldTransform_.translation_ -= dir * moveSpeed_;
	} else {
		// 遠い → 近づく
		worldTransform_.translation_ += dir * moveSpeed_;
	}

	// ===== 向きは常にプレイヤー =====
	worldTransform_.rotation_.y = std::atan2(dir.x, dir.z);

	// ===== 射撃クールタイム =====
	if (shootCoolTime_ > 0) {
		shootCoolTime_--;
	}

	// ===== 射撃 =====
	if (shootCoolTime_ <= 0) {

		if (rifle_->GetAmmo() > 0) {
			rifle_->Fire(bulletManager_, Bullet::Owner::kEnemy);
			shootCoolTime_ = shootInterval_;
		} else if (!rifle_->IsReloading()) {
			rifle_->Reload();
		}
	}

	float hitRadius = 5.0f;

	Vector3 diff = playerPos - worldTransform_.translation_;

	dist = Length(diff);

	if (dist < hitRadius + player_->GetRadius()) {
		player_->Damage(20);
	}

	

	WorldTransformUpdate(worldTransform_);

	// フェーズ終了条件（例）
	if (phaseTimer_ > 240) {
		ChangePhase(BossPhase::DrillDash);
	}
}

void Boss::UpdateSwordRing() {

	float rotateSpeed = 0.02f;
	for (auto& sword : wingSwords_) {
	
		sword->AddStandbyAngle(rotateSpeed);
		//sword->Update(player_);
	}

	if (phaseTimer_ > 180) {
	
		ChangePhase(BossPhase::DecideAction);
	}

}

void Boss::ResetWingSwords() {
	Vector3 backPos = GetBackPosition();
	float bossYaw = worldTransform_.rotation_.y;

	for (auto& sword : wingSwords_) {
		sword->ResetToStandby(backPos, bossYaw);
	}
}

void Boss::UpdateDrillDash(const Vector3& playerPos) {
	const int chargeTime = 60; // 1秒間のタメ

	if (phaseTimer_ < chargeTime) {
		// --- 1. タメ：剣を前方に円錐形に並べる ---
		Vector3 bossForward = {std::sin(worldTransform_.rotation_.y), 0.0f, std::cos(worldTransform_.rotation_.y)};
		Vector3 drillCenter = worldTransform_.translation_ + bossForward * 3.0f; // ボスの少し前

		for (int i = 0; i < wingSwords_.size(); i++) {
			float angle = (2.0f * std::numbers::pi_v<float> / wingSwords_.size()) * i + (phaseTimer_ * 0.5f); // 高速回転
			float radius = 1.5f * (1.0f - (float)i / wingSwords_.size());                                     // 先端にいくほど細く（ドリル形状）

			Vector3 offset = {std::cos(angle) * radius, std::sin(angle) * radius, 0.0f};
			// ボスの向きに合わせてオフセットを回転させて配置
			wingSwords_[i]->SetTargetPosition(drillCenter + TransformNormal(offset, MakeRotateYMatrix(worldTransform_.rotation_.y)));
		}

		// プレイヤーの方向をロックし続ける
		Vector3 toPlayer = Normalize(playerPos - worldTransform_.translation_);
		worldTransform_.rotation_.y = std::atan2(toPlayer.x, toPlayer.z);

		// 突撃方向を確定させる
		dashVelocity_ = toPlayer * drillSpeed_;

	} else if (phaseTimer_ < chargeTime + 90) {
		// --- 2. 突撃：確定した方向に一直線 ---
		worldTransform_.translation_ += dashVelocity_;

		// 突撃中は剣も一緒に移動させる
		for (auto& sword : wingSwords_) {
			sword->AddPosition(dashVelocity_);
		}

		// プレイヤーへの当たり判定（既存の MeleeAttack の流用）
		float dist = Length(playerPos - worldTransform_.translation_);
		if (dist < 4.0f) {       // ドリルなので判定を少し大きく
			player_->Damage(30); // 強力なダメージ
		}
	} else {
		// 終了
		ChangePhase(BossPhase::Cooldown);
	}
}

void Boss::Damage(int damage) {
	if (currentHp_ <= 0) {
		return;
	}

	currentHp_ -= damage;
	if (currentHp_ < 0) {
		currentHp_ = 0;
	}
}

Boss::Boss() = default;

Boss::~Boss() {
	rifle_ = nullptr;

};

bool Boss::AreAllSwordsStuck() const {
	for (const auto& sword : wingSwords_) {
		if (!sword->IsStuck()) {
			return false;
		}
	}
	return true;
};

