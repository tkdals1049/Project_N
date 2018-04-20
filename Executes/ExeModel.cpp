#include "stdafx.h"
#include "ExeModel.h"
#include "../Content/Texture.h"
#include "../Content/BinModel.h"
#include "../Content/FbxModel.h"
#include "../Model/Map.h"
#include "../Environment/Sky.h"
#include "../Meshes/Plane.h"
#include "../FbxModel/MoLoader.h"
#include "../Utilities/Path.h"

ExeModel::ExeModel(ExecuteValues* values)
	: Execute(values)
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	Texture::Get();
	BinModel::Get();
	FbxModel::Get();
	model= new Model();
	sky = new Sky();
	plane = new Plane();
	map=new Map();
	ImGui::SetNextWindowPos(ImVec2(800, 500)); 
}

ExeModel::~ExeModel()
{
	SAFE_DELETE(model);
	SAFE_DELETE(plane);
	SAFE_DELETE(sky);
}

void ExeModel::Update()
{
	D3DXVECTOR3 origin, direction;
	GetRay(&origin,&direction);

	sky->Update(values->MainCamera);
	plane->UpdatePointBuffer(origin,direction);
	plane->Update();
	map->SetDot(plane->GetDot());
	map->PreUpdate(origin,direction);
	map->Update();
}

void ExeModel::PreRender()
{
	
}

void ExeModel::Render()
{
	sky->Render();
	plane->Render();
	map->Render();
}

void ExeModel::PostRender()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Convert", "Ctrl+C")) 
			{
				OpenConvertDialog();
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Open", "Ctrl+O"))
				OpenModelDialog();

			ImGui::EndMenu();
		}//if(BeiginMenu)

		ImGui::EndMainMenuBar();
	}

	sky->PostRender();
	plane->PostRender();
	map->PostRender();
}

void ExeModel::ResizeScreen()
{
	
}

void ExeModel::GetRay(D3DXVECTOR3 * origin, D3DXVECTOR3 * direction)
{
	D3DXVECTOR3 mouse = Mouse::Get()->GetPosition();

	D3DXMATRIX view, projection;
	values->MainCamera->GetMatrix(&view);
	values->Perspective->GetMatrix(&projection);
	D3DXVECTOR3 position;
	values->MainCamera->GetPosition(&position);

	D3DDesc info;
	D3D::GetDesc(&info);

	D3DXVECTOR2 point;
	point.x = ((2.0f*mouse.x) / info.Width) - 1.0f;
	point.y = (((2.0f*mouse.y) / info.Height) - 1.0f)*-1.0f;

	point.x = point.x / projection._11;
	point.y = point.y / projection._22;

	D3DXMATRIX invView;
	D3DXMatrixInverse(&invView, NULL, &view);

	D3DXVECTOR3 dir;
	dir.x = (point.x*invView._11) + (point.y*invView._21) + invView._31;
	dir.y = (point.x*invView._12) + (point.y*invView._22) + invView._32;
	dir.z = (point.x*invView._13) + (point.y*invView._23) + invView._33;

	D3DXVec3Normalize(&dir, &dir);

	*origin = position;
	*direction = dir;
}

void ExeModel::OpenConvertDialog(wstring file)
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	if (file.length() < 1)
	{
		function<void(wstring)> func = bind(&ExeModel::OpenConvertDialog, this, placeholders::_1);
		Path::OpenFileDialog(L"", Path::FbxModelFilter, FbxModels, func, desc.Handle);
	}
	else
	{
		selectedFbxFile = file;
		{
			wstring fileName = Path::GetFileNameWithoutExtension(file);

			function<void(wstring)> func = bind(&ExeModel::Convert, this, placeholders::_1);
			Path::SaveFileDialog(fileName, Path::BinModelFilter, BinModels, func, desc.Handle);
		}
		selectedFbxFile = L"";
	}
}

void ExeModel::Convert(wstring file)
{
	if (selectedFbxFile.length() < 1)
		return;

	wstring saveFile = file + L".model";
	MoLoader::Convert(String::WStringToString(selectedFbxFile), String::WStringToString(saveFile));

	D3DDesc desc;
	D3D::GetDesc(&desc);
	MessageBox(desc.Handle, L"변환 완료!", L"성공", MB_OK);
}

void ExeModel::OpenModelDialog(wstring file)
{
	if (file.length() < 1)
	{
		D3DDesc desc;
		D3D::GetDesc(&desc);

		function<void(wstring)> func = bind(&ExeModel::OpenModelDialog, this, placeholders::_1);
		Path::OpenFileDialog(L"", Path::BinModelFilter, BinModels, func, desc.Handle);
	}
	else
	{
		SAFE_DELETE(model);


		loadThread = new thread([&](wstring fileName)
		{
			isLoaded = false;

			MoLoader::LoadBinary(String::WStringToString(fileName), &model);
			isLoaded = true;
		}, file);

	}
}

void ExeModel::OpenModelFile(wstring file)
{

}