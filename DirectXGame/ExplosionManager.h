#pragma once
#include "KamataEngine.h"
#include "Explosion.h"
#include <string>
#include <list>

class ExplosionManager {
public:

	void Initialize() {
		// ★ここで板ポリ（plane.obj）を読み込む
		// ※mtlファイルで explosion.png を指定しておくのが一番簡単です
		explosionModel_ = KamataEngine::Model::CreateFromOBJ("Explosion");
	}

	void Add(const KamataEngine::Vector3& pos, KamataEngine::Camera* camera) {
		explosions_.emplace_back();
		explosions_.back().Initialize(explosionModel_,pos, camera);

		// デバッグ用に、最後に生成された座標を記録しておく
		lastDebugPos_ = pos;
		hasDebugRecord_ = true;
	}

	void Update() {
		for (auto& e : explosions_) {
			e.Update();
		}
		explosions_.remove_if([](const Explosion& e) { return e.IsFinished(); });
	}

	void Draw() {
		for (auto& e : explosions_) {
			e.Draw();
		}
	}

	// ImGui描画用関数
	void DrawUI() {
#ifdef _DEBUG
		ImGui::Begin("Explosion Debug");
		// 今画面に出ている爆発の数
		ImGui::Text("Active Count: %d", (int)explosions_.size());

		ImGui::Separator();

		// 最後に発生した場所（エフェクトが消えても残り続ける）
		if (hasDebugRecord_) {
			ImGui::Text("Last Spawn Pos (Log):");
			ImGui::Text("X: %.2f", lastDebugPos_.x);
			ImGui::Text("Y: %.2f", lastDebugPos_.y);
			ImGui::Text("Z: %.2f", lastDebugPos_.z);
		} else {
			ImGui::Text("No explosion yet.");
		}
		ImGui::End();
#endif
	}

private:
	std::list<Explosion> explosions_;
	//爆発モデル
	KamataEngine::Model* explosionModel_ = nullptr;

	// デバッグ用変数
	KamataEngine::Vector3 lastDebugPos_ = {0, 0, 0};
	bool hasDebugRecord_ = false;
};
