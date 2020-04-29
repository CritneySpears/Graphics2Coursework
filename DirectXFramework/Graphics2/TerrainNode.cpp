#include "TerrainNode.h"
#include "Cube.h"
#include <vector>

struct CBUFFER
{
	XMMATRIX    CompleteTransformation;
	XMMATRIX	WorldTransformation;
	XMVECTOR    LightVector;
	XMFLOAT4    LightColour;
	XMFLOAT4    AmbientColour;
};

bool TerrainNode::Initialise()
{
	//Load Height Map.
	//Generate Vertices and Indices for polygons in the terrain grid.
	//Generate Normals for Polygons.
	//Create vertex and Index buffers for terrain polygons.
	GenerateGeometry();
	CreateGeometryBuffers();
	BuildShaders();
	BuildVertexLayout();
	BuildConstantBuffer();
	BuildRendererStates();
	return true;
}

void TerrainNode::Render()
{
	XMMATRIX completeTransformation = XMLoadFloat4x4(&_combinedWorldTransformation) * DirectXFramework::GetDXFramework()->GetViewTransformation() * DirectXFramework::GetDXFramework()->GetProjectionTransformation();
	CBUFFER cBuffer;
	cBuffer.CompleteTransformation = completeTransformation;
	cBuffer.WorldTransformation = XMLoadFloat4x4(&_combinedWorldTransformation);
	cBuffer.AmbientColour = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	cBuffer.LightVector = XMVector4Normalize(XMVectorSet(0.0f, 01.0f, 1.0f, 0.0f));
	cBuffer.LightColour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// Update the constant buffer 
	_deviceContext->VSSetConstantBuffers(0, 1, _constantBuffer.GetAddressOf());
	_deviceContext->UpdateSubresource(_constantBuffer.Get(), 0, 0, &cBuffer, 0, 0);

	_deviceContext->VSSetShader(_vertexShader.Get(), 0, 0);
	_deviceContext->PSSetShader(_pixelShader.Get(), 0, 0);
	_deviceContext->IASetInputLayout(_layout.Get());
	// Update the constant buffer
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
	_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_deviceContext->RSSetState(_wireframeRasteriserState.Get());
	_deviceContext->DrawIndexed((UINT)_indices.size(), 0, 0);
}

void TerrainNode::Shutdown()
{
}

void TerrainNode::GenerateGeometry()
{
	// Fill a 1024 x 1024 grid with vertices. Then create the indices for the terrain polygons.

	/*
	VERTEX * v1 = new VERTEX;
	VERTEX * v2 = new VERTEX;
	VERTEX * v3 = new VERTEX;
	v1->Position = XMFLOAT3(-100.0f, -100.0f, 100.0f);
	v2->Position = XMFLOAT3(100.0f, -100.0f, 100.0f);
	v3->Position = XMFLOAT3(-100.0f, -100.0f, 100.0f);

	v1->Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	v2->Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
	v3->Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);

	v1->TexCoord = XMFLOAT2(0.0f, 0.0f);
	v2->TexCoord = XMFLOAT2(0.0f, 0.0f);
	v3->TexCoord = XMFLOAT2(0.0f, 0.0f);

	_vertices.push_back(*v1);
	_vertices.push_back(*v2);
	_vertices.push_back(*v3);

	UINT i1 = 1;
	UINT i2 = 2;
	UINT i3 = 3;

	_indices.push_back(i1);
	_indices.push_back(i2);
	_indices.push_back(i3);
	*/

	for (UINT z = 0; z < _numberOfZPoints; z++)
	{
		for (UINT x = 0; x < _numberOfXPoints; x++)
		{
			VERTEX currentVertex;
			currentVertex.Position = XMFLOAT3((float)x, 0, (float)z);
			currentVertex.Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
			currentVertex.TexCoord = XMFLOAT2(0.0f, 0.0f);

			_vertices.push_back(currentVertex);

		}
	}

	UINT k = 0;

	for (UINT i = 0; i < _indexCount; i += 6)
	{
		UINT currentIndices[6] = {};
		currentIndices[0] = k;
		currentIndices[1] = k + _numberOfZPoints;
		currentIndices[2] = k + _numberOfZPoints + 1;
		currentIndices[3] = k;
		currentIndices[4] = k + _numberOfZPoints + 1;
		currentIndices[5] = k + 1;

		k++;

		for each (UINT indices in currentIndices)
		{
			_indices.push_back(indices);
		}

	}

	/*
	for (int z = 5110; z > -5120; z -= _TerrainCellSize)
	{
		for (int x = -5120; x < 5110; x += _TerrainCellSize)
		{
			float y = 0.0f;
			VERTEX * currentVertex = new VERTEX;
			currentVertex->Position =	XMFLOAT3((float)x, y, (float)z);
			currentVertex->Normal =		XMFLOAT3(0.0f, 0.0f, 0.0f);
			currentVertex->TexCoord =	XMFLOAT2(0.0f, 0.0f);
			
			_vertices.push_back(*currentVertex);
		}
	}
	for (unsigned int cellZ = 1; cellZ <= (_numberOfZPoints - 1); cellZ++)
	{
		for (unsigned int cellX = 1; cellX <= (_numberOfXPoints - 1); cellX++)
		{
			// Indices for triangle 1

			_indices.push_back(cellX + (cellZ * (_numberOfXPoints - 1)));
			_indices.push_back(cellX + 1 + (cellZ * (_numberOfXPoints - 1)));
			_indices.push_back(cellX + ((cellZ + 1) * (_numberOfXPoints - 1)));

			//Indices for triangle 2

			_indices.push_back(cellX + ((cellZ + 1) * (_numberOfXPoints - 1)));
			_indices.push_back(cellX + 1 + (cellZ * (_numberOfXPoints - 1)));
			_indices.push_back(cellX + 1 + ((cellZ + 1) * (_numberOfXPoints - 1)));
		}
	}
	*/
}

void TerrainNode::CreateGeometryBuffers()
{
	// Setup the structure that specifies how big the vertex 
	// buffer should be
	D3D11_BUFFER_DESC vertexBufferDescriptor;
	vertexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDescriptor.ByteWidth = sizeof(VERTEX) * (UINT)_vertices.size();
	vertexBufferDescriptor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDescriptor.CPUAccessFlags = 0;
	vertexBufferDescriptor.MiscFlags = 0;
	vertexBufferDescriptor.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexInitialisationData;
	vertexInitialisationData.pSysMem = &_vertices[0];

	// and create the vertex buffer
	ThrowIfFailed(_device->CreateBuffer(&vertexBufferDescriptor, &vertexInitialisationData, _vertexBuffer.GetAddressOf()));

	// Setup the structure that specifies how big the index 
	// buffer should be
	D3D11_BUFFER_DESC indexBufferDescriptor;
	indexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDescriptor.ByteWidth = sizeof(UINT) * (UINT)_indices.size();
	indexBufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDescriptor.CPUAccessFlags = 0;
	indexBufferDescriptor.MiscFlags = 0;
	indexBufferDescriptor.StructureByteStride = 0;

	// Now set up a structure that tells DirectX where to get the
	// data for the indices from
	D3D11_SUBRESOURCE_DATA indexInitialisationData;
	indexInitialisationData.pSysMem = &_indices[0];

	// and create the index buffer
	ThrowIfFailed(_device->CreateBuffer(&indexBufferDescriptor, &indexInitialisationData, _indexBuffer.GetAddressOf()));
}

void TerrainNode::BuildShaders()
{
	DWORD shaderCompileFlags = 0;
#if defined( _DEBUG )
	shaderCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ComPtr<ID3DBlob> compilationMessages = nullptr;

	//Compile vertex shader
	HRESULT hr = D3DCompileFromFile(L"TerrainShaders.hlsl",
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"VShader", "vs_5_0",
		shaderCompileFlags, 0,
		_vertexShaderByteCode.GetAddressOf(),
		compilationMessages.GetAddressOf());

	if (compilationMessages.Get() != nullptr)
	{
		// If there were any compilation messages, display them
		MessageBoxA(0, (char*)compilationMessages->GetBufferPointer(), 0, 0);
	}
	// Even if there are no compiler messages, check to make sure there were no other errors.
	ThrowIfFailed(hr);
	ThrowIfFailed(_device->CreateVertexShader(_vertexShaderByteCode->GetBufferPointer(), _vertexShaderByteCode->GetBufferSize(), NULL, _vertexShader.GetAddressOf()));

	// Compile pixel shader
	hr = D3DCompileFromFile(L"TerrainShaders.hlsl",
		nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PShader", "ps_5_0",
		shaderCompileFlags, 0,
		_pixelShaderByteCode.GetAddressOf(),
		compilationMessages.GetAddressOf());

	if (compilationMessages.Get() != nullptr)
	{
		// If there were any compilation messages, display them
		MessageBoxA(0, (char*)compilationMessages->GetBufferPointer(), 0, 0);
	}
	ThrowIfFailed(hr);
	ThrowIfFailed(_device->CreatePixelShader(_pixelShaderByteCode->GetBufferPointer(), _pixelShaderByteCode->GetBufferSize(), NULL, _pixelShader.GetAddressOf()));
}

void TerrainNode::BuildVertexLayout()
{
	// Create the vertex input layout. This tells DirectX the format
	// of each of the vertices we are sending to it.

	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	ThrowIfFailed(_device->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), _vertexShaderByteCode->GetBufferPointer(), _vertexShaderByteCode->GetBufferSize(), _layout.GetAddressOf()));
}

void TerrainNode::BuildRendererStates()
{
	// Set default and wireframe rasteriser states
	D3D11_RASTERIZER_DESC rasteriserDesc;
	rasteriserDesc.FillMode = D3D11_FILL_SOLID;
	rasteriserDesc.CullMode = D3D11_CULL_BACK;
	rasteriserDesc.FrontCounterClockwise = false;
	rasteriserDesc.DepthBias = 0;
	rasteriserDesc.SlopeScaledDepthBias = 0.0f;
	rasteriserDesc.DepthBiasClamp = 0.0f;
	rasteriserDesc.DepthClipEnable = true;
	rasteriserDesc.ScissorEnable = false;
	rasteriserDesc.MultisampleEnable = false;
	rasteriserDesc.AntialiasedLineEnable = false;
	ThrowIfFailed(_device->CreateRasterizerState(&rasteriserDesc, _defaultRasteriserState.GetAddressOf()));
	rasteriserDesc.FillMode = D3D11_FILL_WIREFRAME;
	ThrowIfFailed(_device->CreateRasterizerState(&rasteriserDesc, _wireframeRasteriserState.GetAddressOf()));
}

void TerrainNode::BuildConstantBuffer()
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(CBUFFER);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	ThrowIfFailed(_device->CreateBuffer(&bufferDesc, NULL, _constantBuffer.GetAddressOf()));
}

/*
bool TerrainNode::LoadHeightMap(wstring heightMapFilename)
{
	unsigned int mapSize = _numberOfXPoints * _numberOfZPoints;
	USHORT* rawFileValues = new USHORT[mapSize];

	std::ifstream inputHeightMap;
	inputHeightMap.open(heightMapFilename.c_str(), std::ios_base::binary);
	if (!inputHeightMap)
	{
		return false;
	}

	inputHeightMap.read((char*)rawFileValues, mapSize * 2);
	inputHeightMap.close();

	// Normalise BYTE values to the range 0.0f - 1.0f;
	for (unsigned int i = 0; i < mapSize; i++)
	{
		_heightValues.push_back((float)rawFileValues[i] / 65536);
	}
	delete[] rawFileValues;
	return true;
}
*/