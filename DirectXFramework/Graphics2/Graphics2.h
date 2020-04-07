#pragma once
#include "DirectXFramework.h"

class Graphics2 : public DirectXFramework
{
public:
	void CreateSceneGraph();
	void UpdateSceneGraph();

private:
	wchar_t* _woodTexture = L"Wood.png";
	wchar_t* _concreteTexture = L"Concrete.png";
	wchar_t* _brickTexture = L"Bricks.png";
	wchar_t* _woodBoxTexture = L"Woodbox.bmp";
	float	 _circleAngle;
	float	 _planeWobble;
};

