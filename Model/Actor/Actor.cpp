#include "../../stdafx.h"
#include "Actor.h"

#include "../BinModel/Model.h"
#include "../BinModel/ModelSkeleton.h"
#include "../BinModel/ModelMaterial.h"
#include "../BinModel/ModelSkeletonBone.h"
#include "../BinModel/ModelAnimationController.h"

#include "../Etc/Blood.h"

#include "../../Content/BinModel.h"
#include "../../Content/FbxModel.h"
#include "../../Content/Texture.h"



Actor::Actor() 
:model(NULL), mode("idle"), Premode(""), different(false), dot(D3DXVECTOR2(0, 0)), a(0.0f), 
attackRange(NULL), isControl(true), isEquip(false), isHeight(false), isAttack(false)
{
	SaveFile = "";
	blood = new Blood();
}

Actor::~Actor()
{
		SAFE_DELETE(model);
}

void Actor::PreUpdate(D3DXVECTOR3 origin, D3DXVECTOR3 direction)
{

}
void Actor::Update()
{
	if (model != NULL)
	{
		model->Update();
		blood->Update();
	}
	
	D3DXVECTOR3 position=model->GetPosition();
	position.y=Plane::Get()->GetHeight(position.x, position.z);
	model->SetPosition(position);
}
void Actor::PostRender(bool& isUse)
{
}
void Actor::Render()
{
	if(model!=NULL)
	{
		model->Render();
		blood->Render();
	}
}

//조작할 모델을 설정하는 함수
void Actor::SetModel(string file) 
{
	if (model != NULL)SAFE_DELETE(model);
	MoLoader::LoadBinary(file, &model);
	model->Reset();
}
void Actor::SetModel(Model* another)
{
	if(another==NULL){ model=NULL; return;}
	if(model!=NULL)SAFE_DELETE(model);
	model = new Model();
	model=another;
}


void Actor::Check(D3DXVECTOR3 origin, D3DXVECTOR3 direction)
{
		if (model->Check(origin, direction))
		{
				if (model->GetSkeleton() != NULL)
				{
					int num = model->GetSkeleton()->GetBoneCount();
					skeletonList = new const char*[num];
					for (size_t i = 0; (int)i < num; i++)
					{
						ModelSkeletonBone* bone = model->GetSkeleton()->GetSkeletonBone(i);
						skeletonList[i] = new char[bone->GetName().size() + 1];
						strcpy_s((char*)skeletonList[i], bone->GetName().size() + 1, bone->GetName().c_str());
					}
				}
				different = true;
 		}
}
//모델의 자세 조정값을 입력한다
void Actor::SetAdjust(D3DXVECTOR3 adjust)
{
	model->SetAdjust(adjust);
}
void Actor::SetAniPlay(D3DXVECTOR3 ani)
{
	model->SetAniPlay((int)ani.x,(int)ani.y,ani.z);
}

//입력한 OBB에 모델의 스켈레톤을 연결한 OBB가 충돌하는지 계산한다
bool Actor::CalSkeleton(ST_OBB* Attack_box)
{
	if(Attack_box==NULL) return false;

	typedef pair<string, ModelSkeletonBone *> Pair;
	vector<Pair> skeletonBones = model->GetSkeleton()->GetSkeletonBones();

	if (skeletonBones.size()<1)return false;
	for (size_t i = 0; i<skeletonBones.size(); i++)
	{
		if (skeletonBones[i].second->GetParentBoneIndex()<0) { continue; }

		D3DXMATRIX child = model->GetSkeleton()->GetBoneAnimationTransform(i);
		D3DXMATRIX parent = model->GetSkeleton()->GetBoneAnimationTransform(skeletonBones[i].second->GetParentBoneIndex());

		D3DXMATRIX scale, rotate1, rotate2, rotate3, position;
		D3DXVECTOR3 Dot1(child._41, child._42, child._43), Dot2(parent._41, parent._42, parent._43);
		D3DXVECTOR3 Center = (Dot1 + Dot2) / 2, Vec = Dot1 - Dot2;
		D3DXVECTOR2 Vec2[3]{ D3DXVECTOR2(Vec.z,Vec.x),D3DXVECTOR2(Vec.z,Vec.y),D3DXVECTOR2(Vec.x,Vec.y) };

		for (int i = 0; i<3;i++)
			D3DXVec2Normalize(&Vec2[i], &Vec2[i]);

		float size = D3DXVec3Length(&(Dot1 - Dot2));
		//현재 OBB의 크기와 각도와 위치를 조정한다.
		D3DXMatrixScaling(&scale, 1, size, 1);
		D3DXMatrixRotationX(&rotate1, D3DXVec2Dot(&Vec2[1], &D3DXVECTOR2(1, 0)));
		D3DXMatrixRotationY(&rotate2, D3DXVec2Dot(&Vec2[0], &D3DXVECTOR2(1, 0)));
		D3DXMatrixRotationZ(&rotate3, D3DXVec2Dot(&Vec2[2], &D3DXVECTOR2(1, 0)));
		D3DXMatrixTranslation(&position, Center.x, Center.y, Center.z);
		D3DXMatrixInverse(&rotate1, NULL, &rotate1);
		D3DXMatrixInverse(&rotate2, NULL, &rotate2);

		//모델의 위치와 제어값을 OBB제어값에 곱해 최종값을 구한다.
		D3DXMATRIX mat=(scale*rotate1*rotate2*rotate3*position*model->GetGeometricOffset()*model->GetWorld());

		//그 최종값에서 3축의 방향과 중심점을 구한다
		D3DXVECTOR3 root[3], one(0, 0, 0);
		D3DXVec3TransformCoord(&one, &one, &mat);
		D3DXVec3TransformCoord(&root[0], &D3DXVECTOR3(1, 0, 0), &mat);
		D3DXVec3TransformCoord(&root[1], &D3DXVECTOR3(0, 1, 0), &mat);
		D3DXVec3TransformCoord(&root[2], &D3DXVECTOR3(0, 0, 1), &mat);
		root[0] -= one;
		root[1] -= one;
		root[2] -= one;
		
		D3DXVECTOR3 bulk=model->GetScale()/2;
		float length[3]{bulk.x,bulk.y*size,bulk.z};

		ST_OBB* Damage_Box=new ST_OBB(one,root,length);

		if(CheckOBBCollision(Attack_box,Damage_Box))
		{
			blood->SetPosition(one);
			model->isDamage();
			return true;
		}
	}
	return false;
}
//모델의 OBB를 계산해서 출력함
void Actor::CalMatrix(ST_OBB* box, D3DXVECTOR3 max, D3DXVECTOR3 min, D3DXVECTOR3 size,D3DXMATRIX mat)
{
	D3DXVec3TransformCoord(&box->vCenterPos, &D3DXVECTOR3((max+min)/2), &mat);

	D3DXVECTOR3 root[3],one(0,0,0);

	D3DXVec3TransformCoord(&one, &one, &mat);
	
	D3DXVec3TransformCoord(&root[0], &D3DXVECTOR3(1, 0, 0), &mat);
	D3DXVec3TransformCoord(&root[1], &D3DXVECTOR3(0, 1, 0), &mat);
	D3DXVec3TransformCoord(&root[2], &D3DXVECTOR3(0, 0, 1), &mat);

	root[0] -= one;
	root[1] -= one;
	root[2] -= one;

	D3DXVec3Normalize(&box->vAxisDir[0], &root[0]);
	D3DXVec3Normalize(&box->vAxisDir[1], &root[1]);
	D3DXVec3Normalize(&box->vAxisDir[2], &root[2]);

	box->fAxisLen[0] = (max.x-  min.x)/2*size.x;
	box->fAxisLen[1] = (max.y - min.y)/2*size.y;
	box->fAxisLen[2] = (max.z - min.z)/2*size.z;
}

//OBB 충돌체크 함수
BOOL Actor::CheckOBBCollision(ST_OBB* Box1, ST_OBB* Box2)
{
	double c[3][3];
	double absC[3][3];
	double d[3];

	double r0, r1, r;
	int i;

	const double cutoff = 0.999999;
	bool existsParallelPair = false;

	D3DXVECTOR3 diff = Box1->vCenterPos - Box2->vCenterPos;

	for (i = 0; i < 3; ++i)
	{
		c[0][i] = D3DXVec3Dot(&Box1->vAxisDir[0], &Box2->vAxisDir[i]);
		absC[0][i] = abs(c[0][i]);
		if (absC[0][i] > cutoff)
			existsParallelPair = true;
	}
	d[0] = D3DXVec3Dot(&diff, &Box1->vAxisDir[0]);
	r = abs(d[0]);
	r0 = Box1->fAxisLen[0];
	r1 = Box2->fAxisLen[0] * absC[0][0] + Box2->fAxisLen[1] * absC[0][1] + Box2->fAxisLen[2] * absC[0][2];

	if (r > r0 + r1)return FALSE;

	for (i = 0; i < 3; ++i)
	{
		c[1][i] = D3DXVec3Dot(&Box1->vAxisDir[1], &Box2->vAxisDir[i]);
		absC[1][i] = abs(c[1][i]);
		if (absC[1][i] > cutoff)
			existsParallelPair = true;
	}

	d[1] = D3DXVec3Dot(&diff, &Box1->vAxisDir[1]);
	r = abs(d[1]);
	r0 = Box1->fAxisLen[1];
	r1 = Box2->fAxisLen[0] * absC[1][0] + Box2->fAxisLen[1] * absC[1][1] + Box2->fAxisLen[2] * absC[1][2];
	if (r > r0 + r1)return FALSE;

	for (i = 0; i < 3; ++i)
	{
		c[2][i] = D3DXVec3Dot(&Box1->vAxisDir[2], &Box2->vAxisDir[i]);
		absC[2][i] = abs(c[2][i]);
		if (absC[2][i] > cutoff)
			existsParallelPair = true;
	}

	d[2] = D3DXVec3Dot(&diff, &Box1->vAxisDir[2]);
	r = abs(d[2]);
	r0 = Box1->fAxisLen[2];
	r1 = Box2->fAxisLen[0] * absC[2][0] + Box2->fAxisLen[1] * absC[2][1] + Box2->fAxisLen[2] * absC[2][2];
	if (r > r0 + r1)return FALSE;

	r = abs(D3DXVec3Dot(&diff, &Box2->vAxisDir[0]));
	r0 = Box1->fAxisLen[0] * absC[0][0] + Box1->fAxisLen[1] * absC[1][0] + Box1->fAxisLen[2] * absC[2][0];
	r1 = Box2->fAxisLen[0];
	if (r > r0 + r1)return FALSE;

	r = abs(D3DXVec3Dot(&diff, &Box2->vAxisDir[1]));
	r0 = Box1->fAxisLen[0] * absC[0][1] + Box1->fAxisLen[1] * absC[1][1] + Box1->fAxisLen[2] * absC[2][1];
	r1 = Box2->fAxisLen[1];
	if (r > r0 + r1)return FALSE;

	r = abs(D3DXVec3Dot(&diff, &Box2->vAxisDir[2]));
	r0 = Box1->fAxisLen[0] * absC[0][2] + Box1->fAxisLen[1] * absC[1][2] + Box1->fAxisLen[2] * absC[2][2];
	r1 = Box2->fAxisLen[2];
	if (r > r0 + r1)return FALSE;

	if (existsParallelPair == true)	return TRUE;

	r = abs(d[2] * c[1][0] - d[1] * c[2][0]);
	r0 = Box1->fAxisLen[1] * absC[2][0] + Box1->fAxisLen[2] * absC[1][0];
	r1 = Box2->fAxisLen[1] * absC[0][2] + Box2->fAxisLen[2] * absC[0][1];
	if (r > r0 + r1)return FALSE;

	r = abs(d[2] * c[1][1] - d[1] * c[2][1]);
	r0 = Box1->fAxisLen[1] * absC[2][1] + Box1->fAxisLen[2] * absC[1][1];
	r1 = Box2->fAxisLen[0] * absC[0][2] + Box2->fAxisLen[2] * absC[0][0];
	if (r > r0 + r1)return FALSE;

	r = abs(d[2] * c[1][2] - d[1] * c[2][2]);
	r0 = Box1->fAxisLen[1] * absC[2][2] + Box1->fAxisLen[2] * absC[1][2];
	r1 = Box2->fAxisLen[0] * absC[0][1] + Box2->fAxisLen[1] * absC[0][0];
	if (r > r0 + r1)return FALSE;

	r = abs(d[0] * c[2][0] - d[2] * c[0][0]);
	r0 = Box1->fAxisLen[0] * absC[2][0] + Box1->fAxisLen[2] * absC[0][0];
	r1 = Box2->fAxisLen[1] * absC[1][2] + Box2->fAxisLen[2] * absC[1][1];
	if (r > r0 + r1)return FALSE;

	r = abs(d[0] * c[2][1] - d[2] * c[0][1]);
	r0 = Box1->fAxisLen[0] * absC[2][1] + Box1->fAxisLen[2] * absC[0][1];
	r1 = Box2->fAxisLen[0] * absC[1][2] + Box2->fAxisLen[2] * absC[1][0];
	if (r > r0 + r1)return FALSE;

	r = abs(d[0] * c[2][2] - d[2] * c[0][2]);
	r0 = Box1->fAxisLen[0] * absC[2][2] + Box1->fAxisLen[2] * absC[0][2];
	r1 = Box2->fAxisLen[0] * absC[1][1] + Box2->fAxisLen[1] * absC[1][0];
	if (r > r0 + r1)return FALSE;

	r = abs(d[1] * c[0][0] - d[0] * c[1][0]);
	r0 = Box1->fAxisLen[0] * absC[1][0] + Box1->fAxisLen[1] * absC[0][0];
	r1 = Box2->fAxisLen[1] * absC[2][2] + Box2->fAxisLen[2] * absC[2][1];
	if (r > r0 + r1)return FALSE;

	r = abs(d[1] * c[0][1] - d[0] * c[1][1]);
	r0 = Box1->fAxisLen[0] * absC[1][1] + Box1->fAxisLen[1] * absC[0][1];
	r1 = Box2->fAxisLen[0] * absC[2][2] + Box2->fAxisLen[2] * absC[2][0];
	if (r > r0 + r1)return FALSE;
	
	r = abs(d[1] * c[0][2] - d[0] * c[1][2]);
	r0 = Box1->fAxisLen[0] * absC[1][2] + Box1->fAxisLen[1] * absC[0][2];
	r1 = Box2->fAxisLen[0] * absC[2][1] + Box2->fAxisLen[1] * absC[2][0];
	if (r > r0 + r1)return FALSE;

	return TRUE;
}