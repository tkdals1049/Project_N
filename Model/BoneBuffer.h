#pragma once
#include "../Renders/ShaderBuffer.h"

class BoneBuffer : public ShaderBuffer
{
public:
	BoneBuffer()
		: ShaderBuffer(&data,sizeof(Data))
	{
		D3DXMatrixIdentity(&data.boneTrans);
		D3DXMatrixIdentity(&data.boneScale);
		
		for(int i = 0; i < 100; i++)
			D3DXMatrixIdentity(&data.boneArray[i]);

		data.isSkinning = 0;
	}

	void SetSkinning(bool isSkinning)
	{
		data.isSkinning = (UINT)isSkinning;
	}
	void SetBoneArray(D3DXMATRIX* matrix, UINT count)
	{
		memcpy(data.boneArray, matrix, count * sizeof(D3DXMATRIX));

		for(UINT i = 0; i < count; i++)
			D3DXMatrixTranspose(&data.boneArray[i], &data.boneArray[i]);
	}
	void SetScale(D3DXMATRIX* matrix)
	{
		memcpy(data.boneScale, matrix, sizeof(D3DXMATRIX));
	}
	void SetTrans(D3DXMATRIX* matrix)
	{
		memcpy(data.boneTrans, matrix, sizeof(D3DXMATRIX));
	}
	struct Data
	{
		D3DXMATRIX boneTrans;
		D3DXMATRIX boneScale;
		D3DXMATRIX boneArray[100];
		UINT isSkinning;
		D3DXVECTOR3 padding;
	};

private:
	Data data;
};