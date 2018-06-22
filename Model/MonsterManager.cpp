#include "../stdafx.h"
#include "MonsterManager.h"
#include "Monster.h"

MonsterManager::MonsterManager()
{
}
MonsterManager::~MonsterManager()
{
	for (Monster* monster : monsters)
		SAFE_DELETE(monster);
}

void MonsterManager::Update()
{
	if(Keyboard::Get()->Down('O')) AddMonster();
	for(Monster* monster:monsters )
	monster->Update();
}

void MonsterManager::Render()
{
	for (Monster* monster : monsters)
		monster->Update();
}

void MonsterManager::AddMonster(string Name)
{
	Monster* nM=new Monster(Name);
	monsters.push_back(nM);
}

void MonsterManager::AddMonster()
{
	Monster* nM = new Monster("../_Contents/BinModels/Actor_Monster.model");
	monsters.push_back(nM);
}