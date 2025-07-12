/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/SplitHorizontal.h"

#include "Widgets/Container.h"
#include "Widgets/Layout.h"
#include "Widgets/Split.h"
#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	SplitHorizontal::SplitHorizontal()
		: Widget()
		, m_pTopContainer(nullptr)
		, m_pBottomContainer(nullptr)
		, m_pSplit(nullptr)
		, m_pLayout(nullptr)
		, m_splitDragged(false)
	{
		m_pLayout = new Layout(0, 0, 0, 0);
		m_pLayout->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
		m_pLayout->SetDirection(Layout::Direction::Vertical);
		Widget::AddWidget(m_pLayout);

		m_pTopContainer = new Container(0, 200);
		m_pTopContainer->SetSizeStyle(Widget::HSIZE_STRETCH);
		m_pLayout->AddWidget(m_pTopContainer);

		m_pSplit = new Split(false);
		m_pSplit->SetSizeStyle(Widget::HSIZE_STRETCH);
		m_pSplit->OnDrag([this](const Core::Int2& mousePosition)
			{
				const int MIN_SIZE = 50;

				Core::UInt2 topContainerSize = m_pTopContainer->GetSize();

				int offset = mousePosition.y - (m_pTopContainer->GetScreenY() + topContainerSize.y);
				int newTopContainerWidth = topContainerSize.y + offset;

				if (newTopContainerWidth < MIN_SIZE)
					return;

				int newBottonContainerWidth = m_pBottomContainer->GetHeight() - offset;
				if (newBottonContainerWidth < MIN_SIZE)
					return;

				m_pTopContainer->SetHeight(newTopContainerWidth);
				m_pBottomContainer->SetHeight(newBottonContainerWidth);

				WidgetMgr::Get().RequestResize();
			});

		m_pLayout->AddWidget(m_pSplit);

		m_pBottomContainer = new Container(0, 200);
		m_pBottomContainer->SetSizeStyle(Widget::VSIZE_STRETCH | Widget::HSIZE_STRETCH);
		m_pLayout->AddWidget(m_pBottomContainer);
	}

	SplitHorizontal::~SplitHorizontal()
	{}

	void SplitHorizontal::Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor)
	{
		Widget::Draw(windowSize, scissor);
	}

	void SplitHorizontal::Resize(const Core::Int3& parentAbsPos, const Core::UInt2& parentSize)
	{
		ReComputeSize(parentSize);
		ReComputePosition(parentAbsPos, parentSize);

		for (Widget* pChild : m_children)
			pChild->Resize(m_absPos, m_size);
	}

	void SplitHorizontal::AddWidget(Widget* pWidget)
	{
		assert(false && "You must use AddTopPanel or AddBottomPanel in a SplitHorizontal widget.");
	}

	void SplitHorizontal::AddTopPanel(Widget* pWidget)
	{
		m_pTopContainer->AddWidget(pWidget);
	}

	void SplitHorizontal::AddBottomPanel(Widget* pWidget)
	{
		m_pBottomContainer->AddWidget(pWidget);
	}

	void SplitHorizontal::SetTopPanelHeight(int height)
	{
		Core::UInt2 size = m_pTopContainer->GetSize();
		size.y = height;
		m_pTopContainer->SetSize(size);
	}
}
