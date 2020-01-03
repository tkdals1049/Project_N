#pragma once

class FbxModel
{
	public:
	static FbxModel* Get();
	static void Delete();
	string GetFbxModelFile(int num);
	string GetFbxModelPath(int num);
	void Refresh();
	string Convert(string file);
	const char** GetFbxModelList() { return c_fbxmodelList; }
	UINT GetFbxModelSize() { return fbxmodelList.size(); }

	private:
	static FbxModel* instance;
	struct FbxModelList
	{
		FbxModelList(string file, string path)
		{
			this->file = file;
			this->path = path;
		}
		~FbxModelList()
		{
			file.clear();
			path.clear();
		}
		string file;
		string path;
	};
	 FbxModel();
	 void Search(string path);
	 ~FbxModel();
	vector<FbxModelList*> fbxmodelList;
	const char** c_fbxmodelList;
};