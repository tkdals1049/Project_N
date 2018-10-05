#include "../stdafx.h"
#include "CameraManager.h"
#include "Camera.h"
#include "Perspective.h"

CameraManager* CameraManager::instance = NULL;
CameraManager * CameraManager::Get()
{
	if (instance == NULL)
		instance = new CameraManager();

	return instance;
}

void CameraManager::Delete()
{
	SAFE_DELETE(instance);
}



CameraManager::CameraManager():isAuto(false),isRe(false)
{
}


CameraManager::~CameraManager()
{
}



D3DXVECTOR3 CameraManager::GetPosition()
{
	if(!isRe)
	{
		D3DXVECTOR3 position; 
		playerCamera->GetPosition(&position); 
		return position;
	}
	else
	{
		return RePosition;
	}
}

D3DXVECTOR2 CameraManager::GetRotate()
{
	if (!isRe)
	{
		D3DXVECTOR2 rotate;		
		playerCamera->GetRotation(&rotate); 
		return rotate; 
	}
	else
	{
		return ReRotation;
	}
}

D3DXMATRIX CameraManager::GetView()
{
	D3DXMATRIX view;
	playerCamera->GetMatrix(&view);
	return view;
}

D3DXMATRIX CameraManager::GetProj()
{
	D3DXMATRIX proj;
	perspective->GetMatrix(&proj);
	return proj;
}

D3DXMATRIX CameraManager::RenderReflection(float depth, D3DXVECTOR3 normal)
{
	D3DXVECTOR3 forward, right, up, position, no, pPosition;
	D3DXVECTOR2 pRotate;

	playerCamera->GetPosition(&pPosition);
	playerCamera->GetRotation(&pRotate);

	D3DXVec3Normalize(&no, &normal);

	if (no.x == 0)position.x = pPosition.x;
	else  position.x = pPosition.x -2 * no.x*depth;
	if (no.y == 0)position.y = pPosition.y;
	else  position.y = pPosition.y -2 * no.y*depth;
	if (no.z == 0)position.z = pPosition.z;
	else  position.z = pPosition.z -2 * no.z*depth;

	D3DXMATRIX x, y, rotation, view;
	D3DXMatrixRotationX(&x, -pRotate.x);
	D3DXMatrixRotationY(&y, pRotate.y);

	rotation = x * y;

	D3DXVec3TransformCoord(&forward, &D3DXVECTOR3(0, 0, 1), &rotation);
	D3DXVec3TransformCoord(&right, &D3DXVECTOR3(1, 0, 0), &rotation);
	D3DXVec3TransformCoord(&up, &D3DXVECTOR3(0, 1, 0), &rotation);

	D3DXMatrixLookAtLH(&view, &position, &(position + forward), &up);

	VPbuffer->SetView(view);
	VPbuffer->SetVSBuffer(0);
	
	isRe = true;
	RePosition = position;
	ReRotation=D3DXVECTOR2(-pRotate.x, pRotate.y);

	return view;
}

void CameraManager::SetViewProjection(D3DXMATRIX view, D3DXMATRIX projection)
{
	VPbuffer->SetView(view);
	VPbuffer->SetProjection(projection);
	VPbuffer->SetVSBuffer(0);
}

void CameraManager::DefaultCamera() 
{ 
	playerCamera->UpdateView();
	isRe = false;
	D3DXMATRIX view,projection;
	playerCamera->GetMatrix(&view);
	perspective->GetMatrix(&projection);
	VPbuffer->SetView(view);
	VPbuffer->SetProjection(projection);
	VPbuffer->SetVSBuffer(0);
	D3DXVec3TransformCoord(&CameraPos, &D3DXVECTOR3(0, 0, 0), &view);
}
