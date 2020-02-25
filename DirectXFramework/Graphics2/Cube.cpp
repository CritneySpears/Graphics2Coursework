#include "Cube.h"

Cube::Cube(wstring name, ComPtr<ID3D11ShaderResourceView> texture)
{
	_name = name;
	_texture = texture;
	XMStoreFloat4x4(&_worldTransformation, XMMatrixIdentity());
}

Cube::~Cube()
{
}

bool Cube::Initialise()
{
	// Access current instance of DirectFramework.
	DirectXFramework::GetDXFramework();
	//Access device and device context information.
	ComPtr<ID3D11Device> device = 
		DirectXFramework::GetDXFramework()->GetDevice(); //returns ComPtrs to appropriate interface.
	ComPtr<ID3D11DeviceContext> deviceContext = 
		DirectXFramework::GetDXFramework()->GetDeviceContext();

	BuildGeometryBuffers();
	BuildShaders();
	BuildVertexLayout();
	BuildConstantBuffer();
	BuildTexture();
}
