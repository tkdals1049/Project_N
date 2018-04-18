#include "../stdafx.h"
#include "MoAnimationController.h"
#include "MoAnimation.h"

MoAnimationController::MoAnimationController()
{
	currentAnimation = NULL;
	currentKeyFrame = 0;
	nextKeyFrame = 0;
	keyFrameFactor = 0.0f;
	frameTimer = 0.0f;
	speed = 1.0f;
	mode = 0;
	useQuaternionKeyFrames = true;
}

MoAnimationController::~MoAnimationController()
{
	for each(Pair temp in animations)
		SAFE_DELETE(temp.second);

	currentAnimation = NULL;
}

void MoAnimationController::AddAnimation(MoAnimation * animation)
{
	bool isExist = false;
	for (size_t i = 0; i < animations.size(); i++)
	{
		if (animations[i].first == animation->GetName())
		{
			isExist = true;

			break;
		}
	}
	assert(isExist == false);

	animations.push_back(Pair(animation->GetName(), animation));
}


UINT MoAnimationController::GetAnimationCount()
{
	return animations.size();
}

MoAnimation * MoAnimationController::GetAnimation(int index)
{
	return animations[index].second;
}

void MoAnimationController::Wirte(BinaryWriter * w)
{
	int a = animations.size();
	w->UInt(animations.size());

	for (size_t i = 0; i < animations.size(); i++)
	{
		w->String(animations[i].first);
		(animations[i].second)->Write(w);
	}
}