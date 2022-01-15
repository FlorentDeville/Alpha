/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "GameComponent.h"
#include "DirectXMath.h"

//Represent a transform in space
class TransformComponent : public GameComponent
{
public:
	TransformComponent(GameEntity* pParent);
	virtual ~TransformComponent();

	void Update() override;
	void Render() override;

	void SetTransform(const DirectX::XMMATRIX& transform);

	const DirectX::XMMATRIX& GetTransform() const;

private:
	DirectX::XMMATRIX m_transform;
};
