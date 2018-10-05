#pragma once

class RenderTarget;
class PerlinNoise
{
public:
	PerlinNoise();
	~PerlinNoise();

	void MakePerlinNoise(float r = 1, float g = 1, float b = 1);
	void Render();
	void ReadyToRender();
	ID3D11ShaderResourceView** GetPerlinNoise();

	RenderTarget* rndNoise;
private:

	bool renderReady;

	D3DXCOLOR color;
	ID3D11Buffer*		colorBuffer;

	void CreateShader(wstring file);
	void CreateBuffer();


	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader*	pixelShader;
	ID3D11InputLayout*	layout;

	ID3D10Blob*			vertexBlob;
	ID3D10Blob*			pixelBlob;
	ID3D11Buffer*		vertexBuffer;
	


	ID3D11Buffer*		indexBuffer;

	wstring basePath;

	ID3D11ShaderResourceView* finalResult;


	ID3D11VertexShader* vertexShaderfinal;
	ID3D11PixelShader*	pixelShaderfinal;
	ID3D11InputLayout*	layoutfinal;
	ID3D11Buffer*		vertexBufferfinal;
};