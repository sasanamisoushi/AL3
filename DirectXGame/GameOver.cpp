#include "GameOver.h"

using namespace KamataEngine;

void GameOver::Initialize() {

}

void GameOver::Update() {
	if (KamataEngine::Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		isFinish = true;
	}

	#ifdef _DEBUG
	ImGui::Begin("Game Over Sene");
	ImGui::Text("Press SPACE to Start");
	ImGui::End();
#endif
}

void GameOver::Draw() {

}
