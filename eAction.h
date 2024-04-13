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
	//menu actions
	CursorUp,
	CursorDown,
	SelectOption,
	Special,
	ActionCount
};

enum class eActionType
{
	Start,
	Stop,
	ActionTypeCount
};

enum class eSpecial
{
	Forcefield,
	ElectronCannon,
	Disabled,
	SpecialCount,
};
