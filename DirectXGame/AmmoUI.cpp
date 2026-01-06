#include "AmmoUI.h"

using namespace KamataEngine;

void AmmoUI::Initialize() {
	for (int i = 0; i < 10; i++) {
		numberTex_[i] = TextureManager::Load("./Resources/number/" + std::to_string(i) + ".png");
	}
}

void AmmoUI::Update(int ammo, int reserve) {
	CreateNumberSprites(ammo, ammoSprites_, numberTex_, {1130, 400});
	CreateNumberSprites(reserve, reserveSprites_, numberTex_, {1125, 480});
}

void AmmoUI::Draw() {
	for (auto* s : ammoSprites_) {

		s->Draw();
	}
	for (auto* s : reserveSprites_) {
		s->Draw();
	}
}

void AmmoUI::CreateNumberSprites(int value, std::vector<Sprite*>& sprites, uint32_t* numberTex, Vector2 basePos) {
	// 1. 古いスプライトを削除してリストを空にする（メモリリーク防止）
	for (auto* s : sprites) {
		delete s;
	}
	sprites.clear();

	// 2. 数値を文字列に変換して1文字ずつスプライト化
	std::string str = std::to_string(value);

	for (size_t i = 0; i < str.size(); i++) {
		int num = str[i] - '0'; // 文字から数値に変換
		// i * 24.0f で横に並べる
		Sprite* sp = Sprite::Create(numberTex[num], {basePos.x + i * 32.0f, basePos.y});

		sp->SetSize({50.0f, 50.0f});

		sprites.push_back(sp);
	}
}

AmmoUI::~AmmoUI() {

	// 残っているスプライトを全て削除
	for (auto* s : ammoSprites_) {
		delete s;
	}
	for (auto* s : reserveSprites_) {
		delete s;
	}
}