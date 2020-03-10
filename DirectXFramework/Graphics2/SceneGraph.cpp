#include "SceneGraph.h"

bool SceneGraph::Initialise(void)
{
	for (SceneNodePointer child : _children)
	{
		if (child->Initialise() == false)
		{
			return false;
		}
	}
	return true;
}

void SceneGraph::Update(FXMMATRIX& currentWorldTransformation)
{
	XMStoreFloat4x4(&_combinedWorldTransformation, XMLoadFloat4x4(&_worldTransformation) * currentWorldTransformation);
	
	for (SceneNodePointer child : _children)
	{
		child->Update(XMLoadFloat4x4(&_combinedWorldTransformation));
	}
}

void SceneGraph::Render(void)
{
	for (SceneNodePointer child : _children)
	{
		child->Render();
	}
}

void SceneGraph::Shutdown(void)
{
	for (SceneNodePointer child : _children)
	{
		child->Shutdown();
	}
}

void SceneGraph::Add(SceneNodePointer node)
{
	_children.push_back(node);
}

void SceneGraph::Remove(SceneNodePointer node)
{
	for (size_t i = 0; i < _children.size(); i++)
	{
		if (_children[i] == node)
		{
			_children.erase(_children.begin() + i);
		}
		else
		{
			_children[i]->Remove(node);
		}
	}
}

SceneNodePointer SceneGraph::Find(wstring name)
{
	if (this->_name == name)
	{
		return SceneNodePointer(this);
	}

	for (SceneNodePointer child : _children)
	{
		SceneNodePointer found = child->Find(name);
		if (found != nullptr)
		{
			return found;
		}
	}
	return nullptr;
}
