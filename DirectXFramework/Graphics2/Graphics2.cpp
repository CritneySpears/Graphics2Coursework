#include "Graphics2.h"
#include "Cube.h"
#include "TerrainNode.h"

Graphics2 app;

void Graphics2::CreateSceneGraph()
{
	SceneGraphPointer sceneGraph = GetSceneGraph();
	GetCamera()->SetCameraPosition(0.0f, 50.0f, -500.0f);

	SceneNodePointer terrain = make_shared<TerrainNode>(L"SampleTerrain", L"Example_HeightMap.raw");
	terrain->SetWorldTransform(XMMatrixTranslation(-5120, -1024, -5120) * XMMatrixRotationY(XMConvertToRadians(0)));
	sceneGraph->Add(terrain);

	SceneNodePointer node = make_shared<MeshNode>(L"Plane1", L"\\University\\Graphics II\\Graphics2CourseWork\\DirectXFramework\\Plane\\Bonanza.3DS");
	node->SetWorldTransform(XMMatrixRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(90), 0));
	sceneGraph->Add(node);
	
	// This is where you add nodes to the scene graph

	/*SceneNodePointer body = make_shared<Cube>(L"Body", _woodTexture);
	body->SetWorldTransform(XMMatrixScaling(5, 8, 2.5) * XMMatrixTranslation(0, 23, 0));
	sceneGraph->Add(body);

	SceneNodePointer leftLeg = make_shared<Cube>(L"Left Leg", _concreteTexture);
	leftLeg->SetWorldTransform(XMMatrixScaling(1, 7.5, 1) * XMMatrixTranslation(-4, 7.5, 0));
	sceneGraph->Add(leftLeg);

	SceneNodePointer rightLeg = make_shared<Cube>(L"Right Leg", _concreteTexture);
	rightLeg->SetWorldTransform(XMMatrixScaling(1, 7.5, 1) * XMMatrixTranslation(4, 7.5, 0));
	sceneGraph->Add(rightLeg);

	SceneNodePointer head = make_shared<Cube>(L"Head", _woodBoxTexture);
	head->SetWorldTransform(XMMatrixScaling(3, 3, 3) * XMMatrixTranslation(0, 34, 0));
	sceneGraph->Add(head);

	SceneNodePointer leftArm = make_shared<Cube>(L"Left Arm", _brickTexture);
	leftArm->SetWorldTransform(XMMatrixScaling(1, 8.5, 1) * XMMatrixTranslation(-6, 22, 0));
	sceneGraph->Add(leftArm);

	SceneNodePointer rightArm = make_shared<Cube>(L"right Arm", _brickTexture);
	rightArm->SetWorldTransform(XMMatrixScaling(1, 8.5, 1) * XMMatrixTranslation(6, 22, 0));
	sceneGraph->Add(rightArm);*/
}

void Graphics2::UpdateSceneGraph()
{
	SceneGraphPointer sceneGraph = GetSceneGraph();
	SceneNodePointer planeLocal = sceneGraph->Find(L"Plane1");
	SceneNodePointer terrainLocal = sceneGraph->Find(L"SampleTerrain");
	_controller.ProcessGameController();

	_circleAngle += 1.0f;

	planeLocal->SetWorldTransform(XMMatrixRotationRollPitchYaw(XMConvertToRadians(90), XMConvertToRadians(90.0f), 0.0f) * XMMatrixTranslation(0.0f, 0.0f, 100.0f) * XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.2f, 0.0f), -_circleAngle * XM_PI / 180.0f));
	// This is where you make any changes to the local world transformations to nodes
	// in the scene graph

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
