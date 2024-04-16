#pragma once

enum class eAction
{
	Pause,
	Quit,
	ToggleTexture,
	ToggleCollision,
	ToggleHotMenu,
	TogglePauseMenu,
	ToggleGrid,
	ToggleCamera,
	MoveLeft,
	MoveRight,
	MoveUp,
	MoveDown,
	Attack,
	CursorUp,
	CursorDown,
	SelectOption,
	Special,
	New,
	RClick,
	LClick,
	CycleLeft,
	CycleRight,
	WheelUp,
	WheelDown,
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
