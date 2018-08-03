#pragma once

class ModelAnimationQuaternionKeyFrame
{
public:
	ModelAnimationQuaternionKeyFrame() {};
	ModelAnimationQuaternionKeyFrame(const D3DXMATRIX& transform)
	{
		D3DXQuaternionRotationMatrix(&q, &transform);
		translation = D3DXVECTOR3(transform._41, transform._42, transform._43);
	}

	const D3DXQUATERNION& GetQuaternion() const { return q; }
	const D3DXVECTOR3& GetTranslation() const { return translation; }

	static void Write(BinaryWriter * w, ModelAnimationQuaternionKeyFrame * quaterkeyframe);
	static void Read(BinaryReader * r, ModelAnimationQuaternionKeyFrame * quaterkeyframe);

private:
	friend class MoAnimationQuaternionKeyFrame;
	D3DXQUATERNION q; /// 회전축(Vector)과 회전각에 대한 정보를 갖는 Quaternion
	D3DXVECTOR3 translation;
	/// 회전축의 위치(Position) 정보
};

class ModelAnimationKeyFrames
{
public:
	ModelAnimationKeyFrames() {};
	ModelAnimationKeyFrames(string animationName);
	ModelAnimationKeyFrames(ModelAnimationKeyFrames& other);
	~ModelAnimationKeyFrames();

	void AddKeyFrame(const D3DXMATRIX& transform);

	string GetAnimationName();
	void SetAnimationName(string animationName);

	const D3DXMATRIX& GetKeyFrameTransform(int keyFrame) const;
	const ModelAnimationQuaternionKeyFrame& GetQuaternionKeyFrame(int keyFrame) const;

	static void Write(BinaryWriter * w, ModelAnimationKeyFrames * keyframe);
	static void Read(BinaryReader * r, ModelAnimationKeyFrames * keyframe);

private:
	friend class MoKeyFrame;
	string animationName;

	typedef pair<D3DXMATRIX, ModelAnimationQuaternionKeyFrame> Pair;
	vector<Pair> keyFrames;
};