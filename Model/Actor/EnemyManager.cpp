
#include "../../stdafx.h"
#include "EnemyManager.h"
#include "Player.h"
#include "Boss.h"
#include "Monster.h"

EnemyManager::EnemyManager(Player* player):player(player),isMake(false)
{
	boss = new Boss();
}
EnemyManager::~EnemyManager()
{
	for (Monster* monster : monsters)
		SAFE_DELETE(monster);
}

void EnemyManager::Update()
{
	if (!isMake)
	{
		if (Keyboard::Get()->Down('M')) {AddMonster();isMake=true;}
		if (Keyboard::Get()->Down('V')) {AddBoss();	  isMake=true;}
	}
		boss->Update();
	for(Monster* monster:monsters )
	{
		monster->Update();
	}
}

void EnemyManager::Render()
{
	boss->Render();
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


void EnemyManager::AddMonster()
{
	Monster* nM = new Monster();
	nM->SetPlayer(player);
	nM->SetMonster();
	monsters.push_back(nM);
	isMake = false;
}

void EnemyManager::AddBoss()
{
	boss->SetPlayer(player);
	boss->SetBoss();
	isMake = false;
}