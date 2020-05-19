#pragma once

#include "SceneNode.h"
#include "DirectXFramework.h"

class TerrainNode : public SceneNode
{
public:

	struct CBUFFER
	{
		XMMATRIX    CompleteTransformation;
		XMMATRIX	WorldTransformation;
		XMFLOAT4	CameraPosition;
		XMVECTOR    LightVector;
		XMFLOAT4    LightColor;
		XMFLOAT4    AmbientColor;
		XMFLOAT4    DiffuseCoefficient;
		XMFLOAT4	SpecularCoefficient;
		float		Shininess;
		float		Opacity;
		float       Padding[2];
	};

	struct TerrainVertex
	{
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT2 TexCoord;
		XMFLOAT2 BlendMapTexCoord;
	};


	TerrainNode(wstring name, wstring terrainName) : SceneNode(name) { _terrainName = terrainName; }

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

	XMFLOAT4							_cameraPosition;

	ComPtr<ID3D11RasterizerState>		_defaultRasteriserState;
	ComPtr<ID3D11RasterizerState>		_wireframeRasteriserState;

	ComPtr<ID3D11ShaderResourceView>	_texturesResourceView;
	ComPtr<ID3D11ShaderResourceView>	_blendMapResourceView;

	UINT								_numberOfXPoints = 1024;
	UINT								_numberOfZPoints = 1024;
	UINT								_vertexCount = _numberOfXPoints * _numberOfZPoints;
	UINT								_indexCount = (_numberOfXPoints - 1) * (_numberOfZPoints - 1) * 6;
	UINT								_terrainCellSize = 10;

	wstring								_terrainName;
	std::vector<TerrainVertex>			_vertices;
	std::vector<UINT>					_indices;
	std::vector<XMFLOAT3>				_faceNormals;
	std::vector<float>					_heightValues;
	std::vector<float>					_avgHeights;


	void BuildRendererStates();
	void GenerateGeometry();
	void CreateGeometryBuffers();
	void BuildShaders();
	void BuildVertexLayout();
	void BuildConstantBuffer();
	bool LoadHeightMap();
	void LoadTerrainTextures();
	void GenerateBlendMap();
};

