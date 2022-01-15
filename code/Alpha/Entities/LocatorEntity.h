/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Entities/GameEntity.h"
namespace DirectX
{
	struct XMMATRIX;
}

class TransformComponent;

//This entity contains a transform component by default
class LocatorEntity : public GameEntity
{
public:
	LocatorEntity();
	virtual ~LocatorEntity();

	const DirectX::XMMATRIX& GetTransform() const;

protected:
	TransformComponent* m_pTransform;
};
