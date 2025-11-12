#include "Player.h"

#include <algorithm>
#include <cassert>
#include <numbers>


void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {
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
	worldTransform_.rotation_.y = 0.0f;
	
	//銃
	rifle_ = new Rifle();
	rifle_->Initialize(Model::CreateFromOBJ("Raifl"), camera_, position);
	choiceRifle_ = false;

	//剣
	saber_ = new saber();
	saber_->Initialize(Model::CreateFromOBJ("saber"), camera_, position);
	choiceSaber_ = true;
	
	

	//追従カメラの初期化
	followCamera_.Initialize(camera_);
	followCamera_.SetTarget(&worldTransform_.translation_);
	followCamera_.SetTargetOffset({0.0f, 4.0f, 0.0f});
	followCamera_.SetDistance(10.0f);

	WorldTransformUpdate(worldTransform_);
	
}



void Player::Update(BulletManager* bulletManager) { 
	
	// 重力と地面判定
	ApplyGravity();

	//プレイヤー移動
	UpdateMovement();

	////カメラの回転をプレイヤーのY回転に合わせる
	followCamera_.SetTarget(&worldTransform_.translation_);

	followCamera_.SetTargetRotation(&worldTransform_.rotation_);

	 // ロックオン時は敵方向を向く
	if (isLockOn_ && lockOnTarget_) {
		Vector3 toEnemy = *lockOnTarget_ - worldTransform_.translation_;
		worldTransform_.rotation_.y = std::atan2(toEnemy.x, toEnemy.z) + std::numbers::pi_v<float> / 2.0f;
	}

	//装備切り替え
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

	//銃を選んでいるときはプレイヤーの手元に追従
	if (choiceRifle_) {
		
		
		rifle_->SetPosition(handPos, weaponRotation);
		rifle_->Update();

		// 弾の発射処理
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			rifle_->Fire(bulletManager);
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

		//攻撃開始
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			saber_->StartAttack();
		}
	}

	//カメラ更新
	followCamera_.Update();

	#ifdef _DEBUG

	//地面判定
	ImGui::Begin("Player Info");
	ImGui::Text("isOnGround: %s", isOnGround_ ? "true" : "false");
	// ジャンプ時間
	ImGui::Text("Jump Time: %.2f", jumpTime_);


	//銃
	if (choiceRifle_) {

		ImGui::Begin("Rifle");
		ImGui::Text("Ammo: %d", rifle_->GetAmmo());
	}


#endif 



}

void Player::UpdateMovement() { 

	if (Input::GetInstance()->PushKey(DIK_W) ||Input::GetInstance()->PushKey(DIK_S) ||
		Input::GetInstance()->PushKey(DIK_A) || Input::GetInstance()->PushKey(DIK_D)) {

		Vector3 acceleration = {};
		if (Input::GetInstance()->PushKey(DIK_W)) {
		
			//上入力中の下入力
			if (velocity_.x > 0.0f) {
			
				//速度と逆方向に入力中は急ブレーキ
				velocity_.x *= (1.0f - KAcceleration);
			}

			acceleration.x -= KAcceleration / 60.0f;
		
		} else if (Input::GetInstance()->PushKey(DIK_S)) {

			// 下入力中の上入力
			if (velocity_.x < 0.0f) {

				// 速度と逆方向に入力中は急ブレーキ
				velocity_.x *= (1.0f - KAcceleration);
			}

			acceleration.x += KAcceleration / 60.0f;
		} else {
			// 入力なしだと減衰
			velocity_.x *= (1.0f - KAttenuation);
		}


		if (Input::GetInstance()->PushKey(DIK_A)) {

			// 左入力中の右入力
			if (velocity_.z > 0.0f) {

				// 速度と逆方向に入力中は急ブレーキ
				velocity_.z *= (1.0f - KAcceleration);
			}

			acceleration.z-= KAcceleration / 60.0f;

		} else if (Input::GetInstance()->PushKey(DIK_D)) {

			// 右入力中の左入力
			if (velocity_.z < 0.0f) {

				// 速度と逆方向に入力中は急ブレーキ
				velocity_.z *= (1.0f - KAcceleration);
			}

			acceleration.z += KAcceleration / 60.0f;
		} else {
			// 入力なしだと減衰
			velocity_.z *= (1.0f - KAttenuation);
		}


		

		//加速/減速
		velocity_ += acceleration;

		//最大速度制限
		velocity_.x = std::clamp(velocity_.x, -KLimitRunSpeed, KLimitRunSpeed);
		velocity_.z = std::clamp(velocity_.z, -KLimitRunSpeed, KLimitRunSpeed);

		//カメラの向きに合わせて移動方向を変更
		Matrix4x4 matRotY = MakeRotateYMatrix(worldTransform_.rotation_.y);
		Vector3 move = TransformNormal(velocity_, matRotY);

		

		//ワールド座標に反映
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
	if (input->TriggerKey(DIK_O) && isOnGround_) {
		isOnGround_ = false;
		velocity_.y = 0.0f; // 上昇開始時は初速0から
	}

	 // ===== 押している間は上昇 =====
	if (!isOnGround_) {
		if (input->PushKey(DIK_O) && jumpTime_ < kMaxJumpTime) {
			velocity_.y += kJumpPower; // 押してる間は上昇
			jumpTime_ += 1.5f;
		} else {
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


void Player::Draw() { 
	
	//自機
	model_->Draw(worldTransform_, *camera_);

	if (choiceRifle_) {
	
		rifle_->Draw();
	}

	if (choiceSaber_) {
	
		saber_->Draw();
	}
	
}

Player::~Player() { 
	
	delete rifle_;

	delete saber_;
}

