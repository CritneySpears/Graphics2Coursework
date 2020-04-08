#include "TerrainNode.h"
#include <vector>


bool TerrainNode::Initialise()
{
	//Load Height Map.
	//Generate Vertices and Indices for polygons in the terrain grid.
	//Generate Normals for Polygons.
	//Create vertex and Index buffers for terran polygons.
	return true;
}

void TerrainNode::Render()
{
}

void TerrainNode::Shutdown()
{
}

void TerrainNode::GenerateVerticesIndices()
{
	// Fill a 1024 x 1024 grid with vertices. Then create the indices for the terrain polygons.

	for (int x = -5120; x < 5110; x+= 10)
	{
		for (int z = -5120; z < 5110; z+=10)
		{
			VERTEX * currentVertex = new VERTEX;
			currentVertex->Position =	XMFLOAT3((float)x, 0.0f, (float)z);
			currentVertex->Normal =		XMFLOAT3(0.0f, 0.0f, 0.0f);
			currentVertex->TexCoord =	XMFLOAT2(0.0f, 0.0f);

			_vertices.push_back(*currentVertex);
		}
	}

	for (int x = 0; x <= 1023; x+=3)
	{
		// Indices for triangle 1

		_indices.push_back(x);
		_indices.push_back(x + 1);
		_indices.push_back(x + 2);

		//Indices for triangle 2

		_indices.push_back(x + 2);
		_indices.push_back(x + 1);
		_indices.push_back(x + 3);
	}
}

void TerrainNode::CreateVertexIndexBuffers()
{
	D3D11_SUBRESOURCE_DATA vertexInitialisationData;
	vertexInitialisationData.pSysMem = &_vertices[0];
}
