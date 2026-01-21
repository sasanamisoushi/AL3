#include "Player.h"

#include <algorithm>
#include <cassert>
#include <numbers>

using namespace KamataEngine;

void Player::Initialize(Model* model, Model* rifle, Camera* camera, const Vector3& position) {
	// NULLポインタチェック
	assert(model);
	// モデル
	model_ = model;

	// カメラ
	camera_ = camera;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = followCamera_.GetYaw();

	// 銃
	rifle_ = new Rifle();
	rifle_->Initialize(rifle, camera_, position);
	choiceRifle_ = false;

	// 剣
	saber_ = new saber();
	saber_->Initialize(Model::CreateFromOBJ("saber"), camera_, position);
	choiceSaber_ = true;

	// 盾
	shield_ = new Shield();
	shield_->Initialize(Model::CreateFromOBJ("shield"), camera_, position);

	// 追従カメラの初期化
	followCamera_.Initialize(camera_);
	followCamera_.SetTarget(&worldTransform_.translation_);
	followCamera_.SetTargetOffset({0.0f, 4.0f, 0.0f});
	followCamera_.SetDistance(10.0f);

	textureHandle_ = TextureManager::Load("white1x1.png");

	// 被弾用赤スプライトを一度だけ生成
	damageSprite_.reset(Sprite::Create(
	    TextureManager::Load("./Resources/white1x1.png"), // 真っ白な1x1画像
	    {0.0f, 0.0f}));
	damageSprite_->SetSize({1280.0f, 720.0f}); // 画面サイズ
	damageSprite_->SetAnchorPoint({0.0f, 0.0f});
	damageSprite_->SetColor({1, 0, 0, 0}); // 最初は透明

	// HPバー背景
	spriteHPBack_ = Sprite::Create(TextureManager::Load("./Resources/UI/HP,StaminaB.png"), hpBarPos_, {1, 1, 1, 1}, {0.0f, 0.0f});

	// HPバー本体
	spriteHp_ = Sprite::Create(textureHandle_, {193.0f, 624.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f});
	spriteHp_->SetSize(hpBarBaseSize_);

	// UI
	attackAlert_.Initialize();

	bulletNumber_ = Sprite::Create(TextureManager::Load("./Resources/UI/Number of bullets.png"), {0.0f, 0.0f}, {1, 1, 1, 1}, {0.0f, 0.0f});

	// 弾数のUI
	ammoUI_.Initialize();

	WorldTransformUpdate(worldTransform_);
}

void Player::Update(BulletManager* bulletManager, const std::vector<Enemy*>& enemies) {

	// ========重力と地面判定========
	ApplyGravity();

	// ========移動========
	if (isLockOn_) {
		UpdateLockOnMovement(enemies); // ロックオン中の移動
	} else {
		UpdateFreeMovement(); // 自由移動
	}

	// ========武器========
	UpdateWeapons(bulletManager, enemies);

	// ========盾========
	UpdateShield();

	// ========UI========
	UpdateStatusUI();

	// ========カメラ========

	// カメラの回転をプレイヤーのY回転に合わせる
	followCamera_.SetTarget(&worldTransform_.translation_);

	followCamera_.SetTargetRotation(&worldTransform_.rotation_);
	// カメラ更新
	followCamera_.Update();

	// ===== ステージ外に出ないようにする =====
	stageBounds_->ClampToStage(worldTransform_.translation_, GetRadius());
	WorldTransformUpdate(worldTransform_);

#ifdef _DEBUG

	// 地面判定
	ImGui::Begin("Player Info");
	ImGui::Text("isOnGround: %s", isOnGround_ ? "true" : "false");
	ImGui::Text("Shield HP: %d ", shield_->GetShieldHp());
	ImGui::Text("Shield Broken: %s", shield_->IsBroken() ? "true" : "false");
	ImGui::Text("Guarding: %s", shield_->IsGuarding() ? "true" : "false");
	// ジャンプ時間
	ImGui::Text("Jump Time: %.2f", jumpTime_);

	// 銃
	if (choiceRifle_) {

		ImGui::Begin("Rifle");
		ImGui::Text("Ammo: %d", rifle_->GetAmmo());
		ImGui::Text("Reserve: %d", rifle_->GetReserveAmmo());
	}
#endif
}

void Player::UpdateMovement() {

	// 地上と空中によって定数を切り分ける
	const float currentAcc = isOnGround_ ? KAcceleration : KAirAcceleration;
	const float currentAtt = isOnGround_ ? KAttenuation : KAirAttenuation;
	const float currentLimit = isOnGround_ ? KLimitRunSpeed : KLimitAirSpeed;

	if (Input::GetInstance()->PushKey(DIK_W) || Input::GetInstance()->PushKey(DIK_S) || Input::GetInstance()->PushKey(DIK_A) || Input::GetInstance()->PushKey(DIK_D)) {

		Vector3 acceleration = {};
		if (Input::GetInstance()->PushKey(DIK_W)) {

			// 上入力中の下入力
			if (velocity_.x > 0.0f) {

				// 速度と逆方向に入力中は急ブレーキ
				velocity_.x *= (1.0f - currentAcc);
			}

			acceleration.x -= currentAcc / 60.0f;

		} else if (Input::GetInstance()->PushKey(DIK_S)) {

			// 下入力中の上入力
			if (velocity_.x < 0.0f) {

				// 速度と逆方向に入力中は急ブレーキ
				velocity_.x *= (1.0f - currentAcc);
			}

			acceleration.x += currentAcc / 60.0f;
		} else {
			// 入力なしだと減衰
			velocity_.x *= (1.0f - currentAtt);
		}

		if (Input::GetInstance()->PushKey(DIK_A)) {

			// 左入力中の右入力
			if (velocity_.z > 0.0f) {

				// 速度と逆方向に入力中は急ブレーキ
				velocity_.z *= (1.0f - currentAcc);
			}

			acceleration.z -= currentAcc / 60.0f;

		} else if (Input::GetInstance()->PushKey(DIK_D)) {

			// 右入力中の左入力
			if (velocity_.z < 0.0f) {

				// 速度と逆方向に入力中は急ブレーキ
				velocity_.z *= (1.0f - currentAcc);
			}

			acceleration.z += currentAcc / 60.0f;
		} else {
			// 入力なしだと減衰
			velocity_.z *= (1.0f - currentAtt);
		}

		// 加速/減速
		velocity_ += acceleration;

		// 最大速度制限
		velocity_.x = std::clamp(velocity_.x, -currentLimit, currentLimit);
		velocity_.z = std::clamp(velocity_.z, -currentLimit, currentLimit);

		// カメラの向きに合わせて移動方向を変更
		Matrix4x4 matRotY = MakeRotateYMatrix(worldTransform_.rotation_.y);
		Vector3 move = TransformNormal(velocity_, matRotY);

		// ワールド座標に反映
		worldTransform_.translation_ += move;
		WorldTransformUpdate(worldTransform_);
	}
}

void Player::ApplyGravity() {
	// 重力加速度
	const float kGravity = -0.3f;
	// 地面高さ
	const float kGroundHeight = 0.0f;
	// 押している間の上昇速度
	const float kJumpPower = 0.005f;
	// 落下速度の上限
	const float kMaxFallSpeed = -0.2f;

	auto* input = Input::GetInstance();

	// ====== ジャンプ ======
	// if (input->TriggerKey(DIK_O) && isOnGround_) {
	//	isOnGround_ = false;
	//	velocity_.y = 0.0f; // 上昇開始時は初速0から
	//}

	// ===== 押している間は上昇 =====
	if (!isOnGround_) {
		if (input->PushKey(DIK_O) && jumpTime_ < kMaxJumpTime) {
			velocity_.y += kJumpPower; // 押してる間は上昇
			jumpTime_ += 1.5f;
		} else {

			if (velocity_.y > 0.0f) {
				velocity_.y *= 0.95f; // 速度を緩やかに減速させる
			}
			velocity_.y += kGravity; // 離したら重力が働く
			jumpTime_ -= 1.0f;
		}
	}

	if (isOnGround_) {
		jumpTime_ = 0.0f;
	}

	// 落下速度制限
	velocity_.y = std::max<float>(velocity_.y, kMaxFallSpeed);

	// Y方向の移動
	worldTransform_.translation_.y += velocity_.y;

	// 地面判定
	if (worldTransform_.translation_.y < kGroundHeight) {
		worldTransform_.translation_.y = kGroundHeight;
		velocity_.y = 0.0f;
		isOnGround_ = true;
	}

	WorldTransformUpdate(worldTransform_);
}

void Player::UpdateLockOnMovement(const std::vector<Enemy*>&) {
	if (!lockOnTarget_) {
		return;
	}

	Input* input = Input::GetInstance();

	//敵方向
	Vector3 toEnemy = *lockOnTarget_ - worldTransform_.translation_;
	toEnemy.y = 0.0f;

	// float dist = Length(toEnemy);
	Vector3 dirToEnemy = Normalize(toEnemy);

	// ロックオン中は即座に敵を向く
	float targetY = std::atan2(dirToEnemy.x, dirToEnemy.z) + std::numbers::pi_v<float> / 2.0f;
	worldTransform_.rotation_.y = targetY;

	Vector3 move = {0, 0, 0};

	// 前後（距離調整）
	if (input->PushKey(DIK_W)) {
		move += dirToEnemy;
	}
	if (input->PushKey(DIK_S)) {
		move -= dirToEnemy;
	}

	// 左右（円運動）
	Vector3 right = {dirToEnemy.z, 0.0f, -dirToEnemy.x};

	if (input->PushKey(DIK_D)) {
		move += right;
	}
	if (input->PushKey(DIK_A)) {
		move -= right;
	}

	if (Length(move) > 0.0f) {
		move = Normalize(move) * KLimitRunSpeed;
		worldTransform_.translation_ += move;
	}
}

void Player::UpdateFreeMovement() {
	// プレイヤー移動
	UpdateMovement();
}

void Player::UpdateWeapons(BulletManager* bulletManager, const std::vector<Enemy*>& enemies) {

	// 装備切り替え
	if (Input::GetInstance()->TriggerKey(DIK_R)) {
		choiceRifle_ = !choiceRifle_;
		choiceSaber_ = !choiceSaber_;
	}

	// 手元位置
	Vector3 handOffset = {0.0f, 0.5f, 1.5f};
	// プレイヤーの回転を考慮したオフセットを計算
	Matrix4x4 rifleRotY = MakeRotateYMatrix(worldTransform_.rotation_.y);
	Vector3 rotatedOffset = TransformNormal(handOffset, rifleRotY);

	// ワールド座標での銃の位置
	Vector3 handPos = worldTransform_.translation_ + rotatedOffset;

	// 銃の回転（プレイヤーと同じY回転でOK）
	Vector3 weaponRotation = {0.0f, worldTransform_.rotation_.y - std::numbers::pi_v<float> / 2.0f, 0.0f};

	// 銃を選んでいるときはプレイヤーの手元に追従
	if (choiceRifle_) {

		rifle_->SetPosition(handPos, weaponRotation);
		// ロックオン中は敵の方向を向かせる
		if (isLockOn_ && lockOnTarget_) {
			Vector3 toEnemy = *lockOnTarget_ - handPos;

			// --- 水平角度（Y回転） ---
			float ry = std::atan2(toEnemy.x, toEnemy.z);

			// --- 垂直角度（X回転：ピッチ） ---
			float horizontalDist = std::sqrt(toEnemy.x * toEnemy.x + toEnemy.z * toEnemy.z);
			float rx = std::atan2(-toEnemy.y, horizontalDist); // 上下の角度（上を向くとマイナス）

			// 銃の回転を設定（Z回転は使わない）
			rifle_->SetPosition(handPos, {rx, ry, 0.0f});
		}

		rifle_->Update();

		// 弾の発射処理
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			rifle_->Fire(bulletManager, Bullet::Owner::kPlayer);
		}

		// リロード
		if (Input::GetInstance()->TriggerKey(DIK_E)) {
			rifle_->Reload();
		}
	}

	// 剣を選んでいるときはプレイヤーの手元に追従
	if (choiceSaber_) {

		saber_->SetPosition(handPos, weaponRotation);

		saber_->Update();

		// 攻撃開始
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			saber_->StartAttack();
		}

		if (saber_->IsAttacking()) {
			for (Enemy* enemy : enemies) {
				if (!enemy->IsDead() && saber_->CheckHit(enemy)) {
					enemy->Damage(35); // ← ここがダメージ処理の正しい場所
				}
			}
		}
	}
}

void Player::UpdateShield() {

	//---------------------盾----------------------

	// 盾の位置オフセット（左手の横に）
	Vector3 shieldOffsetSide = {0.0f, -0.3f, -1.3f};

	// 盾を前に構えるオフセット
	Vector3 shieldOffsetFront = {-1.0f, -0.3f, 0.0f};

	// プレイヤーの回転を考慮
	Matrix4x4 rotY = MakeRotateYMatrix(worldTransform_.rotation_.y);

	// 回転後のオフセット
	Vector3 shieldPosSide = worldTransform_.translation_ + TransformNormal(shieldOffsetSide, rotY);
	Vector3 shieldPosFront = worldTransform_.translation_ + TransformNormal(shieldOffsetFront, rotY);

	// ガード中（P押しっぱなし）かどうか
	bool guarding = Input::GetInstance()->PushKey(DIK_P);
	shield_->SetGuarding(guarding);

	if (guarding) {
		// 盾を前に構える
		shield_->SetPosition(shieldPosFront, {0.0f, worldTransform_.rotation_.y + std::numbers::pi_v<float> / 2.0f, 0.0f});
	} else {
		// 盾を横に構える
		shield_->SetPosition(shieldPosSide, {0.0f, worldTransform_.rotation_.y, 0.0f});
	}
}

void Player::UpdateStatusUI() {

	attackAlert_.Update();

	// ダメージエフェクトタイマー更新
	if (damageEffectTimer_ > 0.0f) {
		damageEffectTimer_ -= 1.0f / 60.0f;
	}

	// 無敵タイマー
	if (invincibleTimer_ > 0.0f) {
		invincibleTimer_ -= 1.0f / 60.0f;
	}

	// HPが0未満にならないように
	if (currentHp_ < 0) {
		currentHp_ = 0;
	}

	// HPが範囲外にならないようクランプ
	currentHp_ = std::clamp(currentHp_, 0.0f, maxHp_);

	// HPの割合を計算 (0.0 ～ 1.0)
	float hpRate = currentHp_ / maxHp_;

	// 元の最大幅（hpBarBaseSize_.x）に割合を掛けて、現在の幅を決定する
	// アンカーポイントが {0.0f, 0.5f} なので、左端固定で右側だけ縮む
	spriteHp_->SetSize({hpBarBaseSize_.x * hpRate, hpBarBaseSize_.y});

	// 銃を装備している時だけUIを更新
	if (rifle_) {
		ammoUI_.Update(rifle_->GetAmmo(), rifle_->GetReserveAmmo());
	}
}

void Player::Damage(int damage) {

	if (invincibleTimer_ > 0.0f) {
		return; // 無敵中
	}

	currentHp_ -= damage;

	if (currentHp_ < 0) {
		currentHp_ = 0;
		// isDead_ = true;（必要なら）
	}

	// 被弾エフェクト
	damageEffectTimer_ = damageEffectDuration_;
	invincibleTimer_ = invincibleTime_;
}

Vector2 Player::WorldToScreen(const Vector3& worldPos) {
	// ViewProjection 行列を自前で作る
	Matrix4x4 view = camera_->matView;
	Matrix4x4 proj = camera_->matProjection;
	// projection * view (正しい順序)
	Matrix4x4 vp = Multiply(proj, view);

	// 変換
	Vector3 screenPos = Transform(worldPos, vp);

	// カメラ後方なら無効
	if (screenPos.z <= 0.0f) {
		return {-1000.0f, -1000.0f};
	}

	// 4. スクリーン座標への変換 (1280x720想定)
	Vector2 screen;
	screen.x = (screenPos.x * 0.5f + 0.5f) * 1280.0f;
	screen.y = (1.0f - (screenPos.y * 0.5f + 0.5f)) * 720.0f; // Y軸は上下反転

	return screen;
}

void Player::SetLockOn(Enemy* enemy) {
	isLockOn_ = true;
	lockOnEnemy_ = enemy;
}

void Player::ClearLockOn() {
	isLockOn_ = false;
	lockOnEnemy_ = nullptr;
}

void Player::Draw() {

	// 自機
	model_->Draw(worldTransform_, *camera_);

	if (choiceRifle_) {

		rifle_->Draw();
	}

	if (choiceSaber_) {

		saber_->Draw();
	}

	shield_->Draw();
}

void Player::DrawDamageEffect() {
	if (damageEffectTimer_ > 0.0f && damageSprite_) {

		float alpha = damageEffectTimer_ / damageEffectDuration_;
		damageSprite_->SetColor({1.0f, 0.0f, 0.0f, alpha * 0.4f});
		damageSprite_->Draw();
	}

	// UI
	attackAlert_.Draw();

	bulletNumber_->Draw();

	// UIの描画

	ammoUI_.Draw();

	spriteHPBack_->Draw();
	spriteHp_->Draw();
}

Player::~Player() {

	rifle_ = nullptr;

	delete saber_;

	delete shield_;
}
