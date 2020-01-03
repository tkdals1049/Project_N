#pragma once
#include "Execute.h"

class ExeModel : public Execute
{
public:
	ExeModel();
	~ExeModel();

	void CreateSingleTon();
	void DeleteSingleTon();

	void Update();
	void PreRender();
	void Render();
	void PostRender();

	void ResizeScreen();
	void GetRay(D3DXVECTOR3 * origin, D3DXVECTOR3 * direction);

private:
	void OpenConvertDialog(wstring file = L"");
	void Convert(wstring file);

	void OpenModelDialog(wstring file = L"");
	void SaveModelDialog(wstring file = L"");

	class ModelGroup* group;
	class MusicPlayer* music;
	MusicPlayer* music2;
	wstring selectedFbxFile;

	bool isLoaded,isEnviroment,isModel, isWire;
	thread* loadThread;
};