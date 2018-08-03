#pragma once
#include "Actor.h"

class Monster :public Actor
{
public:
	Monster();
	Monster(string file);
	~Monster();

	void PreUpdate(D3DXVECTOR3 origin, D3DXVECTOR3 direction);

	void Update();
	void Render();
	void Input(string mode);
	void SetMonster();

private:
	void Notify();

	int dir, degree, degree_goal, weaponNum;
	float speed;
	bool isControl, isEquip, isHeight;
	string mode, Premode;
	D3DXVECTOR3 way;

};