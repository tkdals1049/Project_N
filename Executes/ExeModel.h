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
	void OpenModelFile(wstring file);


	class Sky* sky;
	class Plane* plane;
	class Map* map;

	ID3D11RasterizerState * getRasterizer;
	ID3D11RasterizerState* setRasterizer;

	Model* model;
	wstring selectedFbxFile;

	bool isLoaded;
	thread* loadThread;
};