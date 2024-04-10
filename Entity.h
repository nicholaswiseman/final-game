#pragma once

#include "eEntityTags.h"
#include "Components.h"
#include <tuple>

class EntityManager;

typedef std::tuple<
	CTransform,
	CInput,
	CBoundingBox,
	CAnimation,
	CCollision,
	CDamage,
	CHealth,
	CBehavior,
	CBlockVision,
	CInvincibility
> ComponentTuple;

class Entity
{
	friend class EntityManager;

private:
	Entity(eEntityTag tag, size_t totalEntities);

	ComponentTuple m_components;
	eEntityTag m_tag;
	bool m_active;
	size_t m_id;

public:

	void markInactive();
	bool isInactive();
	eEntityTag getTag();
	size_t getId();

	template <typename T>
	bool HasComponent() const
	{
		return GetComponent<T>().has;
	}

	template <typename T, typename... TArgs>
	T& AddComponent(TArgs&&... mArgs)
	{
		auto& component = GetComponent<T>();
		component = T(std::forward<TArgs>(mArgs)...);
		component.has = true;
		return component;
	}

	template<typename T>
	T& GetComponent()
	{
		return std::get<T>(m_components);
	}

	template<typename T>
	const T& GetComponent() const
	{
		return std::get<T>(m_components);
	}

	template<typename T>
	void RemoveComponent()
	{
		GetComponent<T>() = T();
	}
};

