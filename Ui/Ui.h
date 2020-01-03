#pragma once

class Ui
{
public:
	static Ui* Get();
	static void Delete();

	void Update();
	void Render();

	void RenderCock();
	void RenderItemBox();
	void PlayerGetDmg(UINT dmg);
	void SetScreenSize();
	void SetPlayer(class Player* player) { this->player = player; }
private:
	static Ui* instance;

	typedef pair<wstring, class UiPart*> Pair;
	typedef pair<wstring, class Bar*> Pair2;
	map<wstring,  UiPart*> parts;
	map<wstring,  Bar*> bars;
	class Mini* mini;
	class Screen* screen;
	class Player* player;

	D3DXVECTOR2 WinSize;
	Ui();
	~Ui();
};