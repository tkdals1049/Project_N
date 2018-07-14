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
	void Input(string mode);
	
	void SetPlayer();
	float SetHeight();
	
	const bool GetisEquip() { return isEquip; }
	const bool GetisAttack() { return isAttack; }
	const string GetMode(){return mode;}
	const string GetPremode() {return Premode;}
	ST_OBB* GetAttackRange(){return attackRange;}

	void AddWeapon(Model * weaponFile, D3DXVECTOR3 scale);
	void ClearWeapon();
	
private:
	class Weapon* weapon;

	void Control();
	void Notify();

	ST_OBB* attackRange;

	int dir,degree,degree_goal;
	float speed;
	bool isControl,isEquip,isHeight, isAttack;
	string mode,Premode;
	D3DXVECTOR3 way,way2;

	bool isLoaded;
	thread* loadThread;
};
