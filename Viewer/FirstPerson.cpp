#include "stdafx.h"
#include "FirstPerson.h"

FirstPerson::FirstPerson(float moveSpeed, float rotationSpeed)
	: moveSpeed(moveSpeed), rotationSpeed(rotationSpeed)
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
		if (Keyboard::Get()->Press('W'))
			position += GetForward() * moveSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('S'))
			position += -GetForward() * moveSpeed * Time::Delta();

		if (Keyboard::Get()->Press('A'))
			position += -GetRight() * moveSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('D'))
			position += GetRight() * moveSpeed * Time::Delta();

		if (Keyboard::Get()->Press('E'))
			position += GetUp() * moveSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('Q'))
			position += -GetUp() * moveSpeed * Time::Delta();
	}
	SetPosition(position);

	D3DXVECTOR2 rotation;
	GetRotation(&rotation);
	{
		if (Mouse::Get()->Press(1))
		{
			D3DXVECTOR3 move = Mouse::Get()->GetMoveValue();

			rotation.x += move.y * Time::Delta() * rotationSpeed;
			rotation.y += move.x * Time::Delta() * rotationSpeed;
		}
	}
	SetRotation(rotation);
}
