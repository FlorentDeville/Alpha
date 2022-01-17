/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Entities/LocatorEntity.h"

#include "DirectXMath.h"

class PlayerEntity : public LocatorEntity
{
public:
	PlayerEntity();
	virtual ~PlayerEntity();

	void Update() override;

private:
	char m_padding[8];
	DirectX::XMVECTOR m_position;
	DirectX::XMVECTOR m_euler;
	DirectX::XMVECTOR m_scale;
};
