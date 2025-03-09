#include "TitleScene.h"
#include "../Enemy.h"
#include <DxLib.h>

TitleScene::TitleScene()
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Update()
{
	//Dでダイスクトラ
	if (CheckHitKey(KEY_INPUT_D)) {
		Enemy* enemy = (Enemy*)FindGameObject<Enemy>();
		if (enemy) {
			enemy->SetDijkstra(true);
		}
		SceneManager::ChangeScene("PLAY");
	}
	//BでBFS
	if (CheckHitKey(KEY_INPUT_B)) {
		Enemy* enemy = (Enemy*)FindGameObject<Enemy>();
		if (enemy) {
			enemy->SetDijkstra(false);
		}
		SceneManager::ChangeScene("PLAY");
	}
	if (CheckHitKey(KEY_INPUT_ESCAPE)) {
		SceneManager::Exit();
	}
}


void TitleScene::Draw()
{
	DrawString(0, 0, "TITLE SCENE", GetColor(255, 255, 255));
	DrawString(100, 420, "Press [B] for BFS", GetColor(255, 255, 255));
	DrawString(100, 440, "Press [D] for Dijkstra", GetColor(255, 255, 255));
}
