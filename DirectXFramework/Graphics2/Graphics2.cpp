#include "Graphics2.h"
#include "Cube.h"
#include "TerrainNode.h"
#include "SkyNode.h"

Graphics2 app;

void Graphics2::CreateSceneGraph()
{
	SceneGraphPointer sceneGraph = GetSceneGraph();
	GetCamera()->SetCameraPosition(300.0f, -450.0f, -1600.0f);

	// This is where you add nodes to the scene graph

	SceneNodePointer terrain = make_shared<TerrainNode>(L"SampleTerrain", L"Example_HeightMap.raw");
	terrain->SetWorldTransform(XMMatrixTranslation(-5120, -512, -5120) * XMMatrixRotationY(XMConvertToRadians(0)));
	sceneGraph->Add(terrain);

	SceneNodePointer plane = make_shared<MeshNode>(L"Plane1", L"\\University\\Graphics II\\Graphics2CourseWork\\DirectXFramework\\Plane\\Bonanza.3DS");
	plane->SetWorldTransform(XMMatrixRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(90.0f), XMConvertToRadians(0.0f)) * XMMatrixRotationX(XMConvertToRadians(-30)) * XMMatrixTranslation(700.0f, -300.0f, -900.0f));
	sceneGraph->Add(plane);

	SceneNodePointer rock = make_shared<MeshNode>(L"rock1", L"\\University\\Graphics II\\Graphics2CourseWork\\DirectXFramework\\Rock\\rock_3.obj");
	rock->SetWorldTransform(XMMatrixTranslation(1, 1, 1));
	sceneGraph->Add(rock);

	SceneNodePointer building = make_shared<MeshNode>(L"House", L"\\University\\Graphics II\\Graphics2CourseWork\\DirectXFramework\\Building\\WoodenCabinObj.obj");
	building->SetWorldTransform(XMMatrixTranslation(0, -480, -1100));
	sceneGraph->Add(building);

	SceneNodePointer skyBox = make_shared<SkyNode>(L"SkyBox", _skyBoxTexture, 5000.0f);
	//skyBox->SetWorldTransform(XMMatrixTranslationFromVector(GetCamera()->GetCameraPosition()));
	sceneGraph->Add(skyBox);
}

void Graphics2::UpdateSceneGraph()
{
	// This is where you make any changes to the local world transformations to nodes
	// in the scene graph

	SceneGraphPointer sceneGraph = GetSceneGraph();
	SceneNodePointer planeLocal = sceneGraph->Find(L"Plane1");
	SceneNodePointer terrainLocal = sceneGraph->Find(L"SampleTerrain");
	SceneNodePointer skyBoxLocal = sceneGraph->Find(L"SkyBox");
	_controller.ProcessGameController();

	_circleAngle += 0.50f;
	XMVECTOR camPosition = GetCamera()->GetCameraPosition();

	//skyBoxLocal->SetWorldTransform(XMMatrixTranslationFromVector(GetCamera()->GetCameraPosition()));
	planeLocal->SetWorldTransform(XMMatrixRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(90.0f), XMConvertToRadians(0.0f)) * XMMatrixRotationX(XMConvertToRadians(-30)) * XMMatrixTranslation(0.0f, 0.0f, 300.0f) * XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), -_circleAngle * XM_PI / 180.0f) * XMMatrixTranslation(700.0f, -300.0f, -900.0f));

	// This is where controls for moving the camera are set.

	// Move Forwards with "W"
	if (GetAsyncKeyState(0x57) < 0)
	{
		GetCamera()->SetForwardBack(_movementSpeed);
	}
	// Move Backwards with "S"
	if (GetAsyncKeyState(0x53) < 0)
	{
		GetCamera()->SetForwardBack(-_movementSpeed);
	}

	// Move Left with "A"
	if (GetAsyncKeyState(0x41) < 0)
	{
		GetCamera()->SetLeftRight(-_movementSpeed);
	}
	// Move Right with "D"
	if (GetAsyncKeyState(0x44) < 0)
	{
		GetCamera()->SetLeftRight(_movementSpeed);
	}
	
	// Look Left with "LEFT ARROW"
	if (GetAsyncKeyState(VK_LEFT) < 0)
	{
		GetCamera()->SetYaw(-_movementSpeed);
	}
	// Look Right with "RIGHT ARROW"
	if (GetAsyncKeyState(VK_RIGHT) < 0)
	{
		GetCamera()->SetYaw(_movementSpeed);
	}

	// Look Up with "UP ARROW"
	if (GetAsyncKeyState(VK_UP) < 0)
	{
		GetCamera()->SetPitch(-_movementSpeed);
	}
	//Look Down with "DOWN ARROW"
	if (GetAsyncKeyState(VK_DOWN) < 0)
	{
		GetCamera()->SetPitch(_movementSpeed);
	}

	// Mouse Movement
}
