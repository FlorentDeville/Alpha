/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Widgets/Widgets/Container_V2.h"

namespace Widgets
{
	Container_V2::Container_V2()
		: Widget()
	{
		m_virtualSize.x = INT32_MAX * 0.5f;
		m_virtualSize.y = INT32_MAX * 0.5f;

		//debug
		m_showBorder = true;
		m_borderWidth = 1;
		m_borderColor = Color(255, 255, 255);
	}

	Container_V2::~Container_V2()
	{ }

	void Container_V2::Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor)
	{
		D3D12_RECT innerRect;
		innerRect.left = m_absPos.x;
		innerRect.right = m_absPos.x + m_size.x;
		innerRect.top = m_absPos.y;
		innerRect.bottom = m_absPos.y + m_size.y;
		Parent::Draw(windowSize, innerRect);
	}

	bool Container_V2::Handle(const BaseEvent& ev)
	{
		return Parent::Handle(ev);
	}

	void Container_V2::ResizeChildren()
	{
		//Parent::ResizeChildren();
		for (Widget* pChild : m_children)
			pChild->Resize(m_absPos, m_virtualSize);
	}
}
