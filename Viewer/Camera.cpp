#include "stdafx.h"
#include "Camera.h"

Camera::Camera()
	: position(0, 0, 0), rotation(0, 0)
	, forward(0, 0, 1), right(1, 0, 0), up(0, 1, 0)
{
	D3DXMatrixIdentity(&matRotation);
	D3DXMatrixIdentity(&matView);

	UpdateRotation();
	UpdateView();
}

Camera::~Camera()
{

}

void Camera::GetMatrix(D3DXMATRIX * mat)
{
	memcpy(mat, this->matView, sizeof(D3DXMATRIX));
}

void Camera::SetMatrix(D3DXMATRIX * mat)
{
	memcpy( this->matView, mat, sizeof(D3DXMATRIX));
}

//카메라에 흔들림 효과를 주기 위해 지진 파라미터를 추가함
void Camera::UpdateRotation(D3DXVECTOR2 quike)
{
	D3DXMATRIX x, y;
	D3DXMatrixRotationX(&x, rotation.x+quike.x);
	D3DXMatrixRotationY(&y, rotation.y+quike.y);

	matRotation = x * y;

	D3DXVec3TransformCoord(&forward, &D3DXVECTOR3(0, 0, 1), &matRotation);
	D3DXVec3TransformCoord(&right, &D3DXVECTOR3(1, 0, 0), &matRotation);
	D3DXVec3TransformCoord(&up, &D3DXVECTOR3(0, 1, 0), &matRotation);
}

void Camera::UpdateView()
{
	D3DXMatrixLookAtLH(&matView, &position, &(position + forward), &up);
}