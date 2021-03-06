#include "stdafx.h"
#include "Program.h"

#include "./Viewer/FirstPerson.h"
#include "./Executes/Execute.h"
#include "./Executes/ExeGui.h"
#include "./Executes/ExeModel.h"

Program::Program()
{
	States::Create(); 
	ZFrustum::Get();
	CameraManager::Get();
	LightManager::Get();
	Plane::Get();
	Sky::Get();

	D3DDesc desc;
	D3D::GetDesc(&desc); 

	values = new ExecuteValues();
	values->GlobalLight = new GlobalLightBuffer();
	values->ViewProjection = new ViewProjectionBuffer();
	values->Perspective = new Perspective(desc.Width, desc.Height);
	values->Viewport = new Viewport(desc.Width, desc.Height);
	values->MainCamera = new FirstPerson();
	values->MainCamera->SetPosition(0, 10, -20);
	LightManager::Get()->SetBuffer(values->GlobalLight);
	
	CameraManager::Get()->SetPlayerCamera(values->MainCamera);
	CameraManager::Get()->SetViewProjectionBuffer(values->ViewProjection);
	CameraManager::Get()->SetPerspective(values->Perspective);
	
	executes.push_back(new ExeModel());
}

Program::~Program()
{
	for (Execute* exe : executes)
		SAFE_DELETE(exe);

	SAFE_DELETE(values->GlobalLight);
	SAFE_DELETE(values->ViewProjection);
	SAFE_DELETE(values->Perspective);
	SAFE_DELETE(values->MainCamera);
	SAFE_DELETE(values->Viewport);
	SAFE_DELETE(values);

	CameraManager::Delete();
	ZFrustum::Delete();
	States::Delete();
	LightManager::Delete();
	Plane::Delete();
	Sky::Delete();
}

void Program::Update()
{
	values->MainCamera->Update();
	
	D3DXMATRIX view, projection;
	values->MainCamera->GetMatrix(&view);
	values->Perspective->GetMatrix(&projection);
	ZFrustum::Get()->Make(&(view*projection));
	
	values->ViewProjection->SetView(view);
	values->ViewProjection->SetProjection(projection);
	values->ViewProjection->SetVSBuffer(0);
	
	LightManager::Get()->Update();
	values->GlobalLight->SetVSBuffer(2);

	for (Execute* exe : executes)
		exe->Update();
}

void Program::PreRender()
{
	for (Execute* exe : executes)
		exe->PreRender();
}

void Program::Render()
{

	for (Execute* exe : executes)
		exe->Render();
}

void Program::PostRender()
{
	for (Execute* exe : executes)
		exe->PostRender();
}

void Program::ResizeScreen()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);
	
	values->Perspective->Set(desc.Width, desc.Height);
	values->Viewport->Set(desc.Width, desc.Height);

	for (Execute* exe : executes)
		exe->ResizeScreen();
}
