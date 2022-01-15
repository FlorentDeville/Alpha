/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

class GameEntity;

//A component is an atomic basic block for entities.
class GameComponent
{
public:
	GameComponent(GameEntity* pParent);
	virtual ~GameComponent();

	virtual void Update();
	virtual void Render();

protected:
	GameEntity* m_pParent;
};
