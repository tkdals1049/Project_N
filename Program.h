#pragma once

class Program
{
public:
	Program();
	~Program();

	void Update();

	void PreRender();
	void Render();
	void PostRender();

	void ResizeScreen();

private:
	struct ExecuteValues* values;
	vector<class Execute *> executes;//위에 해더를 쓸 필요없이 전방선언의 효과를 한다.
};