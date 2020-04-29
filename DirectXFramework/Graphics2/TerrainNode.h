#pragma once

#include "SceneNode.h"
#include "DirectXFramework.h"
#include "ResourceManager.h"

class TerrainNode : public SceneNode
{
public:
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

	ComPtr<ID3D11RasterizerState>		_defaultRasteriserState;
	ComPtr<ID3D11RasterizerState>		_wireframeRasteriserState;

	UINT								_numberOfXPoints = 1024;
	UINT								_numberOfZPoints = 1024;
	UINT								_vertexCount = _numberOfXPoints * _numberOfZPoints;
	UINT								_indexCount = (_numberOfXPoints - 1) * (_numberOfZPoints - 1) * 6;
	UINT								_TerrainCellSize = 10;

	wstring								_terrainName;
	std::vector<VERTEX>					_vertices;
	std::vector<UINT>					_indices;


	//bool LoadHeightMap(wstring heightMapFilename);
	void BuildRendererStates();
	void GenerateGeometry();
	void CreateGeometryBuffers();
	void BuildShaders();
	void BuildVertexLayout();
	void BuildConstantBuffer();
};

