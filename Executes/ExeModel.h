#pragma once
#include "Execute.h"

class ExeModel : public Execute
{
public:
	ExeModel(ExecuteValues* values);
	~ExeModel();

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

	ID3D11RasterizerState * getRasterizer;
	ID3D11RasterizerState* setRasterizer;

	Model* model;
	wstring selectedFbxFile;

	bool isLoaded,isEnviroment,isModel;
	thread* loadThread;
};