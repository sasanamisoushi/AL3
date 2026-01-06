#include "AttackAlert.h"

using namespace KamataEngine;

void AttackAlert::Initialize() {
	alaertR_ = Sprite::Create(TextureManager::Load("./Resources/UI/alertR.png"), {0.0f,0.0f}, {1, 1, 1, 1}, {0.0f, 0.0f});
	alaertL_ = Sprite::Create(TextureManager::Load("./Resources/UI/alertL.png"), {0.0f, 0.0f}, {1, 1, 1, 1}, {0.0f, 0.0f});
	alaertTop_ = Sprite::Create(TextureManager::Load("./Resources/UI/alertTop.png"), {0.0f, 0.0f}, {1, 1, 1, 1}, {0.0f, 0.0f});
	alaertBottom_ = Sprite::Create(TextureManager::Load("./Resources/UI/alertB.png"), {0.0f, 0.0f}, {1, 1, 1, 1}, {0.0f, 0.0f});
}

void AttackAlert::Trigger(AlertDir dir) {
	switch (dir) {
	case AlertDir::Right:
		timerR_ = kDisplayTime;
		break;
	case AlertDir::Left:
		timerL_ = kDisplayTime;
		break;
	case AlertDir::Front:
		timerTop_ = kDisplayTime;
		break;
	case AlertDir::Back:
		timerBottom_ = kDisplayTime;
		break;
	}
}

void AttackAlert::Update() {
	const float dt = 1.0f; // 60fps前提

	if (timerR_ > 0)
		timerR_ -= dt;
	if (timerL_ > 0)
		timerL_ -= dt;
	if (timerTop_ > 0)
		timerTop_ -= dt;
	if (timerBottom_ > 0)
		timerBottom_ -= dt;
}

void AttackAlert::Draw() {
	if (timerR_ > 0) {
		alaertR_->Draw();
	}
	if (timerL_ > 0) {
		alaertL_->Draw();
	}
	if (timerTop_ > 0) {
		alaertTop_->Draw();
	}
	if (timerBottom_ > 0) {
		alaertBottom_->Draw();
	}
	
}