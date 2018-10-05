#pragma once
#include "Actor.h"

class Player:public Actor
{
public:
	Player();
	~Player();

	void PreUpdate(D3DXVECTOR3 origin, D3DXVECTOR3 direction);

	void Update();
	void Render();
	void CameraUpdate();
	void Input(string mode);
	
	void SetPlayer();
	float SetHeight();

	void AddWeapon(Model * weaponFile, D3DXVECTOR3 scale);
	void ClearWeapon();
	
private:
	class Weapon* weapon;
	void Control();
	void Notify();

	int dir,degree,degree_goal;
	float speed;
	D3DXVECTOR3 way,way2;

	bool isLoaded,isRec;
	thread* loadThread;
};
