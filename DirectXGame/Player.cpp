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
	worldTransform_.translation_ = position;

}



void Player::Update() { 
	
	//プレイヤー移動
	UpdateMovement();

}

void Player::UpdateMovement() { 

	if (Input::GetInstance()->PushKey(DIK_W) ||Input::GetInstance()->PushKey(DIK_S) ||
		Input::GetInstance()->PushKey(DIK_A) || Input::GetInstance()->PushKey(DIK_D)) {

		Vector3 acceleration = {};
		if (Input::GetInstance()->PushKey(DIK_W)) {
		
			//上入力中の下入力
			if (velocity_.z < 0.0f) {
			
				//速度と逆方向に入力中は急ブレーキ
				velocity_.z *= (1.0f - KAcceleration);
			}

			acceleration.z += KAcceleration / 60.0f;
		
		} else if (Input::GetInstance()->PushKey(DIK_S)) {

			// 下入力中の上入力
			if (velocity_.z > 0.0f) {

				// 速度と逆方向に入力中は急ブレーキ
				velocity_.z *= (1.0f - KAcceleration);
			}

			acceleration.z -= KAcceleration / 60.0f;
		} else {
			// 入力なしだと減衰
			velocity_.z *= (1.0f - KAttenuation);
		}


		if (Input::GetInstance()->PushKey(DIK_A)) {

			// 左入力中の右入力
			if (velocity_.x < 0.0f) {

				// 速度と逆方向に入力中は急ブレーキ
				velocity_.x *= (1.0f - KAcceleration);
			}

			acceleration.x += KAcceleration / 60.0f;

		} else if (Input::GetInstance()->PushKey(DIK_D)) {

			// 右入力中の左入力
			if (velocity_.x > 0.0f) {

				// 速度と逆方向に入力中は急ブレーキ
				velocity_.x *= (1.0f - KAcceleration);
			}

			acceleration.x -= KAcceleration / 60.0f;
		} else {
			// 入力なしだと減衰
			velocity_.x *= (1.0f - KAttenuation);
		}

		//加速/減速
		velocity_ += acceleration;

		//最大速度制限
		velocity_.x = std::clamp(velocity_.x, -KLimitRunSpeed, KLimitRunSpeed);
		velocity_.z = std::clamp(velocity_.z, -KLimitRunSpeed, KLimitRunSpeed);

		//カメラの向きに合わせて移動方向を変更
		 Matrix4x4 matRotY = MakeRotateYMatrix(camera_->rotation_.y);
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
