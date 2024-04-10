#pragma once

enum class eAction
{
	Pause,
	Quit,
	ToggleTexture,
	ToggleCollision,
	ToggleGrid,
	ToggleCamera,
	MoveLeft,
	MoveRight,
	MoveUp,
	MoveDown,
	Attack,
	ActionCount
};

enum class eActionType
{
	Start,
	Stop,
	ActionTypeCount
};
