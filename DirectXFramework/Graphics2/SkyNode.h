#pragma once
#include "SceneNode.h"
#include "DirectXFramework.h"
#include <vector>

class SkyNode : public SceneNode
{
public:

	struct Vertex
	{
		XMFLOAT3 Position;
	};

	struct CBUFFER
	{
		XMMATRIX CompleteTransformation;
	};

	SkyNode(wstring name, wchar_t* skyNodeTexturePath, float radius) : SceneNode(name) { _skyNodeTexturePath = skyNodeTexturePath; _radius = radius; }

	bool Initialise();
	void Render();
	void Shutdown();

private:

	ComPtr<ID3D11Device>				_device = DirectXFramework::GetDXFramework()->GetDevice();
	ComPtr<ID3D11DeviceContext>			_deviceContext = DirectXFramework::GetDXFramework()->GetDeviceContext();

	ComPtr<ID3D11Buffer>				_vertexBuffer;
	ComPtr<ID3D11Buffer>				_indexBuffer;

	ComPtr<ID3DBlob>					_vertexShaderByteCode = nullptr;
	ComPtr<ID3DBlob>					_pixelShaderByteCode = nullptr;
	ComPtr<ID3D11VertexShader>			_vertexShader;
	ComPtr<ID3D11PixelShader>			_pixelShader;
	ComPtr<ID3D11InputLayout>			_layout;
	ComPtr<ID3D11Buffer>				_constantBuffer;

	ComPtr<ID3D11RasterizerState>		_defaultRasteriserState;
	ComPtr<ID3D11RasterizerState>		_noCullRasteriserState;

	ComPtr<ID3D11DepthStencilState>		_stencilState;

	ComPtr<ID3D11ShaderResourceView>	_texturesResourceView;
	ComPtr<ID3D11ShaderResourceView>	_blendMapResourceView;
	ComPtr<ID3D11Resource>				_texture;

	wchar_t*							_skyNodeTexturePath;
	float								_radius;
	size_t								_tesselationParam = 30;
	std::vector<Vertex>					_vertices;
	std::vector<UINT>					_indices;
	unsigned int						_numberOfVertices = 0;
	unsigned int						_numberOfIndices = 0;

	void BuildRendererStates();
	void BuildDepthStencilState();
	void CreateSphere(float radius, size_t tessellation);
	void BuildGeometryBuffers();
	void BuildShaders();
	void BuildVertexLayout();
	void BuildConstantBuffer();
	void BuildTexture();
};

