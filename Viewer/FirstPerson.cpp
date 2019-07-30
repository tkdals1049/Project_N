#include "stdafx.h"
#include "FirstPerson.h"

FirstPerson::FirstPerson(float moveSpeed, float rotationSpeed)
	: moveSpeed(moveSpeed), rotationSpeed(rotationSpeed),quike(D3DXVECTOR2(0,0))
{

}

FirstPerson::~FirstPerson()
{

}

void FirstPerson::Update()
{
	D3DXVECTOR3 position;
	GetPosition(&position);
	{
		if (Keyboard::Get()->Press('I'))
			position += GetForward() * moveSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('K'))
			position += -GetForward() * moveSpeed * Time::Delta();

		if (Keyboard::Get()->Press('J'))
			position += -GetRight() * moveSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('L'))
			position += GetRight() * moveSpeed * Time::Delta();

		if (Keyboard::Get()->Press('U'))
			position += GetUp() * moveSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('O'))
			position += -GetUp() * moveSpeed * Time::Delta();
	}
	SetPosition(position);

	D3DXVECTOR2 rotation;
	GetRotation(&rotation);
	{
		static int value = 0;
		static float time = 0;
		if (Keyboard::Get()->Down('G'))  value = 10;

		time += Time::Get()->Delta();
		if (time > 0.03f)
		{
			if (value > 0) quike.x = (float)((value == 10 || value == 1 ? value : 2 * value - 1)*(value-- % 2 == 0 ? 1 : -1)*(float)D3DX_PI / 180.0/16);
			time = 0;
		}
		//일정 시간마다 카메라를 흔들어 지진이 일어난 듯한 효과를 준다
		
		if (Mouse::Get()->Press(1))
		{
			D3DXVECTOR3 move = Mouse::Get()->GetMoveValue();

			rotation.x += move.y * Time::Delta() * rotationSpeed;
			rotation.y += move.x * Time::Delta() * rotationSpeed;
		}
	}
	SetRotation(rotation);
	UpdateRotation(quike);
	UpdateView();
	
}
