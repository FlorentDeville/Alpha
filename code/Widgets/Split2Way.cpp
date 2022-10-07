/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Split2Way.h"

#include "Widgets/Container.h"
#include "Widgets/Layout.h"
#include "Widgets/Split.h"
#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	Split2Way::Split2Way()
		: Widget()
		, m_pLeftContainer(nullptr)
		, m_pRightContainer(nullptr)
		, m_pSplit(nullptr)
		, m_pLayout(nullptr)
		, m_splitDragged(false)
	{
		m_pLayout = new Layout(0, 0, 0, 0);
		m_pLayout->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
		m_pLayout->SetDirection(Layout::Direction::Horizontal);
		Widget::AddWidget(m_pLayout);

		m_pLeftContainer = new Container(200, 0);
		m_pLeftContainer->SetSizeStyle(Widget::VSIZE_STRETCH);
		m_pLayout->AddWidget(m_pLeftContainer);

		m_pSplit = new Split();
		m_pSplit->SetSizeStyle(Widget::VSIZE_STRETCH);
		m_pLayout->AddWidget(m_pSplit);

		m_pRightContainer = new Container(200, 0);
		m_pRightContainer->SetSizeStyle(Widget::VSIZE_STRETCH | Widget::HSIZE_STRETCH);
		m_pLayout->AddWidget(m_pRightContainer);
	}

	Split2Way::~Split2Way()
	{}

	void Split2Way::Update()
	{
		const int MIN_SIZE = 50;

		if (m_pSplit->IsDragged())
		{
			DirectX::XMINT2 currentMousePosition = WidgetMgr::Get().GetCursorPosition();

			DirectX::XMINT2 dt;
			dt.x = currentMousePosition.x - m_pSplit->GetPreviousCursorPosition().x;
			dt.y = currentMousePosition.y - m_pSplit->GetPreviousCursorPosition().y;

			DirectX::XMUINT2 leftContainerSize = m_pLeftContainer->GetSize();

			//prevent overflow cause leftContainerSize is unsigned
			if (-dt.x > (int)leftContainerSize.x)
				return;

			leftContainerSize.x += dt.x;
			leftContainerSize.y += dt.y;

			if (leftContainerSize.x < MIN_SIZE)
				return;

			DirectX::XMUINT2 rightContainerSize = m_pRightContainer->GetSize();

			//prevent overflow cause leftContainerSize is unsigned
			if (dt.x > (int)rightContainerSize.x)
				return;

			rightContainerSize.x -= dt.x;
			rightContainerSize.y -= dt.y;

			if (rightContainerSize.x < MIN_SIZE)
				return;

			m_pLeftContainer->SetSize(leftContainerSize);
			m_pRightContainer->SetSize(rightContainerSize);

			WidgetMgr::Get().Resize();

			m_pSplit->SetPreviousCursorPosition(currentMousePosition);
		}
	}

	void Split2Way::Draw(const DirectX::XMFLOAT2& windowSize)
	{
		Widget::Draw(windowSize);
	}

	void Split2Way::Resize(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize)
	{
		const DirectX::XMUINT2 oldSize = m_size;

		float leftContainerRatio = 1;
		float middleContainerRatio = 1;
		if (oldSize.x != 0)
		{
			leftContainerRatio = m_pLeftContainer->GetSize().x / static_cast<float>(oldSize.x);
			middleContainerRatio = m_pRightContainer->GetSize().x / static_cast<float>(oldSize.x);
		}
		else
		{
			leftContainerRatio = m_pLeftContainer->GetSize().x / static_cast<float>(parentSize.x);
			middleContainerRatio = m_pRightContainer->GetSize().x / static_cast<float>(parentSize.x);
		}

		ReComputeSize(parentSize);
		ReComputePosition(parentAbsPos, parentSize);

		DirectX::XMUINT2 leftcontainerNewSize = m_pLeftContainer->GetSize();
		leftcontainerNewSize.x = static_cast<uint32_t>(leftContainerRatio * m_size.x);
		m_pLeftContainer->SetSize(leftcontainerNewSize);

		/*DirectX::XMUINT2 middlecontainerNewSize = m_pMiddleContainer->GetSize();
		middlecontainerNewSize.x = static_cast<uint32_t>(middleContainerRatio * m_size.x);
		m_pMiddleContainer->SetSize(middlecontainerNewSize);*/

		for (Widget* pChild : m_children)
			pChild->Resize(m_absPos, m_size);

	}

	void Split2Way::AddWidget(Widget* pWidget)
	{
		assert(false && "You must use AddLeftPanel or AddRightPanel in a Split2Way widget.");
	}

	void Split2Way::AddLeftPanel(Widget* pWidget)
	{
		m_pLeftContainer->AddWidget(pWidget);
	}

	void Split2Way::AddRightPanel(Widget* pWidget)
	{
		m_pRightContainer->AddWidget(pWidget);
	}

	void Split2Way::SetLeftPanelWidth(int width)
	{
		DirectX::XMUINT2 size = m_pLeftContainer->GetSize();
		size.x = width;
		m_pLeftContainer->SetSize(size);
	}
}
