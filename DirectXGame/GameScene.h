#pragma once

#include "KamataEngine.h"

using namespace KamataEngine;

class GameScene {
public:
	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// デスフラグのgetter
	bool IsFinished() const { return isFinish; }

	

private:
	// カメラ
	Camera camera_;

	

	// 終了フラグ
	bool isFinish = false;
};
