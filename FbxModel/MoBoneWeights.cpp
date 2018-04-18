#include "../stdafx.h"
#include "MoBoneWeights.h"

const UINT MoBoneWeights::MaxBonesPerVertex = 4;
const UINT MoBoneWeights::MaxBonesPerMax = 100;

MoBoneWeights::MoBoneWeights()
	: sumWeight(0.0f)
{

}

MoBoneWeights::MoBoneWeights(const MoBoneWeights & otherMoBoneWeights)
{
	this->sumWeight = otherMoBoneWeights.sumWeight;

	boneWeights.clear();
	boneWeights.assign(otherMoBoneWeights.boneWeights.begin(), otherMoBoneWeights.boneWeights.end());
}

MoBoneWeights::~MoBoneWeights()
{
}

void MoBoneWeights::AddBoneWeight(int boneIndex, float boneWeight)
{
	if (boneWeight <= 0.0f)
		return;

	sumWeight += boneWeight;

	// 중복체크
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

void MoBoneWeights::AddBoneWeight(std::pair<int, float> boneWeightPair)
{
	AddBoneWeight(boneWeightPair.first, boneWeightPair.second);
}

void MoBoneWeights::AddBoneWeights(const MoBoneWeights & boneWeights)
{
	for (int i = 0; i < boneWeights.GetBoneWeightCount(); ++i)
	{
		AddBoneWeight(boneWeights.GetBoneWeight(i));
	}
}

void MoBoneWeights::Validate()
{
	sumWeight = 0.0f;
	int i = 0;

	vector<pair<int, float>>::iterator it = boneWeights.begin();
	while (it != boneWeights.end())
	{
		// Bone Weight의 최대치를 넘어가면 최대치 이상의 BoneWeight 삭제
		if (i >= MaxBonesPerVertex)
		{
			it = boneWeights.erase(it);
		}
		else
		{
			// 최대치 안의 Weight값들만 합산
			sumWeight += it->second;

			++i; ++it;
		}
	}
}

void MoBoneWeights::Normalize()
{
	Validate(); //Bone Weight의 수가 유효하도록 설정
	
	float fScale = 1.0f / sumWeight;
	std::vector<std::pair<int, float>>::iterator it = boneWeights.begin();
	
	while (it != boneWeights.end())
	{
		it->second *= fScale;

		++it;
	}
}

MoBlendWeights MoBoneWeights::GetBlendWeights()
{
	MoBlendWeights blendWeights;

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