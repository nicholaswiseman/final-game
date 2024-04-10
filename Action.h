#pragma once

#include "eAction.h"

class Action
{
	eAction m_name;
	eActionType m_type;

public:
	Action();
	Action(eAction name, eActionType type);

public:
	const eAction GetName() const;
	const eActionType GetType() const;
};

