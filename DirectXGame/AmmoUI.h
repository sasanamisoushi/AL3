#pragma once
#include "KamataEngine.h"


class AmmoUI {
public:

	~AmmoUI();

	void Initialize();
	void Update(int ammo, int reserve);
	void Draw();

	void CreateNumberSprites(int value, std::vector<KamataEngine::Sprite*>& sprites, uint32_t* numberTex, KamataEngine::Vector2 basePos);

private:
	std::vector<KamataEngine::Sprite*> ammoSprites_;    // 現在弾数
	std::vector<KamataEngine::Sprite*> reserveSprites_; // 総弾数
	uint32_t numberTex_[10];
};
