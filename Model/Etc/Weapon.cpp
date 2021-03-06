#include "stdafx.h"
#include "Weapon.h"
#include "../Actor/Actor.h"
#include "../BinModel/Model.h"

Weapon::Weapon(Actor* actor, string equipName, string equipName2, string unequipName)
:actor(actor),weaponNum(-1), count(20),effectCount(0),effectTime(0),
equipName(equipName), equipName2(equipName2), unequipName(unequipName)
{
	shader = new Shader(Shaders + L"WeaponEffect.hlsl");
	worldBuffer = new WorldBuffer();
	D3DXMATRIX world;
	D3DXMatrixIdentity(&world);
	worldBuffer->SetMatrix(world);
	CreateBuffer();
}

Weapon::~Weapon()
{
	for (size_t i = 0; i <weapons.size(); i++)
		SAFE_DELETE(weapons[i].second);
	SAFE_DELETE(shader);
	SAFE_DELETE(worldBuffer);
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);
	SAFE_DELETE_ARRAY(index);
	SAFE_DELETE_ARRAY(vertex);
}

void Weapon::PreUpdate()
{
	if (Keyboard::Get()->Down('F') && actor->GetMode() == "idle")
	{
		weaponNum++;
		if ((unsigned int)weaponNum >= weapons.size()) weaponNum = 0;

		weapons[weaponNum].second->SetWorld(actor->GetModel()->GetWeaponWorld(unequipName));
	}
}

void Weapon::Update()
{
	if (weaponNum >= 0 && (unsigned int)weaponNum<weapons.size())
	{
		D3DXMATRIX world, scale, rotate;
		string worldName;
		weapons[weaponNum].second->Update();

		//무기 장착시와 아닐시 고정 위치 변동
		if (actor->GetisEquip())
		{
			if (actor->GetMode().find("equip") != -1)
				worldName = WeaponSetting(D3DXVECTOR3(90, 0, 0), D3DXVECTOR3(0, -0.5f, 0), equipName2);
			else
				worldName = WeaponSetting(D3DXVECTOR3(-90, 0, 0), D3DXVECTOR3(0.3f, 1.0f, 0), equipName);
		}
		else
		{
			worldName = WeaponSetting(D3DXVECTOR3(0, 60, 90), D3DXVECTOR3(-0.07f, -2.0f, 0.4f), unequipName, actor->GetMode() == "idle");
		}

		//무기 영역 및 월드 계산 
		weapons[weaponNum].second->SetWorld(actor->GetModel()->GetWeaponWorld(worldName));
		weaponWorld=weaponsInfo[weaponNum]->lotate* weapons[weaponNum].second->GetGeometricOffset()*actor->GetModel()->GetWeaponWorld(worldName);
				
		//무기 장비시의 월드 및 OBB 계산
		if (actor->GetisAttack()&& actor->GetAttackRange() != NULL)
		{
			actor->CalMatrix(actor->GetAttackRange(), weaponsInfo[weaponNum]->maxp, weaponsInfo[weaponNum]->minp, weaponsInfo[weaponNum]->size,	weaponWorld);
		}

		EffectUpdate();
	}
}

void Weapon::Render()
{
	States::SetRasterizerNone();

	if (weaponNum >= 0 && (unsigned int)weaponNum<weapons.size())
	{
		weapons[weaponNum].second->Render();
		EffectRender();
	}

	States::SetRasterizerDefault();
}

void Weapon::ClearWeapon()
{
	for each(WeaponVec temp in weapons)
	{
		SAFE_DELETE(temp.second);
	}
	weapons.clear();
}

//무기를 추가함
void Weapon::AddWeaponVector(Model * weaponFile, D3DXVECTOR3 scale)
{
	weaponFile->Reset();
	weapons.push_back(WeaponVec(equipName, weaponFile));
	WeaponInfo* info = new WeaponInfo(weaponFile, scale);
	weaponsInfo.push_back(info); 
}

//무기의 각도와 오차를 조정하고 모델의 장착부위를 출력함
string Weapon::WeaponSetting(D3DXVECTOR3 rotate, D3DXVECTOR3 adjust, string equipName, bool idleType)
{
	if (actor->GetMode() != actor->GetPremode() || idleType)
	{
		weapons[weaponNum].second->SetRotate(rotate);
		weapons[weaponNum].second->SetAdjust(adjust);
	}
	return equipName;
}

//일정시간이 지날 때마다 정점을 저장하교 궤적 이펙트를 남긴다
void Weapon::EffectUpdate()
{
	static bool effectReset = false;
	static int num[6]{ 0,2,1,1,2,3 };
	static int num2[6]{ (int)count * 2 - 2,0,(int)count * 2 - 1,(int)count * 2 - 1,0,1 };

	effectTime += Time::Get()->Delta();
	if (effectTime > 0.05f)
	{
		if (actor->GetisAttack())
		{
			D3DXVECTOR3 up, down, max = weaponsInfo[weaponNum]->maxp, min = weaponsInfo[weaponNum]->minp;

			D3DXVec3TransformCoord(&down, &D3DXVECTOR3(2 * min.x, 0, 0), &weaponWorld);
			D3DXVec3TransformCoord(&up, &D3DXVECTOR3(-min.x, 0, 0), &weaponWorld);

			vertex[2 * effectCount].position = down;vertex[2 * effectCount + 1].position = up;
			vertex[2 * effectCount].color = vertex[2 * effectCount + 1].color =D3DXCOLOR(1, 1, 1, 1);
			
			//마지막 인덱스 시에는 숫자가 초기화되기에 초기화 된 값에 맞추기 위한 반복문
			for (int i = 0; i < 6; i++)
			{
				if (effectCount != 0) index[(effectCount - 1) * 6 + i] = (effectCount - 1) * 2 + num[i];
				else index[(count - 1) * 6 + i] = num2[i];

				//마지막 이펙트와 처음 이펙트가 이어지지 않도록 공백을 만든다
				index[(effectCount) * 6 + i] = (effectCount) * 6;
			}

			D3D::GetDC()->UpdateSubresource
			(vertexBuffer, 0, NULL, vertex, sizeof(VertexType)*count * 2, 0);
			D3D::GetDC()->UpdateSubresource
			(indexBuffer, 0, NULL, index, sizeof(UINT)*count * 6, 0);
		}
		else
		{
			//계속 업데이트 하면 부담되니 색이 있을 경우에만 업데이트하도록 함
			if (effectReset)
			{
				effectReset = false;

				D3D::GetDC()->UpdateSubresource
				(vertexBuffer, 0, NULL, vertex, sizeof(VertexType)*count * 2, 0);
				D3D::GetDC()->UpdateSubresource
				(indexBuffer, 0, NULL, index, sizeof(UINT)*count * 6, 0);
			}
		}

		for (int i = 0; i < (int)count * 2 + 1; i++)
		{
			if (vertex[i].color.a > 0)
			{
				effectReset = true; 
				vertex[i].color.a -= 0.05f;
			}
		}

		effectTime = 0;
		if (++effectCount >= count) effectCount = 0;

	}
}

void Weapon::EffectRender()
{
	ID3D11DeviceContext* dc = D3D::GetDC();

	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	dc->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	dc->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	worldBuffer->SetVSBuffer(1);

	shader->Render();

	States::SetDepthStencilMaskZero();
	States::SetBlendOn();
	{
		dc->DrawIndexed(count*6, 0, 0);
	}
	States::SetBlendOff();
	States::SetDepthStencilDefault();
}

void Weapon::CreateBuffer()
{
	HRESULT hr;

	vertexCount = count*2;
	vertex = NULL;
	vertex = new VertexType[vertexCount];
	
	index = NULL;
	indexCount = (count)*6;
	index = new UINT[indexCount];
	
	for (int i = 0;i < (int)count;i++)
	{
		vertex[2 * effectCount].position = vertex[2 * effectCount + 1].position = D3DXVECTOR3(0, 0, 0);
		vertex[2 * effectCount].color = vertex[2 * effectCount + 1].color = D3DXCOLOR(0, 0, 0, 0);

		for (int j = 0;j<6;j++) index[i * 6 + j] = 0;
	}

	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;

	//1. Vertex Buffer
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(VertexType) * vertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = vertex;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
	assert(SUCCEEDED(hr));

	//2. Index Buffer
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(UINT) * indexCount;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = index;

	hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
	assert(SUCCEEDED(hr));
}


void Weapon::EffecetReset()
{

	for (int i = 0; i < (int)count; i++)
	{
		vertex[2 * effectCount].position = vertex[2 * effectCount + 1].position =D3DXVECTOR3(0, 0, 0);
		vertex[2 * effectCount].color = vertex[2 * effectCount + 1].color = D3DXCOLOR(0, 0, 0, 0);

		for (int j = 0; j < 6; j++) index[i * 6 + j] = 0;
	}

	D3D::GetDC()->UpdateSubresource
	(vertexBuffer, 0, NULL, vertex, sizeof(VertexType)*count * 2, 0);
	D3D::GetDC()->UpdateSubresource
	(indexBuffer, 0, NULL, index, sizeof(UINT)*count * 6, 0);
}
