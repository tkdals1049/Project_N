#include "stdafx.h"
#include "ExeModel.h"
#include "../Content/Texture.h"
#include "../Content/BinModel.h"
#include "../Content/FbxModel.h"
#include "../Model/ModelGroup.h"
#include "../FbxModel/MoLoader.h"
#include "../Utilities/Path.h"

ExeModel::ExeModel(ExecuteValues* values)
	: Execute(values),isEnviroment(false),isModel(false), loadThread(NULL), isLoaded(false)
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	Texture::Get();
	BinModel::Get();
	FbxModel::Get();

	group=new ModelGroup();
	ImGui::SetNextWindowPos(ImVec2(800, 500)); 

	D3D11_RASTERIZER_DESC rdesc;
	States::GetRasterizerDesc(&rdesc);

	rdesc.FillMode = D3D11_FILL_SOLID;
	rdesc.CullMode = D3D11_CULL_NONE;
	States::CreateRasterizer(&rdesc, &setRasterizer);
}

ExeModel::~ExeModel()
{
}

void ExeModel::Update()
{

	D3DXVECTOR3 origin, direction;
	GetRay(&origin,&direction);

	Sky::Get()->Update(values->MainCamera);
	Plane::Get()->UpdatePointBuffer(origin,direction);
	Plane::Get()->Update();

	group->SetDot(Plane::Get()->GetDot());
	group->PreUpdate(origin,direction);
	group->Update();

	if (isLoaded == true)
	{
		if (loadThread != NULL)
		{
			loadThread->join();
			SAFE_DELETE(loadThread);
		}
	}	
}

void ExeModel::PreRender()
{
	
}

void ExeModel::Render()
{
	Sky::Get()->Render();
	Plane::Get()->Render();

	D3D::GetDC()->RSGetState(&getRasterizer);
	D3D::GetDC()->RSSetState(setRasterizer);
	{
		group->Render();
	}
	D3D::GetDC()->RSSetState(getRasterizer);
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

			if (ImGui::MenuItem("Save", "Ctrl+S"))
				SaveModelDialog();

			ImGui::EndMenu();
		}//if(BeiginMenu)
		if (ImGui::BeginMenu("Tool"))
		{
			if (ImGui::MenuItem("Envireoment", "Ctrl+E"))
			{
				isEnviroment=!isEnviroment;
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Model", "Ctrl+M"))
			{
				isModel=!isModel;
			}

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	if(isEnviroment==true)
	{
	Sky::Get()->PostRender(isEnviroment);
	Plane::Get()->PostRender(isEnviroment);
	}
	if(isModel)
	{
	group->PostRender(isModel);
	}
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
	//MessageBox(desc.Handle, L"변환 완료!", L"성공", MB_OK);
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
		loadThread = new thread([&](wstring fileName)
		{
			isLoaded = false;
			group->SetModel(String::WStringToString(fileName));
			isLoaded = true;
		}, file);
	}
}

void ExeModel::SaveModelDialog(wstring file)
{
	if (file.length() < 1)
	{
		D3DDesc desc;
		D3D::GetDesc(&desc);

		function<void(wstring)> func = bind(&ExeModel::SaveModelDialog, this, placeholders::_1);
		Path::SaveFileDialog(L"", Path::BinModelFilter, BinModels, func, desc.Handle);
	}
	else
	{
		MoLoader::WriteBinary(String::WStringToString(file + L".model"),group->GetAttitude());
	}
}