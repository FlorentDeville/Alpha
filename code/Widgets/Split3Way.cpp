/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Split3Way.h"

#include "Widgets/WidgetMgr.h"

extern LPCWSTR g_pIconName;

namespace Widgets
{
	Split3Way::Split3Way()
		: Widget()
		, m_pLeftContainer(nullptr)
		, m_pMiddleContainer(nullptr)
		, m_pRightContainer(nullptr)
		, m_pLeftSplit(nullptr)
		, m_pRightSplit(nullptr)
		, m_pLayout(nullptr)
		, m_leftDragged(false)
		, m_rightDragged(false)
	{
		m_pLayout = new Layout(0, 0, 0, 0);
		m_pLayout->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
		m_pLayout->SetDirection(Layout::Direction::Horizontal);
		AddWidget(m_pLayout);

		m_pLeftContainer = new Container(200, 0);
		m_pLeftContainer->SetSizeStyle(Widget::VSIZE_STRETCH);
		m_pLayout->AddWidget(m_pLeftContainer);

		m_pLeftSplit = new Split();
		m_pLeftSplit->SetSizeStyle(Widget::VSIZE_STRETCH);
		m_pLayout->AddWidget(m_pLeftSplit);

		m_pMiddleContainer = new Container(600, 0);
		m_pMiddleContainer->SetSizeStyle(Widget::VSIZE_STRETCH);
		m_pLayout->AddWidget(m_pMiddleContainer);

		m_pRightSplit = new Split();
		m_pRightSplit->SetSizeStyle(Widget::VSIZE_STRETCH);
		m_pLayout->AddWidget(m_pRightSplit);

		m_pRightContainer = new Container(200, 0);
		m_pRightContainer->SetSizeStyle(Widget::VSIZE_STRETCH | Widget::HSIZE_STRETCH);
		m_pLayout->AddWidget(m_pRightContainer);
	}

	Split3Way::~Split3Way()
	{}

	void Split3Way::Update()
	{
		const int MIN_SIZE = 50;

		if (m_pLeftSplit->IsDragged())
		{
			DirectX::XMINT2 currentMousePosition = WidgetMgr::Get().GetCursorPosition();

			DirectX::XMINT2 dt;
			dt.x = currentMousePosition.x - m_pLeftSplit->GetPreviousCursorPosition().x;
			dt.y = currentMousePosition.y - m_pLeftSplit->GetPreviousCursorPosition().y;

			DirectX::XMUINT2 leftContainerSize = m_pLeftContainer->GetSize();

			//prevent overflow cause leftContainerSize is unsigned
			if (-dt.x > (int)leftContainerSize.x)
				return;

			leftContainerSize.x += dt.x;
			leftContainerSize.y += dt.y;

			if (leftContainerSize.x < MIN_SIZE)
				return;

			DirectX::XMUINT2 middleContainerSize = m_pMiddleContainer->GetSize();

			//prevent overflow cause leftContainerSize is unsigned
			if (dt.x > (int)middleContainerSize.x)
				return;

			middleContainerSize.x -= dt.x;
			middleContainerSize.y -= dt.y;

			if (middleContainerSize.x < MIN_SIZE)
				return;

			m_pLeftContainer->SetSize(leftContainerSize);
			m_pMiddleContainer->SetSize(middleContainerSize);

			WidgetMgr::Get().Resize();

			m_pLeftSplit->SetPreviousCursorPosition(currentMousePosition);
		}

		if (m_pRightSplit->IsDragged())
		{
			DirectX::XMINT2 currentMousePosition = WidgetMgr::Get().GetCursorPosition();

			DirectX::XMINT2 dt;
			dt.x = currentMousePosition.x - m_pRightSplit->GetPreviousCursorPosition().x;
			dt.y = currentMousePosition.y - m_pRightSplit->GetPreviousCursorPosition().y;

			DirectX::XMUINT2 middleContainerSize = m_pMiddleContainer->GetSize();
			
			//prevent overflow cause middleContainerSize is unsigned
			if (-dt.x > (int)middleContainerSize.x)
				return;

			middleContainerSize.x += dt.x;
			middleContainerSize.y += dt.y;

			if (middleContainerSize.x < MIN_SIZE)
				return;

			DirectX::XMUINT2 rightContainerSize = m_pRightContainer->GetSize();

			//prevent overflow cause rightContainerSize is unsigned
			if (dt.x > (int)rightContainerSize.x)
				return;

			rightContainerSize.x -= dt.x;
			if (rightContainerSize.x < MIN_SIZE)
				return;

			m_pMiddleContainer->SetSize(middleContainerSize);

			WidgetMgr::Get().Resize();

			m_pRightSplit->SetPreviousCursorPosition(currentMousePosition);
		}
	}

	void Split3Way::Draw(const DirectX::XMFLOAT2& windowSize)
	{
		Widget::Draw(windowSize);
	}

	void Split3Way::Resize(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize)
	{
		const DirectX::XMUINT2 oldSize = m_size;

		float leftContainerRatio = 1;
		float middleContainerRatio = 1;
		if (oldSize.x != 0)
		{
			leftContainerRatio = m_pLeftContainer->GetSize().x / static_cast<float>(oldSize.x);
			middleContainerRatio = m_pMiddleContainer->GetSize().x / static_cast<float>(oldSize.x);
		}
		else
		{
			leftContainerRatio = m_pLeftContainer->GetSize().x / static_cast<float>(parentSize.x);
			middleContainerRatio = m_pMiddleContainer->GetSize().x / static_cast<float>(parentSize.x);
		}

		ReComputeSize(parentSize);
		ReComputePosition(parentAbsPos, parentSize);

		DirectX::XMUINT2 leftcontainerNewSize = m_pLeftContainer->GetSize();
		leftcontainerNewSize.x = static_cast<uint32_t>(leftContainerRatio * m_size.x);
		m_pLeftContainer->SetSize(leftcontainerNewSize);

		DirectX::XMUINT2 middlecontainerNewSize = m_pMiddleContainer->GetSize();
		middlecontainerNewSize.x = static_cast<uint32_t>(middleContainerRatio * m_size.x);
		m_pMiddleContainer->SetSize(middlecontainerNewSize);

		for (Widget* pChild : m_children)
			pChild->Resize(m_absPos, m_size);

	}

	void Split3Way::AddLeftPanel(Widget* pWidget)
	{
		m_pLeftContainer->AddWidget(pWidget);
	}

	void Split3Way::AddMiddlePanel(Widget* pWidget)
	{
		m_pMiddleContainer->AddWidget(pWidget);
	}

	void Split3Way::AddRightPanel(Widget* pWidget)
	{
		m_pRightContainer->AddWidget(pWidget);
	}
}
