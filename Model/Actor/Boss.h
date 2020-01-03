#pragma once

class Boss :public Actor
{
public:
	Boss();
	~Boss();

	void Update();
	void Render();

	void Moving(D3DXVECTOR3 playerPos);

	void Input(string mode);
	void SetBoss(string Name = "../_Contents/BinModels/Actor_Boss.model");
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

