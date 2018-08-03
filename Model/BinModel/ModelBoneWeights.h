#pragma once

struct ModelBlendWeights
{
	D3DXVECTOR4 BlendIndices;
	D3DXVECTOR4 BlendWeights;

	ModelBlendWeights()
	{
		BlendIndices = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
		BlendWeights = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
	}

	void SetBlendWeight(int nIndex, int nBoneIndex, float fWeight)
	{
		switch (nIndex)
		{
			case 0:
				BlendIndices.x = (float)nBoneIndex;
				BlendWeights.x = fWeight;
				break;
			case 1:
				BlendIndices.y = (float)nBoneIndex;
				BlendWeights.y = fWeight;
				break;
			case 2:
				BlendIndices.z = (float)nBoneIndex;
				BlendWeights.z = fWeight;
				break;
			case 3:
				BlendIndices.w = (float)nBoneIndex;
				BlendWeights.w = fWeight;
				break;
		}
	}
};

class ModelBoneWeights
{
public:
	ModelBoneWeights();
	ModelBoneWeights(const ModelBoneWeights& otherModelBoneWeights);
	~ModelBoneWeights();

	static const UINT MaxBonesPerVertex; /// �� Vertex�� ������ ��ġ�� �ִ� Bone�� ��
	static const UINT MaxBonesPerMax; /// �ִ� Bone�� ��

	void AddBoneWeight(int boneIndex, float boneWeight);
	void AddBoneWeight(pair<int, float> boneWeightPair);
	void AddBoneWeights(const ModelBoneWeights& boneWeights);

	void Validate();
	void Normalize();

	int GetBoneWeightCount() const { return boneWeights.size(); }
	pair<int, float> GetBoneWeight(int index) const { return boneWeights[index]; }
	ModelBlendWeights GetBlendWeights();

private:
	float sumWeight; /// �� Vertex�� ������ �ִ� ��� Wieght�� ��

	vector<pair<int, float>> boneWeights; /// Bone Index�� Weight���� Pair�� �����ϴ� ����
};