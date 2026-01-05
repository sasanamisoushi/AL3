#pragma once
#include "KamataEngine.h"



class AttackAlert {
public:

	enum class AlertDir { Front, Back, Left, Right };

	void Initialize();
	void Trigger(AlertDir dir);
	void Update();
	void Draw();

private:
	float timer_ = 0.0f;
	AlertDir dir_;
	static constexpr float kDisplayTime = 30.0f;

	KamataEngine::Sprite* alaertR_ = nullptr;
	KamataEngine::Sprite* alaertL_ = nullptr;
	KamataEngine::Sprite* alaertTop_ = nullptr;
	KamataEngine::Sprite* alaertBottom_ = nullptr;

	float timerR_ = 0.0f;
	float timerL_ = 0.0f;
	float timerTop_ = 0.0f;
	float timerBottom_ = 0.0f;

};
