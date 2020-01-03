#pragma once
#include "Execute.h"

class ExeGui : public Execute
{
public:
	ExeGui();
	~ExeGui();

	void Update();

	void PreRender();
	void Render();
	void PostRender();

	void ResizeScreen();

private:
	UINT count;

	bool bShowDemo;
	bool bShowAnother;
};