#include "TerrainNode.h"
#include <vector>
#include <fstream>

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
	LoadHeightMap(_terrainName);
	LoadTerrainTextures();
	GenerateGeometry();
	CreateGeometryBuffers();
	GenerateBlendMap();
	BuildShaders();
	BuildVertexLayout();
	BuildConstantBuffer();
	//BuildRendererStates(); - wireframe.
	return true;
}

void TerrainNode::Render()
{
	XMMATRIX viewTransformation = DirectXFramework::GetDXFramework()->GetCamera()->GetViewMatrix();
	XMMATRIX completeTransformation = XMLoadFloat4x4(&_combinedWorldTransformation) * viewTransformation * DirectXFramework::GetDXFramework()->GetProjectionTransformation();
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

	_deviceContext->PSSetShaderResources(0, 1, _blendMapResourceView.GetAddressOf());
	_deviceContext->PSSetShaderResources(1, 1, _texturesResourceView.GetAddressOf());

	// Update the constant buffer
	UINT stride = sizeof(TerrainVertex);
	UINT offset = 0;
	_deviceContext->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &stride, &offset);
	_deviceContext->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//_deviceContext->RSSetState(_wireframeRasteriserState.Get()); - wireframe.
	_deviceContext->DrawIndexed((UINT)_indices.size(), 0, 0);
}

void TerrainNode::Shutdown()
{
}

void TerrainNode::GenerateGeometry()
{
	// Fill a 1024 x 1024 grid with vertices. Then create the indices for the terrain polygons.

	int heightValueIndex = 0;

	for (UINT z = 0; z < _numberOfZPoints; z++)
	{
		for (UINT x = 0; x < _numberOfXPoints; x++)
		{
			float worldHeightValue = 1024.0f;
			float yValue = _heightValues[heightValueIndex] * worldHeightValue;
			float u = (float)(x / _numberOfXPoints);
			float v = (float)(z / _numberOfZPoints);
			TerrainVertex currentVertex;
			currentVertex.Position = XMFLOAT3((float)x * _terrainCellSize, yValue, (float)z * _terrainCellSize);
			currentVertex.Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
			currentVertex.TexCoord = XMFLOAT2(0.0f, 0.0f);
			currentVertex.BlendMapTexCoord = XMFLOAT2(u, v);
			_vertices.push_back(currentVertex);
			heightValueIndex++;
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

	for (UINT z = 0; z < _numberOfZPoints - 1; z++)
	{
		for (UINT x = 0; x < _numberOfXPoints - 1; x++)
		{
			//Calculate face normal.
			//Add it to a vector.
			//move to next cell.

			//Get the three indices for the face.
			UINT index1 = (z * _numberOfZPoints) + x; // bottom left vertex
			UINT index2 = (z * _numberOfZPoints) + (x + 1); // bottom right vertex
			UINT index3 = ((z + 1) * _numberOfZPoints) + x; // top left vertex

			//Get the three vertices for the face.
			TerrainVertex vertex1 = _vertices[index1];
			TerrainVertex vertex2 = _vertices[index2];
			TerrainVertex vertex3 = _vertices[index3];

			//Calculate the two vectors for the face.
			XMFLOAT3 vector1;
			vector1.x = vertex1.Position.x - vertex3.Position.x;
			vector1.y = vertex1.Position.y - vertex3.Position.y;
			vector1.z = vertex1.Position.z - vertex3.Position.z;

			XMFLOAT3 vector2;
			vector2.x = vertex3.Position.x - vertex2.Position.x;
			vector2.y = vertex3.Position.y - vertex2.Position.y;
			vector2.z = vertex3.Position.z - vertex2.Position.z;

			//Get the cell number. Not used.
			UINT cellIndex = (z * (_numberOfZPoints - 1)) + x;

			//Calculate the face normal via cross-product.
			XMFLOAT3 faceNormal;
			faceNormal.x = (vector1.y * vector2.z) - (vector1.z * vector2.y);
			faceNormal.y = (vector1.z * vector2.x) - (vector1.x * vector2.z);
			faceNormal.z = (vector1.x * vector2.y) - (vector1.y * vector2.x);
			_faceNormals.push_back(faceNormal);

			// Set the texture co-ordinates at each corner of the cell face.

			UINT index4 = ((z + 1) * _numberOfZPoints) + (x + 1); // top right vertex

			_vertices.at(index1).TexCoord = XMFLOAT2(0.0f, 1.0f); // bottom left tex coord
			_vertices.at(index2).TexCoord = XMFLOAT2(1.0f, 1.0f); // bottom right tex coord
			_vertices.at(index3).TexCoord = XMFLOAT2(0.0f, 0.0f); // top left tex coord
			_vertices.at(index4).TexCoord = XMFLOAT2(1.0f, 0.0f); // top right tex coord
		}
	}

	//add adjacent average face normals to vertex normal.
	for (int z = 0; z < (int)_numberOfZPoints; z++)
	{
		for (int x = 0; x < (int)_numberOfXPoints; x++)
		{
			//Initialise sum.
			float sum[3] = { 0.0f, 0.0f, 0.0f };

			//Initialise index.
			int index = 0;

			//Initialise count.
			UINT count = 0;

			//Bottom-left face.
			if (((x - 1) >= 0) && ((z - 1) >= 0))
			{
				index = ((z - 1) * ((int)_numberOfZPoints - 1)) + (x - 1);
				sum[0] += _faceNormals[index].x;
				sum[1] += _faceNormals[index].y;
				sum[2] += _faceNormals[index].z;
				count++;
			}

			//Bottom-right face.
			if ((x < ((int)_numberOfXPoints - 1)) && ((z - 1) >= 0))
			{
				index = ((z - 1) * ((int)_numberOfZPoints - 1)) + x;
				sum[0] += _faceNormals[index].x;
				sum[1] += _faceNormals[index].y;
				sum[2] += _faceNormals[index].z;
				count++;
			}

			//Upper-left face.
			if (((x - 1) >= 0) && (z < ((int)_numberOfZPoints - 1)))
			{
				index = (z * ((int)_numberOfZPoints - 1)) + (x - 1);
				sum[0] += _faceNormals[index].x;
				sum[1] += _faceNormals[index].y;
				sum[2] += _faceNormals[index].z;
				count++;
			}

			//Upper-right face.
			if ((x < ((int)_numberOfXPoints - 1)) && (z < ((int)_numberOfZPoints - 1)))
			{
				index = (z * ((int)_numberOfZPoints - 1)) + x;
				sum[0] += _faceNormals[index].x;
				sum[1] += _faceNormals[index].y;
				sum[2] += _faceNormals[index].z;
				count++;
			}

			//Take average of faces.
			sum[0] = (sum[0] / (float)count);
			sum[1] = (sum[1] / (float)count);
			sum[2] = (sum[2] / (float)count);

			//Calculate length of normal.
			float length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			//Get index to vertex location in vector.
			index = ((z * (int)_numberOfZPoints) + x);

			//Normalize normal and add it to vertex container.
			_vertices.at(index).Normal = XMFLOAT3(sum[0] / length, sum[1] / length, sum[2] / length);
		}
	}
}

void TerrainNode::CreateGeometryBuffers()
{
	// Setup the structure that specifies how big the vertex 
	// buffer should be
	D3D11_BUFFER_DESC vertexBufferDescriptor;
	vertexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDescriptor.ByteWidth = sizeof(TerrainVertex) * (UINT)_vertices.size();
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
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 }
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

void TerrainNode::LoadTerrainTextures()
{
	// Change the paths below as appropriate for your use
	wstring terrainTextureNames[5] = { L"grass.dds", L"darkdirt.dds", L"stone.dds", L"lightdirt.dds", L"snow.dds" };

	// Load the textures from the files
	ComPtr<ID3D11Resource> terrainTextures[5];
	for (int i = 0; i < 5; i++)
	{
		ThrowIfFailed(CreateDDSTextureFromFileEx(_device.Get(),
			_deviceContext.Get(),
			terrainTextureNames[i].c_str(),
			0,
			D3D11_USAGE_IMMUTABLE,
			D3D11_BIND_SHADER_RESOURCE,
			0,
			0,
			false,
			terrainTextures[i].GetAddressOf(),
			nullptr
		));
	}
	// Now create the Texture2D arrary.  We assume all textures in the
	// array have the same format and dimensions

	D3D11_TEXTURE2D_DESC textureDescription;
	ComPtr<ID3D11Texture2D> textureInterface;
	terrainTextures[0].As<ID3D11Texture2D>(&textureInterface);
	textureInterface->GetDesc(&textureDescription);

	D3D11_TEXTURE2D_DESC textureArrayDescription;
	textureArrayDescription.Width = textureDescription.Width;
	textureArrayDescription.Height = textureDescription.Height;
	textureArrayDescription.MipLevels = textureDescription.MipLevels;
	textureArrayDescription.ArraySize = 5;
	textureArrayDescription.Format = textureDescription.Format;
	textureArrayDescription.SampleDesc.Count = 1;
	textureArrayDescription.SampleDesc.Quality = 0;
	textureArrayDescription.Usage = D3D11_USAGE_DEFAULT;
	textureArrayDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureArrayDescription.CPUAccessFlags = 0;
	textureArrayDescription.MiscFlags = 0;

	ComPtr<ID3D11Texture2D> textureArray = 0;
	ThrowIfFailed(_device->CreateTexture2D(&textureArrayDescription, 0, textureArray.GetAddressOf()));

	// Copy individual texture elements into texture array.

	for (UINT i = 0; i < 5; i++)
	{
		// For each mipmap level...
		for (UINT mipLevel = 0; mipLevel < textureDescription.MipLevels; mipLevel++)
		{
			_deviceContext->CopySubresourceRegion(textureArray.Get(),
				D3D11CalcSubresource(mipLevel, i, textureDescription.MipLevels),
				NULL,
				NULL,
				NULL,
				terrainTextures[i].Get(),
				mipLevel,
				nullptr
			);
		}
	}

	// Create a resource view to the texture array.
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDescription;
	viewDescription.Format = textureArrayDescription.Format;
	viewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDescription.Texture2DArray.MostDetailedMip = 0;
	viewDescription.Texture2DArray.MipLevels = textureArrayDescription.MipLevels;
	viewDescription.Texture2DArray.FirstArraySlice = 0;
	viewDescription.Texture2DArray.ArraySize = 5;

	ThrowIfFailed(_device->CreateShaderResourceView(textureArray.Get(), &viewDescription, _texturesResourceView.GetAddressOf()));
}

void TerrainNode::GenerateBlendMap()
{
	// Note that _numberOfRows and _numberOfColumns need to be setup
	// to the number of rows and columns in your grid in order for this
	// to work.
	
	int mapSize = _numberOfXPoints * _numberOfZPoints;
	DWORD* blendMap = new DWORD[mapSize];
	DWORD* blendMapPtr = blendMap;
	BYTE r;
	BYTE g;
	BYTE b;
	BYTE a;

	int index = 0;
	for (DWORD z = 0; z < _numberOfZPoints - 1; z++)
	{
		for (DWORD x = 0; x < _numberOfXPoints - 1; x++)
		{

			// Calculate the appropriate blend colour for the 
			// current location in the blend map.  This has been
			// left as an exercise for you.  You need to calculate
			// appropriate values for the r, g, b and a values (each
			// between 0 and 255). The code below combines these
			// into a DWORD (32-bit value) and stores it in the blend map.

			float slope = _faceNormals[index].y;
			int index1 = x;
			int index2 = x + 1;
			int index3 = x + (z * _numberOfZPoints);
			int index4 = (x + 1) + (z * _numberOfZPoints);

			float v1Height = _vertices.at(index1).Position.y;
			float v2Height = _vertices.at(index2).Position.y;
			float v3Height = _vertices.at(index3).Position.y;
			float v4Height = _vertices.at(index4).Position.y;
			float heightAverage = (v1Height + v2Height + v3Height + v4Height) / 4;

			r = 0;
			g = 0;
			b = 0;
			a = 0;



			DWORD mapValue = (a << 24) + (b << 16) + (g << 8) + r;
			*blendMapPtr++ = mapValue;
			index++;
		}
	}
	D3D11_TEXTURE2D_DESC blendMapDescription;
	blendMapDescription.Width = _numberOfXPoints;
	blendMapDescription.Height = _numberOfZPoints;
	blendMapDescription.MipLevels = 1;
	blendMapDescription.ArraySize = 1;
	blendMapDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	blendMapDescription.SampleDesc.Count = 1;
	blendMapDescription.SampleDesc.Quality = 0;
	blendMapDescription.Usage = D3D11_USAGE_DEFAULT;
	blendMapDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	blendMapDescription.CPUAccessFlags = 0;
	blendMapDescription.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA blendMapInitialisationData;
	blendMapInitialisationData.pSysMem = blendMap;
	blendMapInitialisationData.SysMemPitch = 4 * _numberOfZPoints;

	ComPtr<ID3D11Texture2D> blendMapTexture;
	ThrowIfFailed(_device->CreateTexture2D(&blendMapDescription, &blendMapInitialisationData, blendMapTexture.GetAddressOf()));

	// Create a resource view to the texture array.
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDescription;
	viewDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	viewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDescription.Texture2D.MostDetailedMip = 0;
	viewDescription.Texture2D.MipLevels = 1;

	ThrowIfFailed(_device->CreateShaderResourceView(blendMapTexture.Get(), &viewDescription, _blendMapResourceView.GetAddressOf()));
	delete[] blendMap;
}
