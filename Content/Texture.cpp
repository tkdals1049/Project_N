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
