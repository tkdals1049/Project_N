#pragma once
#include "../Renders/ShaderBuffer.h"

class BrushBuffer : public ShaderBuffer
{
public:
	BrushBuffer() : ShaderBuffer(&data, sizeof(Data))
	{
		data.size = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
		data.point = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	void SetSize(D3DXVECTOR4& size)
	{
		data.size = size;
	}

	void SetPoint(D3DXVECTOR4& point)
	{
		data.point = point;
	}
	struct Data
	{
		D3DXVECTOR4 size;
		D3DXVECTOR4 point;
	};

private:
	Data data;
};
