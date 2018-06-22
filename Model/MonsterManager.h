#pragma once

class Monster;
class MonsterManager
{
public:
	MonsterManager();
	~MonsterManager();

	void Update();
	void Render();
	void AddMonster(string Name);
	void AddMonster();
private:
	vector<Monster*> monsters;

};
