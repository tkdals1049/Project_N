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
		if (Mouse::Get()->Press(1))
		{
			D3DXVECTOR3 move = Mouse::Get()->GetMoveValue();

			rotation.x += move.y * Time::Delta() * rotationSpeed;
			rotation.y += move.x * Time::Delta() * rotationSpeed;
		}
	}
	SetRotation(rotation);
}
