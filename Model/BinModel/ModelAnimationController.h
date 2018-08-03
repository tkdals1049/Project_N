#pragma once

enum class AnimationMode
{
	Play = 0, Pause, Stop,
};

class ModelAnimation;
class ModelAnimationController
{
public:
	ModelAnimationController();
	~ModelAnimationController();

	void AddAnimation(ModelAnimation* animation);

	ModelAnimation* GetCurrentAnimation();
	void SetCurrentAnimation(string animationName);
	void SetCurrentAnimation(int index);

	UINT GetAnimationCount();
	ModelAnimation* GetAnimation(int index);
	int GetAnimationNum();
	ModelAnimation* GetAnimation(string animName);

	int GetCurrentAnimationCount();
	int GetCurrentKeyFrame() { return currentKeyFrame; }
	void SetCurrentKeyFrame(int keyFrame);
	
	int GetNextKeyFrame() { return nextKeyFrame; }
	float GetKeyFrameFactor() { return keyFrameFactor; }

	AnimationMode GetAnimationMode() { return animationMode; }

	void UseQuaternionKeyFrames(bool use) { useQuaternionKeyFrames = use; }
	bool UseQuaternionKeyFrames() { return useQuaternionKeyFrames; }

	void Play();
	void Pause();
	void Stop();

	void Update(float speed=1.0f);
	static void Write(BinaryWriter * w, ModelAnimationController * modelAnimationController);
	static void Read(BinaryReader * r, ModelAnimationController * modelAnimationController);
	static void Add(BinaryReader * r, ModelAnimationController * modelAnimationController);
private:
	AnimationMode animationMode; /// ��� ���
	ModelAnimation* currentAnimation; /// ���� Animation
	ModelAnimation* prevAnimation; /// �� Animation

	typedef pair<string, ModelAnimation *> Pair;
	vector<Pair> animations; /// Animation ����
	
	float frameTimer; /// ���� KeyFrame���� ���� �� ��� �ð�

	int currentKeyFrame; /// ���� KeyFrame
	int nextKeyFrame; /// ���� KeyFrame
	float keyFrameFactor; /// ���� KeyFrame�� ���� KeyFrame���̿����� ���� ��
	bool useQuaternionKeyFrames; /// ���� ��� ���� flag
};