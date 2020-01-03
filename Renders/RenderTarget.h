#pragma once

class RenderTarget
{
public:
	RenderTarget(DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT);
	~RenderTarget();

	ID3D11ShaderResourceView** GetResourceView()
	{
		return &resourceView;
	}

	void SetTarget();
	void Clear(D3DXCOLOR color = D3DXCOLOR(0xFF555566));

	void SaveTexture(wstring file);

private:
	ID3D11Texture2D* texture;
	ID3D11RenderTargetView* renderView;

	ID3D11ShaderResourceView* resourceView;
};