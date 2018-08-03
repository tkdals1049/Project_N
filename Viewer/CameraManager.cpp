#include "../stdafx.h"
#include "CameraManager.h"
#include "Camera.h"

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



CameraManager::CameraManager():isAuto(false)
{
}


CameraManager::~CameraManager()
{
}

