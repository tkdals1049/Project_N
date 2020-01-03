#include "stdafx.h"
#include "Ui.h"
#include "Screen.h"
#include "UiPart.h"
#include "Mini.h"
#include "Bar.h"
#include "../Model/Actor/Player.h"


#define Xpixel (float)1/512
#define Ypixel Xpixel*2

Ui* Ui::instance = NULL;
Ui * Ui::Get()
{
	if (instance == NULL)
		instance = new Ui();

	return instance;
}

void Ui::Delete()
{
	SAFE_DELETE(instance);
}

Ui::Ui()
{
	mini = new Mini();
	screen = new Screen();
	bars[L"Hp"]= new Bar();
	bars[L"St"] = new Bar(D3DXCOLOR(1,1,0,1));

	parts.insert(Pair(L"Cock",new UiPart(L"Textures/Ui/Cock.png")));
	parts.insert(Pair(L"Item", new UiPart(L"Textures/Ui/ItemIcon.png")));
	
	SetScreenSize();
}

Ui::~Ui()
{
	for each(Pair temp in parts)
		SAFE_DELETE(temp.second);
	for each(Pair2 temp in bars)
		SAFE_DELETE(temp.second);

	SAFE_DELETE(mini);
	SAFE_DELETE(screen);
}

void Ui::Update()
{
	if (Keyboard::Get()->Down('T')) { screen->SetMode(); }

	screen->Update();
	mini->Update();
}

void Ui::Render()
{
	CameraManager::Get()->SetScreenCamera();

	screen->Render(D3DXVECTOR2(0,0),WinSize);

	RenderCock();
	RenderItemBox();

	CameraManager::Get()->DefaultCamera();
}

void Ui::RenderCock()
{
	D3DXVECTOR2 CockPos(60, WinSize.y - 80);
	D3DXVECTOR2 CockSize(60, 60), BarSize(CockSize.x * 2, CockSize.y / 10);

	bars[L"Hp"]->Render(CockPos + D3DXVECTOR2(CockSize.x * 3, CockSize.y * (float)18 / 81), BarSize);
	bars[L"St"]->Render(CockPos + D3DXVECTOR2(CockSize.x * 3, CockSize.y * (float)0 / 81), BarSize);

	parts[L"Cock"]->Render(D3DXVECTOR4(0, Ypixel * 1, Xpixel * 82, Ypixel * 83), CockPos, CockSize);
	parts[L"Cock"]->Render(D3DXVECTOR4(Xpixel * 99, Ypixel * 75, Xpixel * 212, Ypixel * 80),
		CockPos + D3DXVECTOR2(CockSize.x * 3, CockSize.y * (float)18 / 81), BarSize);
	parts[L"Cock"]->Render(D3DXVECTOR4(Xpixel * 99, Ypixel * 75, Xpixel * 212, Ypixel * 80),
		CockPos + D3DXVECTOR2(CockSize.x * 3, CockSize.y * (float)0 / 81), BarSize);

	mini->Render(D3DXVECTOR2(80, 80), D3DXVECTOR2(100,100), minimap);
	parts[L"Cock"]->Render(D3DXVECTOR4(Xpixel * 338, Ypixel * 38, Xpixel * 380, Ypixel * 80),
		D3DXVECTOR2(100, 100), D3DXVECTOR2(88, 88));

}

void Ui::RenderItemBox()
{
	D3DXVECTOR2 ItemBoxPos(WinSize.x - 120,90);
	D3DXVECTOR2 ItemUv(128,85),ItemSize(20,20);

	parts[L"Cock"]->Render(D3DXVECTOR4(Xpixel * 184, Ypixel * 128, Xpixel * 229, Ypixel * 165),
		ItemBoxPos + D3DXVECTOR2(-60, 15), D3DXVECTOR2(35, 35));
	parts[L"Cock"]->Render(D3DXVECTOR4(Xpixel * 259, Ypixel * 90, Xpixel * 335, Ypixel * 158),
		ItemBoxPos, D3DXVECTOR2(60, 60));
	parts[L"Cock"]->Render(D3DXVECTOR4(Xpixel * 229, Ypixel * 128, Xpixel * 184, Ypixel * 165),
		ItemBoxPos + D3DXVECTOR2(60, 15), D3DXVECTOR2(35, 35));

	parts[L"Item"]->Render(D3DXVECTOR4(Xpixel * ItemUv.x * 1, Ypixel * 0, Xpixel * ItemUv.x * 2, Ypixel * ItemUv.y),
		ItemBoxPos + D3DXVECTOR2(-60, 15), ItemSize);
	parts[L"Item"]->Render(D3DXVECTOR4(Xpixel * ItemUv.x * 0, Ypixel * 0, Xpixel * ItemUv.x * 1, Ypixel * ItemUv.y),
		ItemBoxPos + D3DXVECTOR2(0, 15), ItemSize + D3DXVECTOR2(10, 10));
	parts[L"Item"]->Render(D3DXVECTOR4(Xpixel * ItemUv.x * 2, Ypixel * 0, Xpixel * ItemUv.x * 3, Ypixel * ItemUv.y),
		ItemBoxPos + D3DXVECTOR2( 60, 15), ItemSize);

	ImGuiWindowFlags flag = 0;
	flag |= ImGuiWindowFlags_NoTitleBar;
	flag |= ImGuiWindowFlags_NoMove;
	flag |= ImGuiWindowFlags_NoResize;
	string str = "Grilled Meat";
	ImGui::SetNextWindowPos(ImVec2(WinSize.x - 170, WinSize.y - 40));
	ImGui::Begin("itme window", NULL, flag);
	ImGui::Text(str.c_str());
	ImGui::End();

}

void Ui::PlayerGetDmg(UINT dmg)
{
	bars[L"Hp"]->Damage(dmg);
}

void Ui :: SetScreenSize()
{
	D3DDesc Windesc;
	D3D::GetDesc(&Windesc);

	WinSize = D3DXVECTOR2((float)Windesc.Width, (float)Windesc.Height);
}