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
	AnimationMode animationMode; /// 재생 모드
	ModelAnimation* currentAnimation; /// 현재 Animation
	ModelAnimation* prevAnimation; /// 전 Animation

	typedef pair<string, ModelAnimation *> Pair;
	vector<Pair> animations; /// Animation 벡터
	
	float frameTimer; /// 현재 KeyFrame으로 변경 후 경과 시간

	int currentKeyFrame; /// 현재 KeyFrame
	int nextKeyFrame; /// 다음 KeyFrame
	float keyFrameFactor; /// 현재 KeyFrame과 다음 KeyFrame사이에서의 보간 값
	bool useQuaternionKeyFrames; /// 보간 사용 유무 flag
};