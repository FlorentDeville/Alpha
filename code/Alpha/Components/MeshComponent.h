/********************************************************************/
/* � 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Components/GameComponent.h"

#include "Rendering/Material/MaterialId.h"
#include "Rendering/Mesh/MeshId.h"

class MeshComponent : public GameComponent
{
public:
	MeshComponent(GameEntity* pParent, Rendering::MeshId meshId, Rendering::MaterialId materialId);
	virtual ~MeshComponent();

	void Render() override;

private:
	Rendering::MeshId m_meshId;
	Rendering::MaterialId m_materialId;
};
