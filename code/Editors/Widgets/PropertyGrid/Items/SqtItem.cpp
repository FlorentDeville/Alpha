/********************************************************************************/
/* Copyright (C) 2026 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#include "Editors/Widgets/PropertyGrid/Items/SqtItem.h"

#include "Core/Math/Constants.h"
#include "Core/Math/Sqt.h"

#include "Editors/Widgets/Utils.h"
#include "Editors/ObjectWatcher/ObjectWatcher.h"

#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/TextBox.h"

#include <charconv>

namespace Editors
{
	SqtItem::SqtItem(Systems::Object* pObj, const Core::FieldDescriptor* pField, uint32_t index)
		: PropertyGridItem(pObj, pField, index)
		, m_pTranslationTextbox()
		, m_pRotationTextbox()
		, m_pScaleTextbox()
	{ }

	SqtItem::~SqtItem()
	{ }

	void SqtItem::CreateWidgets()
	{
		Widgets::Layout* pLayout = new Widgets::Layout();
		pLayout->SetDirection(Widgets::Layout::Direction::Vertical);
		pLayout->SetSizeStyle(Widgets::Widget::HSIZE_DEFAULT | Widgets::Widget::VSIZE_FIT);
		pLayout->GetDefaultStyle().SetBorderSize(1);
		pLayout->GetDefaultStyle().SetBorderColor(Widgets::Color(255, 0, 0, 255));
		pLayout->GetDefaultStyle().ShowBorder(true);

		const uint32_t TEXTBOX_WIDTH = 50;
		//T
		{
			Widgets::Layout* pRowLayout = new Widgets::Layout(Widgets::Layout::Direction::Horizontal, Widgets::Widget::FIT);
			pLayout->AddWidget(pRowLayout);

			Widgets::Label* pLabel = new Widgets::Label("T ");
			pLabel->SetSizeStyle(Widgets::Widget::DEFAULT);
			pLabel->SetWidth(30);
			pRowLayout->AddWidget(pLabel);

			for (int ii = 0; ii < 3; ++ii)
			{
				m_pTranslationTextbox[ii] = new Widgets::TextBox();
				m_pTranslationTextbox[ii]->SetSizeStyle(Widgets::Widget::DEFAULT);
				m_pTranslationTextbox[ii]->SetReadOnly(m_pField->IsReadOnly());
				m_pTranslationTextbox[ii]->SetWidth(TEXTBOX_WIDTH);
				m_pTranslationTextbox[ii]->OnValidate([this, ii](const std::string& value)
					{
						float newValue = 0;
						std::from_chars(value.c_str(), value.c_str() + value.size(), newValue);

						const Core::Sqt* pValue = GetDataPtr<Core::Sqt>();
						Core::Sqt copy = *pValue;
						Core::Vec4f t = pValue->GetTranslation();
						t.Set(ii, newValue);
						copy.SetTranslation(t);

						ObjectWatcher::Get().ModifyField(m_pObj, m_pField, ObjectWatcher::SET_FIELD, m_index, &copy);
					});
				pRowLayout->AddWidget(m_pTranslationTextbox[ii]);
			}
		}

		//Q
		{
			Widgets::Layout* pRowLayout = new Widgets::Layout(Widgets::Layout::Direction::Horizontal, Widgets::Widget::FIT);
			pLayout->AddWidget(pRowLayout);

			Widgets::Label* pLabel = new Widgets::Label("R ");
			pLabel->SetSizeStyle(Widgets::Widget::DEFAULT);
			pLabel->SetWidth(30);
			pRowLayout->AddWidget(pLabel);

			for (int ii = 0; ii < 3; ++ii)
			{
				m_pRotationTextbox[ii] = new Widgets::TextBox();
				m_pRotationTextbox[ii]->SetSizeStyle(Widgets::Widget::DEFAULT);
				m_pRotationTextbox[ii]->SetReadOnly(m_pField->IsReadOnly());
				m_pRotationTextbox[ii]->SetWidth(TEXTBOX_WIDTH);
				m_pRotationTextbox[ii]->OnValidate([this, ii](const std::string& value)
					{
						float newValue = 0;
						std::from_chars(value.c_str(), value.c_str() + value.size(), newValue);
						
						while (newValue >= 360)
							newValue = newValue - 360;

						while (newValue <= -360)
							newValue = newValue + 360;

						float radian = newValue * Core::PI_OVER_180;

						const Core::Sqt* pValue = GetDataPtr<Core::Sqt>();
						Core::Sqt copy = *pValue;
						Core::Vec4f eulerAngles = pValue->GetRotationQuaternion().ToEulerAngles();
						eulerAngles.Set(ii, radian);
						copy.SetRotationQuaternion(Core::Quaternion::FromEulerAngles(eulerAngles));

						ObjectWatcher::Get().ModifyField(m_pObj, m_pField, ObjectWatcher::SET_FIELD, m_index, &copy);
					});
				pRowLayout->AddWidget(m_pRotationTextbox[ii]);
			}
		}

		//S
		{
			Widgets::Layout* pRowLayout = new Widgets::Layout(Widgets::Layout::Direction::Horizontal, Widgets::Widget::FIT);
			pLayout->AddWidget(pRowLayout);

			Widgets::Label* pLabel = new Widgets::Label("S ");
			pLabel->SetSizeStyle(Widgets::Widget::DEFAULT);
			pLabel->SetWidth(30);
			pRowLayout->AddWidget(pLabel);

			for (int ii = 0; ii < 3; ++ii)
			{
				m_pScaleTextbox[ii] = new Widgets::TextBox();
				m_pScaleTextbox[ii]->SetSizeStyle(Widgets::Widget::DEFAULT);
				m_pScaleTextbox[ii]->SetReadOnly(m_pField->IsReadOnly());
				m_pScaleTextbox[ii]->OnValidate([this, ii](const std::string& value)
					{
						float newValue = 0;
						std::from_chars(value.c_str(), value.c_str() + value.size(), newValue);

						const Core::Sqt* pValue = GetDataPtr<Core::Sqt>();
						Core::Sqt copy = *pValue;
						Core::Vec4f s = pValue->GetScale();
						s.Set(ii, newValue);
						copy.SetScale(s);

						ObjectWatcher::Get().ModifyField(m_pObj, m_pField, ObjectWatcher::SET_FIELD, m_index, &copy);
					});
				pRowLayout->AddWidget(m_pScaleTextbox[ii]);
			}
		}
	
		UpdateValue();

		m_pNameWidget = CreateDefaultItemLabel();
		m_pEditingWidget = pLayout;
	}

	void SqtItem::UpdateValue()
	{
		const Core::Sqt* pValue = GetDataPtr<const Core::Sqt>();

		const Core::Vec4f& t = pValue->GetTranslation();
		for (int ii = 0; ii < 3; ++ii)
		{
			const uint32_t BUFFER_SIZE = 16;
			char buffer[BUFFER_SIZE] = { '\0' };
			Utils::FToA(t.Get(ii), buffer, BUFFER_SIZE);

			m_pTranslationTextbox[ii]->SetText(buffer);
		}

		Core::Quaternion q = pValue->GetRotationQuaternion();
		q.Normalize();
		Core::Vec4f eulerAngles = q.ToEulerAngles();
		for (int ii = 0; ii < 3; ++ii)
		{
			const uint32_t BUFFER_SIZE = 16;
			char buffer[BUFFER_SIZE] = { '\0' };

			float degree = eulerAngles.Get(ii) * 180.f * Core::ONE_OVER_PI;
			Utils::FToA(degree, buffer, BUFFER_SIZE);

			m_pRotationTextbox[ii]->SetText(buffer);
		}

		const Core::Vec4f& s = pValue->GetScale();
		for (int ii = 0; ii < 3; ++ii)
		{
			const uint32_t BUFFER_SIZE = 16;
			char buffer[BUFFER_SIZE] = { '\0' };
			Utils::FToA(s.Get(ii), buffer, BUFFER_SIZE);

			m_pScaleTextbox[ii]->SetText(buffer);
		}
	}
}
