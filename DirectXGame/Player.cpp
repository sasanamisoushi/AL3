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
	
	
	

	//追従カメラの初期化
	followCamera_.Initialize(camera_);
	followCamera_.SetTarget(&worldTransform_.translation_);
	followCamera_.SetTargetOffset({0.0f, 4.0f, 0.0f});
	followCamera_.SetDistance(10.0f);

	WorldTransformUpdate(worldTransform_);
	
}



void Player::Update() { 
	
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

	//カメラ更新
	followCamera_.Update();

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


void Player::Draw() { 
	
	//自機
	model_->Draw(worldTransform_, *camera_);
	
}

