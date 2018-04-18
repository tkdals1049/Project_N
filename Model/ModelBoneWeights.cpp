#include "../stdafx.h"
#include "ModelBoneWeights.h"

const UINT ModelBoneWeights::MaxBonesPerVertex = 4;
const UINT ModelBoneWeights::MaxBonesPerMax = 100;

ModelBoneWeights::ModelBoneWeights()
	: sumWeight(0.0f)
{

}

ModelBoneWeights::ModelBoneWeights(const ModelBoneWeights & otherModelBoneWeights)
{
	this->sumWeight = otherModelBoneWeights.sumWeight;

	boneWeights.clear();
	boneWeights.assign(otherModelBoneWeights.boneWeights.begin(), otherModelBoneWeights.boneWeights.end());
}

ModelBoneWeights::~ModelBoneWeights()
{
}

void ModelBoneWeights::AddBoneWeight(int boneIndex, float boneWeight)
{
	if (boneWeight <= 0.0f)
		return;

	sumWeight += boneWeight;

	// �ߺ�üũ
	bool isAdded = false;
	std::vector<std::pair<int, float>>::iterator it;
	for (it = boneWeights.begin(); it != boneWeights.end(); it++)
	{
		if (boneWeight > it->second)
		{
			boneWeights.insert(it, std::make_pair(boneIndex, boneWeight));
			isAdded = true;
			break;
		}
	}

	if (isAdded == false)
		boneWeights.push_back(make_pair(boneIndex, boneWeight));
}

void ModelBoneWeights::AddBoneWeight(std::pair<int, float> boneWeightPair)
{
	AddBoneWeight(boneWeightPair.first, boneWeightPair.second);
}

void ModelBoneWeights::AddBoneWeights(const ModelBoneWeights & boneWeights)
{
	for (int i = 0; i < boneWeights.GetBoneWeightCount(); ++i)
	{
		AddBoneWeight(boneWeights.GetBoneWeight(i));
	}
}

/********************************************************************************
 @brief
 Vertex�� ������ �ִ� Bone�� ���� �ִ�ġ�� ���� �ʵ��� ����
********************************************************************************/
void ModelBoneWeights::Validate()
{
	sumWeight = 0.0f;
	int i = 0;

	vector<pair<int, float>>::iterator it = boneWeights.begin();
	while (it != boneWeights.end())
	{
		// Bone Weight�� �ִ�ġ�� �Ѿ�� �ִ�ġ �̻��� BoneWeight ����
		if (i >= MaxBonesPerVertex)
		{
			it = boneWeights.erase(it);
		}
		else
		{
			// �ִ�ġ ���� Weight���鸸 �ջ�
			sumWeight += it->second;

			++i; ++it;
		}
	}
}

/********************************************************************************
 @brief
 �� BoneWeight���� �� BoneWeight�� �տ� ���� ������ ��ȯ
 ��� BoneWeight�� ���� ���� 1�� �ǵ��� ����
********************************************************************************/
void ModelBoneWeights::Normalize()
{
	Validate(); //Bone Weight�� ���� ��ȿ�ϵ��� ����
	
	float fScale = 1.0f / sumWeight;
	std::vector<std::pair<int, float>>::iterator it = boneWeights.begin();
	
	while (it != boneWeights.end())
	{
		it->second *= fScale;

		++it;
	}
}

/********************************************************************************
 @brief
 ��� Bone Weight ������ �ϳ��� Blend Weights Class �����Ͽ� ��ȯ
********************************************************************************/
ModelBlendWeights ModelBoneWeights::GetBlendWeights()
{
	ModelBlendWeights blendWeights;

	int i = 0;
	vector<pair<int, float>>::iterator it = boneWeights.begin();
	while (it != boneWeights.end())
	{
		blendWeights.SetBlendWeight(i, it->first, it->second);
		++i;
		++it;

		if (i >= MaxBonesPerVertex)
			break;
	}

	return blendWeights;
}