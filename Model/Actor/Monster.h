#pragma once

class Monster :public Actor
{
public:
	Monster();
	Monster(string file);
	~Monster();

	void PreUpdate(D3DXVECTOR3 origin, D3DXVECTOR3 direction);

	void Update();
	void Render();
	void Moving(D3DXVECTOR3 playerPos);
	void Input(string mode);
	void SetMonster(string Name = "../_Contents/BinModels/Actor_Monster.model");
	void SetPlayer(class Player* player) { this->player = player; }
private:
	class Look* look;
	class Player*player;

	void Notify();

	int dir, degree, degree_goal, weaponNum;
	float speed;
	bool isControl, isEquip, isHeight;
	string mode, Premode;
	D3DXVECTOR3 way;

	bool isLoaded;
	thread* loadThread;
};
