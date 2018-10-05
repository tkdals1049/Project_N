#include "../../stdafx.h"
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
		if(isLoaded)monster->Update();
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
				player->SetoffAttack();
			}
		}
	}
}

void EnemyManager::AddMonster(string Name)
{
	loadThread = new thread([&]()
	{
		isLoaded = false;

		Monster* nM=new Monster(Name);
		monsters.push_back(nM);

		isLoaded = true;
	});
}

void EnemyManager::AddMonster()
{
	loadThread = new thread([&]()
	{
		isLoaded = false;

		Monster* nM = new Monster();
		nM->SetMonster();
		nM->SetPlayer(player);
		monsters.push_back(nM);

		isLoaded = true;
	});
}