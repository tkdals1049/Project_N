#include "../stdafx.h"
#include "Model.h"
#include "BoneBuffer.h"
#include "ModelMesh.h"
#include "ModelMaterial.h"
#include "ModelSkeleton.h"
#include "ModelSkeletonBone.h"
#include "ModelAnimation.h"
#include "ModelAnimationController.h"
#include "ModelAnimationKeyFrames.h"
#include "ModelBoneWeights.h"
#include "../FbxModel/MoModelUtility.h"

Model::Model()
: position(D3DXVECTOR3(0,0,0)), rotate(D3DXVECTOR3(0, 0, 0)), scale(D3DXVECTOR3(1,1,1)),
minp(D3DXVECTOR3(0, 0, 0)), maxp(D3DXVECTOR3(0, 0, 0)), origin(D3DXVECTOR3(0, 0, 0)),skeleton(NULL),animationController(NULL)
{

	D3DXMatrixIdentity(&matGeometricOffset);
	D3DXMatrixIdentity(&world);
	D3DXMatrixIdentity(&weaponWorld);

	manager = FbxManager::Create();
	assert(manager != NULL);

	scene = FbxScene::Create(manager, "");
	assert(scene != NULL);

	boneBuffer = new BoneBuffer();
	animationController = new ModelAnimationController();
}

Model::~Model()
{
	for (ModelMaterial* material : materials)
		SAFE_DELETE(material);

	for (ModelMesh* mesh : meshes)
		SAFE_DELETE(mesh);

	SAFE_DELETE(skeleton);
}

void Model::Update()
{
	AnotherUpdate();
	SetWorld();


	for (ModelMesh* mesh : meshes)
		mesh->Update();
}

void Model::Render()
{
	boneBuffer->SetVSBuffer(2);

	for (ModelMesh* mesh : meshes)
		mesh->Render();
}

void Model::AnotherUpdate()
{
	UINT isAnimated = 0;
	UINT root=0;
	float speed=1.0f;
	for(size_t i=0;i<ani.size();i++)
	{
		if(ani[i].num==animationController->GetAnimationNum())
		{
			root = ani[i].root;
			speed = ani[i].speed;
		}
	}

	if (animationController != NULL)
	{
		isAnimated = animationController->GetAnimationCount() > 1;
		
		if (isAnimated !=0)
		{

			animationController->Update(speed);

			if (skeleton != NULL)
			{
				skeleton->UpdateAnimation(animationController,root);
			}
			UpdateAnimation();
		}
	}

	if (skeleton != NULL)
	{
		D3DXMATRIX* bonesArray = skeleton->GetSkinTransforms();
		if (bonesArray != NULL)
		{
			UINT boneCount = skeleton->GetBoneCount();
			boneCount = boneCount < ModelBoneWeights::MaxBonesPerMax ? boneCount : ModelBoneWeights::MaxBonesPerMax;
			boneBuffer->SetBoneArray(bonesArray, boneCount);
		}
	}
	boneBuffer->SetSkinning(isAnimated==0?false:true);

}

void Model::AddAni(string file,string mode, float speed, int root)
{
	//SDK의 버전을 얻어온다.
	int sdkMajor, sdkMinor, sdkRevision;
	FbxManager::GetFileFormatVersion(sdkMajor, sdkMinor, sdkRevision);

	FbxIOSettings * ios = FbxIOSettings::Create(manager, IOSROOT);
	ios->SetBoolProp(IMP_FBX_TEXTURE, true);
	manager->SetIOSettings(ios);

	//임포터 생성
	importer = FbxImporter::Create(manager, "");
	bool status = importer->Initialize(file.c_str(), -1, ios);
	assert(status == true);

	status = importer->Import(scene);
	assert(status == true);

	ProcessAnimations();

	importer->Destroy();

	Ani ani;
	ani.speed=speed;
	ani.root=root;
	ani.mode=mode;
	ani.num= animationController->GetAnimationCount() - 1;	
	this->ani.push_back(ani);


	animationController->SetCurrentAnimation(animationController->GetAnimationCount() - 1);
	animationController->Play();
}
void Model::AniChage(int num)
{
	if(num>0&&num<= animationController->GetAnimationCount())
	{
		animationController->SetCurrentAnimation(num-1);
		animationController->Play();
	}
}

void Model::UpdateAnimation()
{
	
	D3DXMATRIX matAnimationTransform;
	if (animationController != NULL)
	{
		ModelAnimation* animation = animationController->GetCurrentAnimation();

		if (animation != NULL)
		{
			ModelAnimationKeyFrames* keyFrames = NULL;
			map<string, ModelAnimationKeyFrames*>::iterator iter;
			for (iter = animationKeyFrames.begin(); iter != animationKeyFrames.end(); ++iter)
			{
				if (iter->first == animation->GetName())
				{
					keyFrames = iter->second;

					break;
				}
			}

			if (keyFrames != NULL)
			{
				int keyFrame = animationController->GetCurrentKeyFrame();
				matAnimationTransform = keyFrames->GetKeyFrameTransform(keyFrame);

				return;
			}
		}
	}
	D3DXMatrixIdentity(&matAnimationTransform);
}

void Model::ProcessAnimations()
{
	FbxNode* rootNode = scene->GetRootNode();
	if (rootNode == NULL) return;

	string test = rootNode->GetName();
	float frameRate = (float)FbxTime::GetFrameRate(scene->GetGlobalSettings().GetTimeMode());

	FbxArray<FbxString *> takeArray;
	FbxDocument* document = dynamic_cast<FbxDocument *>(scene);
	if (document != NULL)
		document->FillAnimStackNameArray(takeArray);

	for (int i = 0; i < importer->GetAnimStackCount(); i++)
	{
		FbxTakeInfo* takeInfo = importer->GetTakeInfo(i);
		string takeName = (takeInfo->mName.Buffer());
		takeName += to_string(animationController->GetAnimationCount());

		FbxTime start = FbxTime(FbxLongLong(0x7fffffffffffffff));
		FbxTime stop = FbxTime(FbxLongLong(-0x7fffffffffffffff));

		FbxTimeSpan span = takeInfo->mLocalTimeSpan;
		double tempStart = span.GetStart().GetSecondDouble();
		double tempStop = span.GetStop().GetSecondDouble();

		if (tempStart < tempStop)
		{
			int keyFrames = (int)((tempStop - tempStart) * (double)frameRate);

			ModelAnimation* animation = new ModelAnimation(takeName, keyFrames, frameRate);
			animationController->AddAnimation(animation);

			string test2 = rootNode->GetName();
			ProcessAnimation(rootNode, takeName, frameRate, (float)tempStart, (float)tempStop);
		}
	}
	takeArray.Clear();
	//TODO: defualt 스택 세팅
}

void Model::ProcessAnimation(FbxNode * node, string takeName, float frameRate, float start, float stop)
{
	FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();
	if (nodeAttribute != NULL)
	{
		if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
		{
			if (skeleton != NULL)
			{
				string test = node->GetName();
				ModelSkeletonBone* bone = skeleton->FindBone(node->GetName());

				if (bone != NULL)
				{
					ModelAnimationKeyFrames* animationKeyFrames = new ModelAnimationKeyFrames(takeName);

					double time = 0;
					while (time <= (double)stop)
					{
						FbxTime takeTime;
						takeTime.SetSecondDouble(time);

						D3DXMATRIX matAbsoluteTransform = GetAbsoluteTransformFromCurrentTake(node, takeTime);
						D3DXMATRIX matParentAbsoluteTransform = GetAbsoluteTransformFromCurrentTake(node->GetParent(), takeTime);

						D3DXMATRIX matInvParentAbsoluteTransform;
						D3DXMatrixInverse(&matInvParentAbsoluteTransform, NULL, &matParentAbsoluteTransform);

						D3DXMATRIX matTransform = matAbsoluteTransform * matInvParentAbsoluteTransform;
						animationKeyFrames->AddKeyFrame(matTransform);

						time += 1.0f / frameRate;
					}

					bone->AddAnimationKeyFrames(animationKeyFrames);
				}//if(bone)
			}//if(skeleton)
		}
		else if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
				ModelAnimationKeyFrames* animationKeyFrames = new ModelAnimationKeyFrames(takeName);

				double time = 0;
				while (time <= (double)stop)
				{
					FbxTime takeTime;
					takeTime.SetSecondDouble(time);

					D3DXMATRIX matAbsoluteTransform = GetAbsoluteTransformFromCurrentTake(node, takeTime);
					animationKeyFrames->AddKeyFrame(matAbsoluteTransform);

					time += 1.0f / frameRate;
				}
				AddAnimationKeyFrames(animationKeyFrames);
		}//if(nodeAttribute->GetAttributeType())
	}//if(nodeAttribute)

	for (int i = 0; i < node->GetChildCount(); ++i)
		ProcessAnimation(node->GetChild(i), takeName, frameRate, start, stop);
}


/////////////////////////////////////////////////////////////////////////

bool Model::Check(D3DXVECTOR3 origin, D3DXVECTOR3 direction)
{
	bool check=false;
	for (ModelMesh* mesh : meshes)
	{
		if(mesh->ClickCheck(matGeometricOffset,origin,direction))
		return true;
	}
	return false;
}

void Model::Reset()
{
	D3DXVec3TransformCoord(&origin,&origin,&matGeometricOffset);
	for (ModelMesh* mesh : meshes)
	{
		mesh->Reset();
	}
	for (size_t i = 0; i <meshes.size(); i++)
	{
		if (i==0) meshes[i]->GetMinMax(minp, maxp);
		else
		{
			D3DXVECTOR3 minb, maxb;
			meshes[i]->GetMinMax(minb, maxb);
			if (minp>minb) minp = minb;
			if (maxp<maxb) maxp = maxb;
		}
	}
}

void Model::SetWorld()
{
		D3DXVECTOR3 size = maxp - minp;
		D3DXVECTOR3 center = minp + size / 2;
		D3DXMATRIX matS, matISC, matSC;
		D3DXMatrixTranslation(&matSC, center.x, center.y, center.z);
		D3DXMatrixInverse(&matISC, NULL, &matSC);
		D3DXMatrixScaling(&matS, scale.x, scale.y, scale.z);
		matS = matISC * matS*matSC;

		D3DXMATRIX matR, matIRC, matRC, matX, matY, matZ;
		D3DXMatrixTranslation(&matRC, center.x, center.y, center.z);
		D3DXMatrixInverse(&matIRC, NULL, &matRC);
		D3DXMatrixRotationX(&matX, rotate.x);
		D3DXMatrixRotationY(&matY, rotate.y);
		D3DXMatrixRotationZ(&matZ, rotate.z);
		matR = matIRC * matX*matY*matZ*matRC;

		D3DXMATRIX matT, matRT;
		D3DXMatrixTranslation(&matRT, center.x, 0, center.z);
		D3DXMatrixInverse(&matRT, NULL, &matRT);
		D3DXMatrixTranslation(&matT, position.x, position.y, position.z);
		matT = matRT * matT;
		matGeometricOffset = matS*matR*matT;
}

ModelMaterial * Model::GetMatchMaterial(UINT number)
{
	for (ModelMaterial* material : materials)
	{
		if (material->GetNumber() == number)
			return material;
	}

	return NULL;
}

void Model::MakeMaterial(int num, string diffusePath)
{
	materials[num]->SetDiffuseTexture(diffusePath);
}

void Model::MeshMaterial()
{
	for (ModelMesh* mesh : meshes)
	{
		mesh->material = GetMatchMaterial(mesh->material->GetNumber());
	}
}

void Model::SetMaterial(ModelMaterial * material)
{
	materials.push_back(material);
}

void Model::ClearMaterial()
{
	materials.clear();
}

D3DXMATRIX Model::GetWeaponWorld(string weapon)
{
	return matGeometricOffset * skeleton->GetWeapon(weapon);
}

void Model::SetWorld(D3DXMATRIX& world)
{
	this->world=world;
}

void Model::AddAnimationKeyFrames(ModelAnimationKeyFrames * animationKeyFrames)
{
	this->animationKeyFrames.insert(make_pair(animationKeyFrames->GetAnimationName(), animationKeyFrames));
}
void Model::GetFileName(string file)
{
	 file=this->file;
}

void Model::SetPosition(D3DXVECTOR3 & position)
{
	this->position = position;
}

D3DXVECTOR3 Model::GetPosition()
{
	return position;
}

void Model::SetRotate(D3DXVECTOR3 & rotate)
{
	this->rotate = rotate;
}

D3DXVECTOR3 Model::GetRotate()
{
	return rotate;
}

void Model::SetScale(D3DXVECTOR3 & scale)
{
	this->scale = scale;
}

D3DXVECTOR3 Model::GetScale()
{
	return scale;
}

D3DXMATRIX Model::GetAbsoluteTransformFromCurrentTake(FbxNode * node, FbxTime time)
{
	if (node == NULL)
	{
		D3DXMATRIX mat;
		D3DXMatrixIdentity(&mat);

		return mat;
	}

	/*FbxAnimEvaluator* e = node->GetAnimationEvaluator();
	FbxAMatrix matrix = e->GetNodeGlobalTransform(node, time, FbxNode::eSourcePivot, false);*/
	FbxAMatrix matrix = node->EvaluateGlobalTransform(time);

	return MoModelUtility::ToMatrix(matrix);
}
