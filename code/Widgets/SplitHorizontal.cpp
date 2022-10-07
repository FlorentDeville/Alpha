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
		m_pLayout->AddWidget(m_pSplit);

		m_pBottomContainer = new Container(0, 200);
		m_pBottomContainer->SetSizeStyle(Widget::VSIZE_STRETCH | Widget::HSIZE_STRETCH);
		m_pLayout->AddWidget(m_pBottomContainer);
	}

	SplitHorizontal::~SplitHorizontal()
	{}

	void SplitHorizontal::Update()
	{
		const int MIN_SIZE = 50;

		if (m_pSplit->IsDragged())
		{
			DirectX::XMINT2 currentMousePosition = WidgetMgr::Get().GetCursorPosition();

			DirectX::XMINT2 dt;
			dt.x = currentMousePosition.x - m_pSplit->GetPreviousCursorPosition().x;
			dt.y = currentMousePosition.y - m_pSplit->GetPreviousCursorPosition().y;

			DirectX::XMUINT2 topContainerSize = m_pTopContainer->GetSize();

			//prevent overflow cause topContainerSize is unsigned
			if (-dt.y > (int)topContainerSize.y)
				return;

			topContainerSize.x += dt.x;
			topContainerSize.y += dt.y;

			if (topContainerSize.x < MIN_SIZE)
				return;

			DirectX::XMUINT2 bottomContainerSize = m_pBottomContainer->GetSize();

			//prevent overflow cause leftContainerSize is unsigned
			if (dt.y > (int)bottomContainerSize.y)
				return;

			bottomContainerSize.x -= dt.x;
			bottomContainerSize.y -= dt.y;

			if (bottomContainerSize.y < MIN_SIZE)
				return;

			m_pTopContainer->SetSize(topContainerSize);
			//m_pBottomContainer->SetSize(rightContainerSize);

			WidgetMgr::Get().Resize();

			m_pSplit->SetPreviousCursorPosition(currentMousePosition);
		}
	}

	void SplitHorizontal::Draw(const DirectX::XMFLOAT2& windowSize)
	{
		Widget::Draw(windowSize);
	}

	void SplitHorizontal::Resize(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize)
	{
		/*const DirectX::XMUINT2 oldSize = m_size;

		float topContainerRatio = 1;
		float bottomContainerRatio = 1;
		if (oldSize.y != 0)
		{
			leftContainerRatio = m_pLeftContainer->GetSize().x / static_cast<float>(oldSize.x);
			middleContainerRatio = m_pRightContainer->GetSize().x / static_cast<float>(oldSize.x);
		}
		else
		{
			leftContainerRatio = m_pLeftContainer->GetSize().x / static_cast<float>(parentSize.x);
			middleContainerRatio = m_pRightContainer->GetSize().x / static_cast<float>(parentSize.x);
		}*/

		ReComputeSize(parentSize);
		ReComputePosition(parentAbsPos, parentSize);

		/*DirectX::XMUINT2 leftcontainerNewSize = m_pLeftContainer->GetSize();
		leftcontainerNewSize.x = static_cast<uint32_t>(leftContainerRatio * m_size.x);
		m_pLeftContainer->SetSize(leftcontainerNewSize);*/

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
		DirectX::XMUINT2 size = m_pTopContainer->GetSize();
		size.y = height;
		m_pTopContainer->SetSize(size);
	}
}
