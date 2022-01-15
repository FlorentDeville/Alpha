/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <vector>

class GameComponent;

// A game entity is a collection of components
class GameEntity
{
public:
	GameEntity();
	virtual ~GameEntity();

	void AddGameComponent(GameComponent* pComponent);

	virtual void Update();
	virtual void Render();

protected:
	std::vector<GameComponent*> m_components;
};
