#pragma once
#include "DirectXFramework.h"

class Graphics2 : public DirectXFramework
{
public:
	void CreateSceneGraph();
	void UpdateSceneGraph();

private:
	wchar_t* _sampleTexture = L"Wood.png";
};

