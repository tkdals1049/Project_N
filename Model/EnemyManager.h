#pragma once

class Player;
class Boss;
class Monster;
class EnemyManager
{
public:
	EnemyManager(Player* player);
	~EnemyManager();

	void Update();
	void Render();
	void AddMonster(string Name);
	void AddMonster();
private:
	Player* player;
	Boss* boss;
	vector<Monster*> monsters;

	bool isLoaded;
	thread* loadThread;

};
