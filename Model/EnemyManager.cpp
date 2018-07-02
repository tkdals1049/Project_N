#include "../stdafx.h"
#include "EnemyManager.h"
#include "Player.h"
#include "Boss.h"
#include "Monster.h"

EnemyManager::EnemyManager(Player* player):player(player)
{
}
EnemyManager::~EnemyManager()
{
	for (Monster* monster : monsters)
		SAFE_DELETE(monster);
}

void EnemyManager::Update()
{
	if(Keyboard::Get()->Down('M')) AddMonster();
	for(Monster* monster:monsters )
	{
		monster->Update();
	}
}

void EnemyManager::Render()
{
	for (Monster* monster : monsters)
	{
		monster->Render();
		if (player->GetisAttack())
		{
			if(monster->CalSkeleton(player->GetAttackRange()))
			{
			SAFE_DELETE(monster);
			monsters.clear();
			}
		}
	}
}

void EnemyManager::AddMonster(string Name)
{
	Monster* nM=new Monster(Name);
	monsters.push_back(nM);
}

void EnemyManager::AddMonster()
{
	Monster* nM = new Monster();
	nM->SetMonster();
	monsters.push_back(nM);
}