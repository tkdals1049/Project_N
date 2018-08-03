#pragma once

enum Mode
{
	T = 0, T2, idle, slash, walk, jump, death, punch, muscle
};

class MoModel;
class MoMaterial;
class MoSkeleton;
class MoBoneWeights;
class MoLoader
{
public:
	static void Convert(string fbxFile, string saveFile);
	static void LoadBinary(string binFile, Model** model);
	static void WriteBinary(string binFile, Model* model);
	
private:
	MoLoader();
	~MoLoader();

	MoModel* Load(string file);

	void ProcessMaterial();
	void ProcessNode(FbxNode* node, FbxNodeAttribute::EType type);
	void ProcessSkeleton(FbxNode * node);
	void ProcessMesh(FbxNode* node);

	void ProcessAnimations();

	void ProcessAnimation(FbxNode * node, string takeName, float frameRate, float start, float stop);

	void ProcessBoneWeights(FbxMesh * mesh, vector<MoBoneWeights>& meshBoneWeights);
	void ProcessBoneWeights(FbxSkin * skin, vector<MoBoneWeights>& meshBoneWeights);

	MoMaterial* LinkMaterialWithPolygon
	(
		FbxMesh* mesh, int layerIndex
		, int polygonIndex, int polygonVertexIndex
		, int vertexIndex
	);

	int GetMappingIndex
	(
		FbxLayerElement::EMappingMode mode
		, int polygonIndex
		, int polygonVertexIndex
		, int vertexIndex
	);
	
	D3DXVECTOR2 GetUV
	(
		FbxMesh* mesh, int layerIndex
		, int polygonIndex, int polygonVertexIndex
		, int vertexIndex
	);

	MoMaterial* GetMaterial(FbxSurfaceMaterial* fbxMaterial);
	D3DXMATRIX GetGeometricOffset(FbxNode* node);

	D3DXMATRIX GetAbsoluteTransformFromCurrentTake(FbxNode * node, FbxTime time);

	MoModel* model;
	FbxManager* manager;
	FbxScene* scene;
	FbxImporter* importer;
	FbxGeometryConverter* converter;

	vector<FbxSurfaceMaterial *> materials;
};
