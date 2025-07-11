/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/SplitVertical.h"

#include "Widgets/Container.h"
#include "Widgets/Layout.h"
#include "Widgets/Split.h"
#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	SplitVertical::SplitVertical()
		: Widget()
		, m_pLeftContainer(nullptr)
		, m_pRightContainer(nullptr)
		, m_pSplit(nullptr)
		, m_pLayout(nullptr)
		, m_splitDragged(false)
		, m_resizePolicy(KeepRatio)
	{
		m_pLayout = new Layout(0, 0, 0, 0);
		m_pLayout->SetSizeStyle(Widget::HSIZE_STRETCH | Widget::VSIZE_STRETCH);
		m_pLayout->SetDirection(Layout::Direction::Horizontal);
		Widget::AddWidget(m_pLayout);

		m_pLeftContainer = new Container(200, 0);
		m_pLeftContainer->SetSizeStyle(Widget::VSIZE_STRETCH);
		m_pLayout->AddWidget(m_pLeftContainer);

		m_pSplit = new Split(true);
		m_pSplit->SetSizeStyle(Widget::VSIZE_STRETCH);
		m_pLayout->AddWidget(m_pSplit);

		m_pRightContainer = new Container(200, 0);
		m_pRightContainer->SetSizeStyle(Widget::VSIZE_STRETCH | Widget::HSIZE_STRETCH);
		m_pLayout->AddWidget(m_pRightContainer);
	}

	SplitVertical::~SplitVertical()
	{}

	void SplitVertical::Update(uint64_t dt)
	{
		const int MIN_SIZE = 50;

		if (m_pSplit->IsDragged())
		{
			DirectX::XMINT2 currentMousePosition = WidgetMgr::Get().GetCursorPosition();

			DirectX::XMINT2 dt;
			dt.x = currentMousePosition.x - m_pSplit->GetPreviousCursorPosition().x;
			dt.y = currentMousePosition.y - m_pSplit->GetPreviousCursorPosition().y;

			Core::UInt2 leftContainerSize = m_pLeftContainer->GetSize();

			//prevent overflow cause leftContainerSize is unsigned
			if (-dt.x > (int)leftContainerSize.x)
				return;

			leftContainerSize.x += dt.x;
			leftContainerSize.y += dt.y;

			if (leftContainerSize.x < MIN_SIZE)
				return;

			Core::UInt2 rightContainerSize = m_pRightContainer->GetSize();

			//prevent overflow cause leftContainerSize is unsigned
			if (dt.x > (int)rightContainerSize.x)
				return;

			rightContainerSize.x -= dt.x;
			rightContainerSize.y -= dt.y;

			if (rightContainerSize.x < MIN_SIZE)
				return;

			m_pLeftContainer->SetSize(leftContainerSize);
			m_pRightContainer->SetSize(rightContainerSize);

			WidgetMgr::Get().RequestResize();

			m_pSplit->SetPreviousCursorPosition(currentMousePosition);
		}
	}

	void SplitVertical::Draw(const DirectX::XMFLOAT2& windowSize, const D3D12_RECT& scissor)
	{
		Widget::Draw(windowSize, scissor);
	}

	void SplitVertical::Resize(const Core::Int3& parentAbsPos, const Core::UInt2& parentSize)
	{
		const Core::UInt2 oldSize = m_size;
		ReComputeSize(parentSize);
		ReComputePosition(parentAbsPos, parentSize);

		//old ratio
		switch (m_resizePolicy)
		{
		case KeepRatio:
		{
			float leftContainerRatio = 1;
			if (oldSize.x != 0)
				leftContainerRatio = m_pLeftContainer->GetSize().x / static_cast<float>(oldSize.x);
			else
				leftContainerRatio = m_pLeftContainer->GetSize().x / static_cast<float>(parentSize.x);

			Core::UInt2 leftcontainerNewSize = m_pLeftContainer->GetSize();
			leftcontainerNewSize.x = static_cast<uint32_t>(leftContainerRatio * m_size.x);
			m_pLeftContainer->SetSize(leftcontainerNewSize);
		}
		break;

		case KeepLeftSize:
		{
			//nothig to do
		}
		break;

		case KeepRightSize:
		{
			Core::UInt2 oldRightContainerSize = m_pRightContainer->GetSize();
			int leftContainerWidth = m_size.x - oldRightContainerSize.x - m_pSplit->GetWidth();
			m_pLeftContainer->SetSize(Core::UInt2(leftContainerWidth, 0));
		}
		break;
		}

		for (Widget* pChild : m_children)
			pChild->Resize(m_absPos, m_size);

	}

	void SplitVertical::AddWidget(Widget* pWidget)
	{
		assert(false && "You must use AddLeftPanel or AddRightPanel in a Split2Way widget.");
	}

	void SplitVertical::AddLeftPanel(Widget* pWidget)
	{
		m_pLeftContainer->AddWidget(pWidget);
	}

	void SplitVertical::AddRightPanel(Widget* pWidget)
	{
		m_pRightContainer->AddWidget(pWidget);
	}

	void SplitVertical::SetLeftPanelWidth(int width)
	{
		Core::UInt2 size = m_pLeftContainer->GetSize();
		size.x = width;
		m_pLeftContainer->SetSize(size);
	}

	void SplitVertical::SetRightPanelWidth(int width)
	{
		Core::UInt2 size = m_pRightContainer->GetSize();
		size.x = width;
		m_pRightContainer->SetSize(size);
	}

	void SplitVertical::SetResizePolicy(ResizePolicy policy)
	{
		m_resizePolicy = policy;
	}
}
