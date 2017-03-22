#include "THGameObject.h"
#include "THDataStack.h"

namespace THEngine
{
	GameObject::GameObject()
	{
		needRemove = false;

		tweenManager = new TweenManager();
		tweenManager->Retain();
	}

	GameObject::GameObject(const GameObject& object) : EngineObject(object), children(object.children)
	{
		position = object.position;
		color = object.color;
		alpha = object.alpha;
		scale = object.scale;
		rotation3D = object.rotation3D;

		tweenManager = new TweenManager(*object.tweenManager);
		tweenManager->Retain();
		
		needRemove = false;
		
	}

	GameObject::~GameObject()
	{
		TH_SAFE_RELEASE(tweenManager);
	}

	Object* GameObject::Clone()
	{
		return new GameObject(*this);
	}

	void GameObject::AddChild(GameObject* obj)
	{
		children.Add(obj);
	}

	void GameObject::Visit()
	{
		Iterator<GameObject*>* iter = children.GetIterator();

		WriteRenderData();

		SendToRenderQueue();

		PushDataStack();
		while (iter->HasNext())
		{
			iter->Next()->Visit();
		}
		PopDataStack();
	}

	void GameObject::SendToRenderQueue()
	{

	}

	void GameObject::WriteRenderData()
	{
		auto dataStack = DataStack::GetInstance();

		this->positionForRender = dataStack->position + this->position;
	}

	void GameObject::PushDataStack()
	{
		auto dataStack = DataStack::GetInstance();

		dataStack->position = this->positionForRender;
	}

	void GameObject::PopDataStack()
	{
		auto dataStack = DataStack::GetInstance();

		dataStack->position -= this->position;
	}

	void GameObject::Update()
	{
		EngineObject::Update();

		tweenManager->Update();

		Iterator<GameObject*>* iter = children.GetIterator();
		while (iter->HasNext())
		{
			auto obj = iter->Next();
			if (obj->IsPaused())
			{
				continue;
			}
			if (obj->needRemove)
			{
				obj->OnDestroy();
				iter->Remove();
			}
			else
			{
				obj->Update();
			}
		}
	}

	void GameObject::Draw()
	{

	}

	void GameObject::DrawGeometry()
	{

	}

	void GameObject::GetWorldMatrix(Matrix* world)
	{
		Matrix::Identity(world);

		Matrix temp;
		Matrix::Scale(&temp, GetScale().x, GetScale().y, GetScale().z);
		*world *= temp;

		Matrix::RotateQuarternion(&temp, rotation3D);
		*world *= temp;

		Matrix::Translate(&temp, floor(0.5f + positionForRender.x), floor(0.5f + positionForRender.y), positionForRender.z);
		*world *= temp;
	}

	void GameObject::OnDestroy()
	{
		RemoveAllChildren();
	}

	void GameObject::RemoveChild(GameObject* child)
	{
		auto iter = children.GetIterator();
		while (iter->HasNext())
		{
			auto curChild = iter->Next();
			if (curChild == child)
			{
				iter->Remove();
				return;
			}
			curChild->RemoveChild(child);
		}
	}

	void GameObject::RemoveChildAt(int index)
	{
		children.RemoveAt(index);
	}

	void GameObject::RemoveAllChildren()
	{
		auto iter = children.GetIterator();
		while (iter->HasNext())
		{
			auto child = iter->Next();
			child->OnDestroy();
			iter->Remove();
		}
	}

	void GameObject::AddTween(Tween* tween)
	{
		tween->Bind(this);
		tweenManager->AddTween(tween);
	}

	void GameObject::ClearTweens()
	{
		tweenManager->ClearTweens();
	}

	void GameObject::SetRotationByAxis(const Vector3f& axis, float rotation)
	{
		Quaternion::RotateAngleAxis(&rotation3D, Vector3f(axis.x, axis.y, axis.z), rotation);
	}
}

