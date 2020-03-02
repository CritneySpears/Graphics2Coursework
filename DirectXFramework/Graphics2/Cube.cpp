#include "Cube.h"

Cube::Cube() : SceneNode(L"Cube")
{

}

Cube::Cube(wstring name, ComPtr<ID3D11ShaderResourceView> texture) : SceneNode(name)
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

	// The call to CoInitializeEx is needed if we are using
	// textures since the WIC library used requires it
	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

	BuildGeometryBuffers();
	BuildShaders();
	BuildVertexLayout();
	BuildConstantBuffer();
	BuildTexture();

	return true;
}

void Cube::Render()
{
}
