#pragma once

class MoAnimation;
class MoAnimationController
{
public:
	MoAnimationController();
	~MoAnimationController();

	void AddAnimation(MoAnimation* animation);


	UINT GetAnimationCount();
	MoAnimation* GetAnimation(int index);

	int GetNextKeyFrame() { return nextKeyFrame; }
	float GetKeyFrameFactor() { return keyFrameFactor; }


	void UseQuaternionKeyFrames(bool use) { useQuaternionKeyFrames = use; }
	bool UseQuaternionKeyFrames() { return useQuaternionKeyFrames; }
	void GetSpeed(float speed) { this->speed = speed; }
	void Wirte(BinaryWriter * w);
	float GetTime() { return frameTimer; }

	UINT mode = 0;

private:
	MoAnimation* currentAnimation;

	typedef pair<string, MoAnimation *> Pair;
	vector<Pair> animations;

	float frameTimer;
	float speed;
	int currentKeyFrame;
	int nextKeyFrame;
	float keyFrameFactor;
	bool useQuaternionKeyFrames;
};