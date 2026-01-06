#pragma once
#include "KamataEngine.h"

class Explanation {
public:
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// デスフラグのgetter
	bool IsFinished() const { return isFinish; }

	~Explanation();

private:
	// 終了フラグ
	bool isFinish = false;

	//スプライト
	KamataEngine::Sprite* explanation_ = nullptr;

};
