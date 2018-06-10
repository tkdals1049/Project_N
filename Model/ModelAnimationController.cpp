#include "../stdafx.h"
#include "ModelAnimationController.h"
#include "ModelAnimation.h"

ModelAnimationController::ModelAnimationController()
	: currentAnimation(NULL), prevAnimation(NULL)
	, currentKeyFrame(0), nextKeyFrame(0), keyFrameFactor(0.0f), frameTimer(0.0f)
{
	useQuaternionKeyFrames = true;

	animationMode = AnimationMode::Stop;
}

ModelAnimationController::~ModelAnimationController()
{
	for each(Pair temp in animations)
		SAFE_DELETE(temp.second);
	
	currentAnimation = NULL;
}
void ModelAnimationController::AddAnimation(ModelAnimation * animation)
{
	bool isExist = false;
	for (size_t i = 0; i < animations.size(); i++)
	{
		// 중복 확인
		if (animations[i].first == animation->GetName())
		{
			isExist = true;
			
			break;
		}
	}
	assert(isExist == false);

	animations.push_back(Pair(animation->GetName(), animation));
}

ModelAnimation * ModelAnimationController::GetCurrentAnimation()
{
	if(currentAnimation != NULL)
		return currentAnimation;

	return NULL;
}
void ModelAnimationController::SetCurrentAnimation(string animationName)
{
	UINT count = -1;
	for (size_t i = 0; i < animations.size(); i++)
	{
		if (animations[i].first == animationName)
		{
			count = i;
			break;
		}
	}
	
	if (count != -1)
	{
		if (currentAnimation == animations[count].second)
			return;

		// 현재 애니메이션 교체, 이전 애니메이션 저장
		prevAnimation = currentAnimation;
		currentAnimation = animations[count].second;
	}
	else
		return;

	// 현재 Animation이 바뀌면 Animation 실행 중지
	Stop();
}

void ModelAnimationController::SetCurrentAnimation(int index)
{
	if( index < 0 || index >= (int)animations.size() )
		currentAnimation = NULL;
	else
	{
		prevAnimation = currentAnimation;
		currentAnimation = animations[index].second;
	}
	
	Stop();
}

UINT ModelAnimationController::GetAnimationCount()
{
	return animations.size();
}

ModelAnimation * ModelAnimationController::GetAnimation(int index)
{
	if( index < 0 || index >= (int)animations.size() )
		return NULL;
	else
		return animations[index].second;
}

int ModelAnimationController::GetAnimationNum()
{
	for (size_t i = 0; i < animations.size(); i++)
	{
		if(animations[i].second==currentAnimation)
		return i;
	}
	return -1;
}

ModelAnimation * ModelAnimationController::GetAnimation(string animName)
{
	UINT count = -1;
	for (size_t i = 0; i < animations.size(); i++)
	{
		if (animations[i].first == animName)
		{
			count = i;
			break;
		}
	}

	if (count != -1)
	{
		return animations[count].second;
	}
	else
		return NULL;
}

int ModelAnimationController::GetCurrentAnimationCount()
{
	return currentAnimation->GetKeyFrames();
}

void ModelAnimationController::Play()
{
	animationMode = AnimationMode::Play;
}

void ModelAnimationController::Pause()
{
	animationMode = AnimationMode::Pause;
}

void ModelAnimationController::Stop()
{
	animationMode = AnimationMode::Stop;
	frameTimer = 0.0f;

	SetCurrentKeyFrame(0);
}

void ModelAnimationController::SetCurrentKeyFrame(int keyFrame)
{
	currentKeyFrame = (keyFrame) % currentAnimation->GetKeyFrames();
	nextKeyFrame = (currentKeyFrame + 1) % currentAnimation->GetKeyFrames();
	
	keyFrameFactor = 0.0f;
}

void ModelAnimationController::Update(float speed)
{
	if (currentAnimation == NULL || animationMode != AnimationMode::Play)
		return;

	frameTimer += Time::Get()->Delta()*speed;

	float invFrameRate = 1.0f / currentAnimation->GetFrameRate();
	while (frameTimer > invFrameRate)
	{

		currentKeyFrame = (currentKeyFrame + 1) % currentAnimation->GetKeyFrames();
		nextKeyFrame = (currentKeyFrame + 1) % currentAnimation->GetKeyFrames();

		frameTimer -= invFrameRate;
	}
	keyFrameFactor = frameTimer / invFrameRate;
}

void ModelAnimationController::Write(BinaryWriter * w, ModelAnimationController* modelAnimationController)
{
	w->UInt(modelAnimationController->animations.size());

	for (size_t i = 0; i < modelAnimationController->animations.size(); i++)
	{
		w->String(modelAnimationController->animations[i].first);
		(modelAnimationController->animations[i].second)->Write(w, modelAnimationController->animations[i].second);
	}
}

void ModelAnimationController::Read(BinaryReader * r, ModelAnimationController* modelAnimationController)
{
	UINT animationKeyFramesCount =r->UInt();

	for (size_t i = 0; i < animationKeyFramesCount; i++)
	{
		string name = r->String();

		ModelAnimation* modelAnimation = new ModelAnimation();
		modelAnimation->Read(r,modelAnimation);
		modelAnimationController->animations.push_back(make_pair(name, modelAnimation));

	modelAnimationController->SetCurrentAnimation(modelAnimationController->GetAnimationCount()-1);
	modelAnimationController->Play();

	}
}
