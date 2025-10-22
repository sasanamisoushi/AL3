#include "saber.h"
#include "MyMath.h"
#include <numbers>

void saber::Initialize(Model* model, Camera* camera, const Vector3& position) { 
	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = {0.3f, 0.3f, 0.3f};
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float>;

	WorldTransformUpdate(worldTransform_);

}

void saber::Update() { 
	
}

void saber::Draw() {

	
	
		model_->Draw(worldTransform_, *camera_);
	

}

void saber::SetPosition(const Vector3& position, const Vector3& rotation) {
	worldTransform_.translation_ = position;
	worldTransform_.rotation_ = rotation;
	WorldTransformUpdate(worldTransform_);

}

//void saber::StartAttack(const Vector3& playerPos, const Vector3& forward) {}
