#pragma once
#include "Camera.h"

class FirstPerson : public Camera
{
public:
	FirstPerson(float moveSpeed = 60, float rotationSpeed = 2.5f);
	~FirstPerson();

	void Update();

private:
	float moveSpeed;
	float rotationSpeed;
};