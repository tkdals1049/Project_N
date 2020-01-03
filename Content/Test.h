#pragma once

class Test
{
	public:
	static Test* Get();
	static void Delete();

	private:
		Test();
		~Test();
	static Test* instance;
};