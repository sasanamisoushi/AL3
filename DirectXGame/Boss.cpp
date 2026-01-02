#include "Boss.h"
#include "MyMath.h"
#include <numbers>

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

void Boss::Initialize(Model* model, Model* swordModel, Camera* camera, const Vector3& position, BulletManager* bulletManager) {
	model_ = model;
	swordModel_ = swordModel;
	camera_ = camera;
	bulletManager_ = bulletManager;
	// ワールドトランスフォーム初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {1.5f, 1.5f, 1.5f};

	 // ===== Rifle生成 =====
	rifle_ = new Rifle();
	rifle_->Initialize(
	    Model::CreateFromOBJ("Raifl"), // Enemyと同じ
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
};


void Boss::Update(const Vector3& playerPos) {

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
	case BossPhase::Cooldown:
		if (phaseTimer_ > 60) {
			ChangePhase(BossPhase::DecideAction);
		}
		break;
	default:
		break;
	}


	// 向き
	Vector3 toPlayer = playerPos - worldTransform_.translation_;
	Vector3 dir = Normalize(toPlayer);
	worldTransform_.rotation_.y = std::atan2(dir.x, dir.z);

	// BossのY回転のみ反映
	Matrix4x4 rotY = MakeRotateYMatrix(worldTransform_.rotation_.y);

	// Rifleの位置
	Vector3 riflePos = worldTransform_.translation_ + TransformNormal(rifleOffset_, rotY);

	// Rifleの回転
	Vector3 rifleRot = worldTransform_.rotation_;

	rifle_->SetPosition(riflePos, rifleRot);
	rifle_->Update();

	//float rotateSpeed = 0.01f;

	

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

	ImGui::End();
	#endif
};

void Boss::Draw() {
	model_->Draw(worldTransform_, *camera_);

	 if (rifle_) {
		rifle_->Draw();
	}

	for (auto& sword : wingSwords_) {
		sword->Draw(camera_);
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

	// ===== フェーズ開始時の処理 =====
	switch (phase_) {
	case BossPhase::SwordRing:
		ResetWingSwords(); 
		break;

	case BossPhase::FunnelAttack:
		ResetWingSwords(); 
		break;

	case BossPhase::DecideAction:
		ResetWingSwords(); 
		break;

	default:
		break;
	}

	if (next != BossPhase::FunnelAttack) {
		nextLaunchIndex_ = 0;
	}

}
void Boss::UpdateFunnelAttack(const KamataEngine::Vector3& playerPos) {

	Vector3 backPos = GetBackPosition();
	float bossYaw = worldTransform_.rotation_.y;

	// 待機位置更新
	for (auto& sword : wingSwords_) {
		sword->SetStandbyPosition(backPos, bossYaw);
		sword->Update();
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
			rifle_->Fire(bulletManager_);
			shootCoolTime_ = shootInterval_;
		} else if (!rifle_->IsReloading()) {
			rifle_->Reload();
		}
	}

	WorldTransformUpdate(worldTransform_);

	// フェーズ終了条件（例）
	if (phaseTimer_ > 240) {
		ChangePhase(BossPhase::Cooldown);
	}
}

void Boss::UpdateSwordRing() {

	float rotateSpeed = 0.02f;
	for (auto& sword : wingSwords_) {
	
		sword->AddStandbyAngle(rotateSpeed);
		sword->Update();
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

bool Boss::AreAllSwordsStuck() const {
	for (const auto& sword : wingSwords_) {
		if (!sword->IsStuck()) {
			return false;
		}
	}
	return true;
};

