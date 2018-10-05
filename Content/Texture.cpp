#include "../stdafx.h"
#include "Texture.h"

Texture * Texture::instance = NULL;

Texture * Texture::Get()
{
	if (instance == NULL)
		instance = new Texture();

	return instance;
}

void Texture::Delete()
{
	SAFE_DELETE(instance);
}

Texture::Texture()
{
	Search("..\\_Contents\\Textures\\Diffuse");

	c_texturelist = new const char*[textureList.size()];
	for (size_t i = 0; i < textureList.size(); i++) 
	{
		c_texturelist[i] = new char[textureList[i].file.size() + 1];
		strcpy_s((char*)c_texturelist[i], textureList[i].file.size() + 1,textureList[i].file.c_str());
	}
}

void Texture::Search(string path)
{
	_finddata_t fd;
	long handle;
	int result = 1;
	string FindPath = path + "\\*.*";
	handle = _findfirst(FindPath.c_str(), &fd);  //현재 폴더 내 모든 파일을 찾는다.
	if (handle == -1)return;
	UINT num = 0;
	while (result != -1)
	{
		string name = fd.name;

		if (name.size() != 0)
		{
			if (name != "."&&name != "..")
			{
				if (strstr(fd.name, ".")!=NULL)
				{
					TextureList list;
					list.file=name;
					list.path=path + "\\" +name;
					textureList.push_back(list);
				}
				else
				{
					Search(path + "\\" + name);
				}
			}
		}

		result = _findnext(handle, &fd);
	}
	_findclose(handle);

}
Texture::~Texture()
{
}
string Texture::GetTexturePath(int num)
{
	return textureList[num].path;
}
string Texture::Convert(string file)
{
	for each(TextureList temp in textureList)
	{
		if (file == temp.file)
			return temp.path;
	}

	return NULL;
}

void Texture::Refresh()
{	
	textureList.clear();
	Search("..\\_Contents\\Textures\\Diffuse");

	c_texturelist = new const char*[textureList.size()];
	for (size_t i = 0; i < textureList.size(); i++) 
	{
		c_texturelist[i] = new char[textureList[i].file.size() + 1];
		strcpy_s((char*)c_texturelist[i], textureList[i].file.size() + 1, textureList[i].file.c_str());
	}
}
void Texture::LoadTexture(wstring fileName, ID3D11Texture2D** texture)
{
	HRESULT hr;

	ID3D11Texture2D* srcTexture;
	hr = D3DX11CreateTextureFromFile
	(
		D3D::GetDevice()
		, fileName.c_str()
		, NULL
		, NULL
		, (ID3D11Resource **)&srcTexture
		, NULL
	);
	assert(SUCCEEDED(hr));

	D3D11_TEXTURE2D_DESC srcTextureDesc;
	srcTexture->GetDesc(&srcTextureDesc);


	D3D11_TEXTURE2D_DESC destTextureDesc;
	ZeroMemory(&destTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	destTextureDesc.Width = srcTextureDesc.Width;
	destTextureDesc.Height = srcTextureDesc.Height;
	destTextureDesc.MipLevels = 1;
	destTextureDesc.ArraySize = 1;
	destTextureDesc.Format = srcTextureDesc.Format;
	destTextureDesc.SampleDesc.Count = 1;
	destTextureDesc.SampleDesc.Quality = 0;
	destTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	destTextureDesc.Usage = D3D11_USAGE_STAGING;

	ID3D11Texture2D* destTexture;
	hr = D3D::GetDevice()->CreateTexture2D(&destTextureDesc, NULL, &destTexture);
	assert(SUCCEEDED(hr));

	hr = D3DX11LoadTextureFromTexture
	(
		D3D::GetDC()
		, srcTexture
		, NULL
		, destTexture
	);
	assert(SUCCEEDED(hr));

	*texture = destTexture;

	SAFE_RELEASE(srcTexture);
}

void Texture::LoadPixel(ID3D11Texture2D * texture, UINT ** colors)
{
	HRESULT hr;

	D3D11_TEXTURE2D_DESC desc;
	texture->GetDesc(&desc);


	D3D11_MAPPED_SUBRESOURCE mapResource;
	hr = D3D::GetDC()->Map(texture, 0, D3D11_MAP_READ, NULL, &mapResource);
	assert(SUCCEEDED(hr));

	UINT width = mapResource.RowPitch / sizeof(UINT);
	UINT height = desc.Height;

	*colors = new UINT[width * height];
	memcpy(*colors, mapResource.pData, sizeof(UINT) * width * height);

	D3D::GetDC()->Unmap(texture, 0);
}