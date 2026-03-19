#pragma once

#include <IExeEngine/Inc/IExeEngine.h>

enum class GameEventType
{
	CollisionEvent = 1,
	FireworkEvent
};

class CollisionEvent : public IExeEngine::Core::Event
{
public:
	CollisionEvent() {}
	SET_EVENT_TYPE_ID(GameEventType::CollisionEvent)
};

class FireworkEvent : public IExeEngine::Core::Event
{
public:
	FireworkEvent() {}
	SET_EVENT_TYPE_ID(GameEventType::FireworkEvent)
};