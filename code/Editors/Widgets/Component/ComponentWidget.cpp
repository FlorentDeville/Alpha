/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/Component/ComponentWidget.h"

#include "Editors/Widgets/AssetId/AssetIdWidget.h"
#include "Editors/Widgets/Component/ComponentModel.h"
#include "Editors/Widgets/Matrix/MatrixWidget.h"
#include "Editors/Property.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/WidgetMgr.h"

namespace Editors
{
	ComponentWidget::ComponentWidget()
		: Widget()
		, m_pModel(nullptr)
		, m_isDirtyWidget(true)
	{
		SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_FIT);
	}

	ComponentWidget::~ComponentWidget()
	{
		delete m_pModel;
	}

	void ComponentWidget::Update(uint64_t dt)
	{
		if (m_isDirtyWidget)
		{
			CreateWidgets();
			m_isDirtyWidget = false;
		}
	}

	void ComponentWidget::SetModel(ComponentModel* pModel)
	{
		if (m_pModel == pModel)
			return;

		delete m_pModel;
		m_pModel = pModel;
	}

	void ComponentWidget::CreateWidgets()
	{
		Widgets::Layout* pGridLayout = new Widgets::Layout();
		pGridLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_FIT);
		pGridLayout->SetDirection(Widgets::Layout::Vertical);
		AddWidget(pGridLayout);

		const int FIELD_HEIGHT = 20;
		const int NAME_WIDTH = 100;
		const int propertyCount = m_pModel->GetPropertyCount();
		for (int ii = 0; ii < propertyCount; ++ii)
		{
			Widgets::Layout* pItemLayout = new Widgets::Layout(0, FIELD_HEIGHT, 0, 0);
			pItemLayout->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_FIT);
			pItemLayout->SetDirection(Widgets::Layout::Horizontal);
			pGridLayout->AddWidget(pItemLayout);

			//add property name
			Widgets::Label* pNameLabel = new Widgets::Label(0, 0, 1, m_pModel->GetPropertyName(ii));
			pNameLabel->SetSize(DirectX::XMUINT2(NAME_WIDTH, FIELD_HEIGHT));
			pNameLabel->SetSizeStyle(Widgets::Widget::DEFAULT);
			pItemLayout->AddWidget(pNameLabel);

			PropertyType type = m_pModel->GetPropertyType(ii);
			switch (type)
			{
			case PropertyType::kAssetMaterial:
			case PropertyType::kAssetMesh:
			{
				AssetIdWidget* pNewWidget = new AssetIdWidget();
				pNewWidget->SetModel(m_pModel->GetModel(ii));
				pNewWidget->SetSize(DirectX::XMUINT2(0, FIELD_HEIGHT));
				pNewWidget->SetSizeStyle(Widgets::Widget::HSIZE_STRETCH);
				pItemLayout->AddWidget(pNewWidget);
			}
			break;

			case PropertyType::kMat44f:
			{
				const Core::Mat44f& matrix = m_pModel->GetPropertyValue<Core::Mat44f>(ii);

				MatrixWidget* pNewWidget = new MatrixWidget();
				pNewWidget->SetValue(matrix);
				pNewWidget->OnValueChanged([this, ii](const Core::Mat44f& value) { m_pModel->SetPropertyValue(ii, value); });
				m_pModel->OnPropertyValueChanged([this, pNewWidget, ii](int row)
					{
						if (row != ii)
							return;

						const Core::Mat44f& matrix = m_pModel->GetPropertyValue<Core::Mat44f>(ii);
						pNewWidget->SetValue(matrix);
					});

				pItemLayout->AddWidget(pNewWidget);
			}
			break;

			default:
				assert(false);
				break;
			}
		}

		Widgets::WidgetMgr::Get().RequestResize();
	}
}
