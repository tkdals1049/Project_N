#include "stdafx.h"
#include "BinModel.h"

BinModel * BinModel::instance = NULL;

BinModel * BinModel::Get()
{
	if (instance == NULL)
		instance = new BinModel();

	return instance;
}

void BinModel::Delete()
{
	SAFE_DELETE(instance);
}


BinModel::BinModel()
{
	Search("..\\_Contents\\BinModels");

	c_binmodelList = new const char*[binmodelList.size()];
	for (size_t i = 0; i < binmodelList.size(); i++)
	{
		c_binmodelList[i] = new char[binmodelList[i]->file.size() + 1];
		strcpy_s((char*)c_binmodelList[i], binmodelList[i]->file.size() + 1, binmodelList[i]->file.c_str());
	}
}

BinModel::~BinModel()
{
	for (size_t i = 0; i < binmodelList.size(); i++)
	{
		SAFE_DELETE(c_binmodelList[i]);
		SAFE_DELETE(binmodelList[i]);
	}
	SAFE_DELETE_ARRAY(c_binmodelList);
}

void BinModel::Search(string path)
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
				if (strstr(fd.name, "model"))	binmodelList.push_back(new BinModelList(name, path + "\\" + name));
				else Search(path + "\\" + name);
			}
		}

		result = _findnext(handle, &fd);
	}
	_findclose(handle);
}


string BinModel::GetBinModelFile(int num)
{
	return binmodelList[num]->file;
}

string BinModel::GetBinModelPath(int num)
{
	return binmodelList[num]->path;
}

void BinModel::Refresh()
{
	for (size_t i = 0; i < binmodelList.size(); i++)
	{
		SAFE_DELETE(c_binmodelList[i]);
		SAFE_DELETE(binmodelList[i]);
	}
	SAFE_DELETE_ARRAY(c_binmodelList);
	Search("..\\_Contents\\BinModels");

	c_binmodelList = new const char*[binmodelList.size()];
	for (size_t i = 0; i < binmodelList.size(); i++)
	{
		c_binmodelList[i] = new char[binmodelList[i]->file.size() + 1];
		strcpy_s((char*)c_binmodelList[i], binmodelList[i]->file.size() + 1, binmodelList[i]->file.c_str());
	}
	
}

string BinModel::Convert(string file)
{
	for each(BinModelList* temp in binmodelList)
	{
		if (file == temp->file)
			return temp->path;
	}

	return NULL;
}
