#include "TitleScene.h"

void TitleScene::Initialize() {}

void TitleScene::Update() {
	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		isFinish = true;
	}

#ifdef _DEBUG
	ImGui::Begin("Title Scene");
	ImGui::Text("Press SPACE to Start");
	ImGui::End();
#endif
}

void TitleScene::Draw() {}
