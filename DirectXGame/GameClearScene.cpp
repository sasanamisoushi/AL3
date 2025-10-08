#include "GameClearScene.h"

void GameClearScene::Initialize() {}

void GameClearScene::Update() {
	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		isFinish = true;
	}

#ifdef _DEBUG
	ImGui::Begin("Game Clear Scene");
	ImGui::End();
#endif
}

void GameClearScene::Draw() {}
