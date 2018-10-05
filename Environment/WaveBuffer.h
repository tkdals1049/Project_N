#pragma once
#include "../stdafx.h"

class WaveBuffer : public ShaderBuffer
{
public:
	WaveBuffer()
		: ShaderBuffer(&Data,sizeof(Struct))
	{
		Data.wave = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	void SetWave(float& wave)
	{
		Data.wave.x=wave;
	}

	struct Struct
	{
		D3DXVECTOR4 wave;
	};

private:
	Struct Data;
};