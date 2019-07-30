#pragma once

#include "Actor.h"
class Boss :public Actor
{
public:
	Boss();
	~Boss();

	void Update();
	void Render();

	void Input(string mode);
	void SetBoss(string Name = "../_Contents/BinModels/Actor_Boss4.model");
	void SetPlayer(class Player* player) { this->player = player; }
private:
	class Look* look;
	class Player* player;

	void Notify();

	int dir, degree, degree_goal, weaponNum;
	float speed;
	bool isControl, isEquip, isHeight;
	string mode, Premode;
	D3DXVECTOR3 way;


	bool isLoaded;
	thread* loadThread;

};

