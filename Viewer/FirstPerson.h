#pragma once
#include "Camera.h"

class FirstPerson : public Camera
{
public:
	FirstPerson(float moveSpeed = 40, float rotationSpeed = 1.0f);
	~FirstPerson();

	void Update();

private:
	float moveSpeed;
	float rotationSpeed;
};