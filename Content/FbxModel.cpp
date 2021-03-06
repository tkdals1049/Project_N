#include "stdafx.h"
#include "FbxModel.h"

FbxModel * FbxModel::instance = NULL;

FbxModel * FbxModel::Get()
{
	if (instance == NULL)
		instance = new FbxModel();

	return instance;
}

void FbxModel::Delete()
{
	SAFE_DELETE(instance);
}


FbxModel::FbxModel()
{
	Search("../_Contents/FbxModels/Animation");

	c_fbxmodelList = new const char*[fbxmodelList.size()];
	for (size_t i = 0; i < fbxmodelList.size(); i++)
	{
		c_fbxmodelList[i] = new char[fbxmodelList[i]->file.size() + 1];
		strcpy_s((char*)c_fbxmodelList[i], fbxmodelList[i]->file.size() + 1, fbxmodelList[i]->file.c_str());
	}

}
FbxModel::~FbxModel()
{
	for (size_t i = 0; i < fbxmodelList.size(); i++)
	{
		SAFE_DELETE(c_fbxmodelList[i]);
		SAFE_DELETE(fbxmodelList[i]);
	}
	SAFE_DELETE_ARRAY(c_fbxmodelList);
}

void FbxModel::Search(string path)
{
	_finddata_t fd;
	long handle;
	int result = 1;
	string FindPath=path+"/*.*";
	handle = _findfirst(FindPath.c_str(), &fd);  //현재 폴더 내 모든 파일을 찾는다.
	if (handle == -1)return;
	UINT num = 0;
	while (result != -1)
	{
		string name = fd.name;

		if (name.size()!=0)
		{
			if(name!="."&&name!="..")
			{
				if (strstr(fd.name, "fbx")|| strstr(fd.name, "FBX"))fbxmodelList.push_back((new FbxModelList(name, path + "\\" + name)));
				else Search(path+"/"+name);
			}
		}
		
		result = _findnext(handle, &fd);
	}
	_findclose(handle);

}


string FbxModel::GetFbxModelFile(int num)
{
	return fbxmodelList[num]->file;
}

string FbxModel::GetFbxModelPath(int num)
{
	return fbxmodelList[num]->path;
}

void FbxModel::Refresh()
{
	for (size_t i = 0; i < fbxmodelList.size(); i++)
	{
		SAFE_DELETE(c_fbxmodelList[i]);
		SAFE_DELETE(fbxmodelList[i]);
	}
	SAFE_DELETE_ARRAY(c_fbxmodelList);

	Search("../_Contents/FbxModels/Animation");
	c_fbxmodelList = new const char*[fbxmodelList.size()];
	for (size_t i = 0; i < fbxmodelList.size(); i++)
	{
		c_fbxmodelList[i] = new char[fbxmodelList[i]->file.size() + 1];
		strcpy_s((char*)c_fbxmodelList[i], fbxmodelList[i]->file.size() + 1, fbxmodelList[i]->file.c_str());
	}

}

string FbxModel::Convert(string file)
{
	for each(FbxModelList* temp in fbxmodelList)
	{
		if (file == temp->file)
			return temp->path;
	}

	return NULL;
}
