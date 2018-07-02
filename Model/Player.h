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
	void ClearWeapon();
	void AddWeaponVector(string weaponName, Model * weaponFile);

	float SetHeight();
	bool GetisAttack(){return isAttack;}
	ST_OBB* GetAttackRange(){return attackRange;}
private:
	void WeaponUpdate();
	void Control();
	void Notify();

	typedef pair<string, Model*> Weapon;
	vector<Weapon> weapons;
	D3DXMATRIX weaponWorld;
	ST_OBB* attackRange;

	int dir,degree,degree_goal,weaponNum;
	float speed;
	bool isControl,isEquip,isHeight, isAttack;
	string mode,Premode,equipName, equipName2, unequipName;
	D3DXVECTOR3 way,way2;

	bool isLoaded;
	thread* loadThread;
};
