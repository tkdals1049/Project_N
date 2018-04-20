#pragma once

class BinModel
{
	public:
	static BinModel* Get();
	static void Delete();
	string GetBinModelFile(int num);
	string GetBinModelPath(int num);
	void Refresh();
	string Convert(string file);
	const char** GetBinmodelList() { return c_binmodelList; }
	UINT GetBinmodelPathSize() { return binmodelList.size(); }

	private:

	static BinModel* instance;
	struct BinModelList
	{
		string file;
		string path;
	};
	 BinModel();
	 void Search(string path);
	 ~BinModel();
	vector<BinModelList> binmodelList;
	const char** c_binmodelList;
};