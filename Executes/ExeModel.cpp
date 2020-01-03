#include "stdafx.h"
#include "ExeModel.h"

#include "../Model/ModelGroup.h"
#include "../Utilities/Path.h"

ExeModel::ExeModel()
	: isEnviroment(false),isModel(false), loadThread(NULL), isLoaded(false),isWire(false)
{
	CreateSingleTon();

	group=new ModelGroup();
	ShaderManager::Get();

	music = new MusicPlayer();
	music2 = new MusicPlayer();

	music->SetSong(Sounds+"BGM.ogg");
	music2->SetSong(Sounds + "BGM2.ogg");

	States::SetDefault();
}
void ExeModel::CreateSingleTon()
{
	Texture::Get();
	BinModel::Get();
	FbxModel::Get();
	Sky::Get();
	Plane::Get();
	Ui::Get();
	ShaderManager::Get();
}
ExeModel::~ExeModel()
{
	SAFE_DELETE(music);
	SAFE_DELETE(music2);
	SAFE_DELETE(group);
	SAFE_DELETE(loadThread);

	DeleteSingleTon();
}
void ExeModel::DeleteSingleTon()
{
	Texture::Delete();
	BinModel::Delete();
	FbxModel::Delete();
	Sky::Delete();
	Plane::Delete();
	Ui::Delete();
	ShaderManager::Delete();
}

void ExeModel::Update()
{
	static bool isplay1 = false, isplay2 = false;
	if (Keyboard::Get()->Down('Y'))
	{
		if (isplay1) music->Stop();
		else music->Start();
		isplay1 = !isplay1;
	}
	if (Keyboard::Get()->Down('H'))
	{
		if (isplay2) music2->Stop();
		else music2->Start();
		isplay2 = !isplay2;
	}
	if (Keyboard::Get()->Down('N')) isWire = !isWire;

	D3DXVECTOR3 origin, direction;
	GetRay(&origin,&direction);

	Sky::Get()->Update();
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

	Ui::Get()->Update();
}

void ExeModel::PreRender()
{	
	ShaderManager::Get()->PreRender(depth);
	{
		Plane::Get()->Render();
		group->Render(); 
	}
	CameraManager::Get()->DefaultCamera();
	
	ShaderManager::Get()->PreRender(shadow);
	{
		Plane::Get()->Render();
	}
	
	ShaderManager::Get()->PreRender(reflect);
	{
		Sky::Get()->Render();
	
		States::SetRasterizerNone();
		{
			Plane::Get()->Render();
			group->Render();
		}
		States::SetRasterizerDefault();
	}
	CameraManager::Get()->DefaultCamera();
	
	ShaderManager::Get()->PreRender(reflact);
	{
		Sky::Get()->Render();
		Plane::Get()->ReverseClip();
		Plane::Get()->Render();
		Plane::Get()->ReverseClip();
		group->Render();
	}

	ShaderManager::Get()->PreRender(minimap);
	{
		Plane::Get()->Render();
		Plane::Get()->ReverseClip();
		Plane::Get()->Render();
		Plane::Get()->ReverseClip();
	}
	CameraManager::Get()->DefaultCamera();
	
	ShaderManager::Get()->PreRender(screen);
	{
		Sky::Get()->Render();

		if (isWire == false)States::SetRasterizerDefault();
		else States::SetRasterizerWire();
		{
			Plane::Get()->Render();
			Plane::Get()->waterRender();
			group->Render();
		}
	}

	D3D::Get()->SetRenderTarget();
	ShaderManager::Get()->PreRender();
}

void ExeModel::Render()
{
	Ui::Get()->Render();
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
		}

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

	
	D3DXMATRIX view=CameraManager::Get()->GetView();
	D3DXMATRIX projection= CameraManager::Get()->GetProj();
	D3DXVECTOR3 position=CameraManager::Get()->GetPosition();

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