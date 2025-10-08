#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include "GameClearScene.h"
#include <Windows.h>

using namespace KamataEngine;

// シーンの状態を管理するenum
enum class Scene {
	kUnknown = 0,
	kTitle,
	kGame,
	kGameClear,
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンの初期化
	KamataEngine::Initialize(L"AL3");

	// 描画のためのDirectXCommmonインスタンスの取得
	DirectXCommon* dxCommmon = DirectXCommon::GetInstance();
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	// シーンオブジェクトへのポインタ
	GameScene* gameScene = nullptr;
	TitleScene* titleScene = nullptr;
	GameClearScene* gameClearScene = nullptr;

	// 現在と次のシーンの状態変数
	Scene currentSceneEnum = Scene::kTitle;
	Scene nextSceneEnum = currentSceneEnum;

	// 初期シーンの設定
	titleScene = new TitleScene();
	titleScene->Initialize();

	// メインループ
	while (true) {
		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}

		// ImGuiのフレーム開始
		imguiManager->Begin();

		// シーンの切り替えチェック
		if (currentSceneEnum != nextSceneEnum) {
			// 古いシーンのクリーンアップ
			switch (currentSceneEnum) {
			case Scene::kTitle:
				delete titleScene;
				titleScene = nullptr;
				break;
			case Scene::kGame:
				delete gameScene;
				gameScene = nullptr;
				break;
			case Scene::kGameClear:
				delete gameClearScene;
				gameClearScene = nullptr;
				break;
			default:
				break;
			}
		}

		// 新しいシーンに更新
		currentSceneEnum = nextSceneEnum;

		// 新しいシーンの初期化
		switch (currentSceneEnum) {
		case Scene::kTitle:
			titleScene = new TitleScene();
			titleScene->Initialize();
			break;
		case Scene::kGame:
			gameScene = new GameScene();
			gameScene->Initialize();
			break;
		case Scene::kGameClear:
			gameClearScene = new GameClearScene();
			gameClearScene->Initialize();
			break;
		}

		// 現在のシーンを更新
		switch (currentSceneEnum) {
		case Scene::kTitle:
			if (titleScene != nullptr) {
				titleScene->Update();
				if (titleScene->IsFinished()) {
					nextSceneEnum = Scene::kGame;
				}
			}
			break;
		case Scene::kGame:
			if (gameScene != nullptr) {
				gameScene->Update();
				if (gameScene->IsFinished()) {
					nextSceneEnum = Scene::kGameClear;
				}
			}
			break;
		case Scene::kGameClear:
			if (gameClearScene != nullptr) {
				gameClearScene->Update();
				if (gameClearScene->IsFinished()) {
					nextSceneEnum = Scene::kTitle;
				}
			}
			break;
		}

		imguiManager->End();

		// 描画開始
		dxCommmon->PreDraw();
		// 現在のシーンを描画
		switch (currentSceneEnum) {
		case Scene::kTitle:
			if (titleScene != nullptr) {
				titleScene->Draw();
			}
			break;
		case Scene::kGame:
			if (gameScene != nullptr) {
				gameScene->Draw();
			}
			break;
		case Scene::kGameClear:
			if (gameClearScene != nullptr) {
				gameClearScene->Draw();
			}
			break;
		}
		imguiManager->Draw();
		// 描画終了
		dxCommmon->PostDraw();
	}

	// 最終的なクリーンアップ
	switch (currentSceneEnum) {
	case Scene::kTitle:
		delete titleScene;
		break;
	case Scene::kGame:
		delete gameScene;
		break;
	case Scene::kGameClear:
		delete gameClearScene;
		break;
	}

	// エンジンの終了処理
	KamataEngine::Finalize();

	return 0;
}
