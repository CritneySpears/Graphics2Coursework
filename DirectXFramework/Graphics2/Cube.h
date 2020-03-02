#pragma once

#include "SceneNode.h"
#include "DirectXFramework.h"

class Cube : public SceneNode
{
public:

	Cube() : SceneNode(L"Cube") {};
	Cube(wstring name, ComPtr<ID3D11ShaderResourceView> texture) : SceneNode(name) {};
	~Cube();

	virtual bool Initialise();
	virtual void Render();

private:

	ComPtr<ID3D11Device>			_device = DirectXFramework::GetDXFramework()->GetDevice();
	ComPtr<ID3D11DeviceContext>		_deviceContext = DirectXFramework::GetDXFramework()->GetDeviceContext();

	ComPtr<ID3D11ShaderResourceView> _texture;;
	wstring _name;
};

