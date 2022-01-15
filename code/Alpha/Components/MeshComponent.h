/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Components/GameComponent.h"

#include "Rendering/Renderable/RenderableId.h"

class MeshComponent : public GameComponent
{
public:
	MeshComponent(GameEntity* pParent, RenderableId id);
	virtual ~MeshComponent();

	void Render() override;

private:
	RenderableId m_renderableId;
};
