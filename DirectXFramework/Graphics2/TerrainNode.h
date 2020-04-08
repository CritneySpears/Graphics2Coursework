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

	ComPtr<ID3D11Buffer>			_vertexBuffer;
	ComPtr<ID3D11Buffer>			_indexBuffer;

	wstring							_terrainName;
	std::vector<VERTEX>				_vertices;
	std::vector<int>				_indices;

	void GenerateVerticesIndices();
	void CreateVertexIndexBuffers();
};

