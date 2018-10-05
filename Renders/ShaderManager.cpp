
#include "../stdafx.h"
#include "ShaderManager.h"

ShaderManager* ShaderManager::instance = NULL;
ShaderManager * ShaderManager::Get()
{
	if (instance == NULL)
		instance = new ShaderManager();

	return instance;
}

void ShaderManager::Delete()
{
	SAFE_DELETE(instance);
}

ShaderManager::ShaderManager():isOther(none)
{
	RenderTarget* render = new RenderTarget();
	renders.push_back(render);
	RenderTarget* render2 = new RenderTarget();
	renders.push_back(render2);
	RenderTarget* render3 = new RenderTarget();
	renders.push_back(render3);
	RenderTarget* render4 = new RenderTarget();
	renders.push_back(render4);
	Shader*	shader = new Shader(Shaders + L"LightDepthShader.hlsl");
	shaders.push_back(shader);
	Shader*	shader2 = new Shader(Shaders + L"Shadow.hlsl");
	shaders.push_back(shader2);
	Shader*	shader3 = new Shader(Shaders + L"LightDepthModelShader.hlsl");
	shaders.push_back(shader3);
	Shader*	shader4 = new Shader(Shaders + L"ModelShadow.hlsl");
	shaders.push_back(shader4);

	D3DXMatrixIdentity(&view);
	D3DXMatrixIdentity(&mirrorView);
	D3DXMatrixIdentity(&projection);
	
}

ShaderManager::~ShaderManager()
{
	for (size_t i = 0; i < renders.size(); i++)
		SAFE_DELETE(renders[i]);
	for (size_t i = 0; i < shaders.size(); i++)
		SAFE_DELETE(shaders[i]);
}

void ShaderManager::Update()
{

}

void ShaderManager::PreRender(RenderMode isOther)
{
	this->isOther = isOther;
	if (isOther == none) { return; }
	else if (isOther == depth)
	{
		//CameraManager::Get()->SetViewProjection(view, projection);
	}
	else if (isOther == reflect) 
	{
		D3DXVECTOR3 position = CameraManager::Get()->GetPosition();
		mirrorView=CameraManager::Get()->RenderReflection(position.y +2);
	}
	renders[isOther]->SetTarget();
	renders[isOther]->Clear();
}

void ShaderManager::Render(UINT indexCount,bool isModel)
{
	shaders[2*isModel+isOther]->Render();
	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
}


void ShaderManager::AddShader(wstring file)
{
	Shader*	shader = new Shader(Shaders + file);
	shaders.push_back(shader);
}

ID3D11ShaderResourceView ** ShaderManager::GetResourceView(int num)
{
	return renders[num]->GetResourceView();
}
