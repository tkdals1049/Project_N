#include "stdafx.h"
#include "Test.h"

Test * Test::instance = NULL;

Test * Test::Get()
{
	if (instance == NULL)
		instance = new Test();

	return instance;
}

void Test::Delete()
{
	SAFE_DELETE(instance);
}


Test::Test()
{}

Test::~Test()
{
}
