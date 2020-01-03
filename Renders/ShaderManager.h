#pragma once

	enum RenderMode{none=-1,depth =0,shadow,reflect,reflact,minimap,screen};
class ShaderManager{
public:
	static ShaderManager* Get();
	static void Delete();

	void Update();
	void PreRender(RenderMode isOther= none);
	void Render(UINT indexCount, bool isModel=false);

	int GetOther() { return isOther; }
	D3DXMATRIX GetMirrorView() { return mirrorView; }
	void AddShader(wstring file);
	ID3D11ShaderResourceView** GetResourceView(int num);

private:
	ShaderManager();
	~ShaderManager();

	static ShaderManager* instance;
	D3DXMATRIX view, projection,mirrorView;
	RenderMode isOther;
	class vector<RenderTarget*> renders;
	class vector<Shader*> shaders;
};