#pragma once

#include "SceneNode.h"
#include "DirectXFramework.h"

class Cube : public SceneNode
{
public:

	Cube(wstring name, ComPtr<ID3D11ShaderResourceView> texture);
	~Cube();

	virtual bool Initialise();
	virtual void Render();
	virtual void Shutdown();

private:
	ComPtr<ID3D11ShaderResourceView> _texture;;
	wstring _name;
	COLORREF _colour;
};

