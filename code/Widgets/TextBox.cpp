/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/TextBox.h"

#include "OsWin/VirtualKeyCode.h"

#include "Widgets/Events/BaseEvent.h"
#include "Widgets/Events/KeyboardEvent.h"
#include "Widgets/Icon.h"
#include "Widgets/Message.h"
#include "Widgets/WidgetMgr.h"

#include "Rendering/Font/FontMgr.h"
#include "Rendering/Material/MaterialId.h"
#include "Rendering/Material/MaterialMgr.h"
#include "Rendering/Mesh/MeshId.h"
#include "Rendering/Mesh/MeshMgr.h"

namespace Widgets
{
	TextBox::TextBox()
		: Widget()
		, m_defaultStyle()
		, m_editStyle()
		, m_hoverStyle()
		, m_currentState(DEFAULT)
		, m_cursorPosition(0)
		, m_cursorLastBlinkChange(0)
		, m_text()
	{
		m_hoverStyle.SetBackgroundColor(DirectX::XMVectorSet(0.12f, 0.12f, 0.12f, 1));
		m_hoverStyle.SetBorderColor(DirectX::XMVectorSet(0.6f, 0.6f, 0.6f, 1));
		m_hoverStyle.SetBorderSize(1);
		m_hoverStyle.SetShowBorder(true);
		m_hoverStyle.SetTextColor(DirectX::XMVectorSet(0.98f, 0.98f, 0.98f, 1));

		m_editStyle = m_hoverStyle;

		m_pCursorIcon = new Icon(WidgetMgr::Get().GetIconTextureId(IconId::kIconCursor));
		m_pCursorIcon->SetX(4);
		m_pCursorIcon->SetY(2);
		m_pCursorIcon->SetSize(DirectX::XMUINT2(2, 16));
		m_pCursorIcon->Disable();
		AddWidget(m_pCursorIcon);

		OnFocusGained([this](const FocusEvent& ev) { OnGetFocusCallback(); });
		OnFocusLost([this](const FocusEvent& ev) { OnLoseFocusCallback(); });

		SetSize(DirectX::XMUINT2(0, 20));
		SetSizeStyle(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_DEFAULT);
	}

	TextBox::~TextBox()
	{}

	void TextBox::Update(uint64_t dt)
	{
		//make the cursor blink here
		if (m_currentState == EDIT)
		{
			const uint64_t PERIOD = 500;
			m_cursorLastBlinkChange += dt;
			if (m_cursorLastBlinkChange >= PERIOD)
			{
				if (m_pCursorIcon->IsEnabled())
					m_pCursorIcon->Disable();
				else
					m_pCursorIcon->Enable();

				m_cursorLastBlinkChange = 0;
			}
		}
	}

	void TextBox::Draw(const DirectX::XMFLOAT2& windowSize)
	{
		TextBoxStyle* pCurrentStyle = &m_defaultStyle;
		if (m_currentState == HOVER)
			pCurrentStyle = &m_hoverStyle;
		else if (m_currentState == EDIT)
			pCurrentStyle = &m_editStyle;

		DirectX::XMMATRIX wvp;
		ComputeWVPMatrix(windowSize, wvp);
		int valueShowBorder = pCurrentStyle->m_showBorder ? 1 : 0;
		float rect[2] = { (float)m_size.x, (float)m_size.y };

		{
			WidgetMgr& widgetMgr = WidgetMgr::Get();
			Rendering::RenderModule& render = Rendering::RenderModule::Get();
			Rendering::MaterialMgr& materialMgr = Rendering::MaterialMgr::Get();

			const Rendering::Material* pMaterial = materialMgr.GetMaterial(widgetMgr.m_materialId);
			render.BindMaterial(*pMaterial, wvp);

			render.SetConstantBuffer(1, sizeof(pCurrentStyle->m_backgroundColor), &pCurrentStyle->m_backgroundColor, 0);
			render.SetConstantBuffer(2, sizeof(valueShowBorder), &valueShowBorder, 0);
			render.SetConstantBuffer(3, sizeof(pCurrentStyle->m_borderColor), &pCurrentStyle->m_borderColor, 0);
			render.SetConstantBuffer(4, sizeof(rect), &rect, 0);
			render.SetConstantBuffer(5, sizeof(pCurrentStyle->m_borderSize), &pCurrentStyle->m_borderSize, 0);

			const Rendering::Mesh* pMesh = Rendering::MeshMgr::Get().GetMesh(widgetMgr.m_quadMeshId);
			render.RenderMesh(*pMesh);
		}

		{
			float fontScale = 1.f;
			const int textXOffset = 5;
			const int textYOffset = 0;
			DirectX::XMFLOAT3 uiPos((float)m_absPos.x + textXOffset, (float)m_absPos.y + textYOffset, (float)m_absPos.z - 1);
			DirectX::XMUINT4 scissor(m_absPos.x + textXOffset, m_absPos.y, m_size.x - (2 * textXOffset), m_size.y);
			Rendering::RenderModule::Get().PrepareRenderText(m_text, WidgetMgr::Get().GetUIFontId(), uiPos, DirectX::XMFLOAT2(fontScale, fontScale), scissor, Widget::NEAR_CAMERA_PLANE, Widget::FAR_CAMERA_PLANE);
		}

		Widgets::Widget::Draw(windowSize);	
	}

	void TextBox::ResizeChildren()
	{
		m_pCursorIcon->Resize(m_absPos, m_size);
	}

	void TextBox::Enable(bool recursive)
	{
		m_enabled = true;
	}

	bool TextBox::Handle(const BaseEvent& ev)
	{
		switch (ev.m_id)
		{
		case EventType::kMouseEnter: //M_MouseEnter:
		{
			if(m_currentState == DEFAULT)
				m_currentState = HOVER;
			return true;
		}
		break;

		case EventType::kMouseExit://M_MouseExit:
		{
			if(m_currentState == HOVER)
				m_currentState = DEFAULT;
			return true;
		}
		break;

		case EventType::kVKeyDown: //M_VirtualKeyDown:
		{
			if (m_currentState == EDIT)
			{
				const KeyboardEvent& keyboardEvent = static_cast<const KeyboardEvent&>(ev);
				if (keyboardEvent.m_virtualKey == Os::VKeyCodes::Right) //right
				{
					if (m_cursorPosition < m_text.size())
					{
						++m_cursorPosition;
						ComputeCursorPosition();
					}
					return true;
				}
				else if (keyboardEvent.m_virtualKey == Os::VKeyCodes::Left) // left
				{
					if (m_cursorPosition > 0)
					{
						--m_cursorPosition;
						ComputeCursorPosition();
					}
					return true;
				}
				else if (keyboardEvent.m_virtualKey == Os::VKeyCodes::Back) // backspace
				{
					if (m_cursorPosition > 0)
					{
						m_text.erase(m_cursorPosition - 1, 1);
						--m_cursorPosition;
						ComputeCursorPosition();
					}
				}
				else if (keyboardEvent.m_virtualKey == Os::VKeyCodes::Delete)
				{
					if (m_cursorPosition < static_cast<int>(m_text.size()))
					{
						m_text.erase(m_cursorPosition, 1);
					}
				}
				else if (keyboardEvent.m_virtualKey == Os::VKeyCodes::Return) // enter
				{
					m_onValidate(m_text);
				}
				else if (keyboardEvent.m_virtualKey == Os::VKeyCodes::Esc)
				{
					m_onEscape();
				}
				else if (keyboardEvent.m_virtualKey == Os::VKeyCodes::Home)
				{
					if (m_cursorPosition != 0)
					{
						m_cursorPosition = 0;
						ComputeCursorPosition();
					}
				}
				else if (keyboardEvent.m_virtualKey == Os::VKeyCodes::End)
				{
					if (m_cursorPosition != static_cast<int>(m_text.size()))
					{
						m_cursorPosition = static_cast<int>(m_text.size());
						ComputeCursorPosition();
					}
				}
			}
			return true;
		}
		break;

		case EventType::kCharKeyDown: //M_CharKeyDown:
		{
			if(m_currentState == EDIT)
			{
				const KeyboardEvent& keyboardEvent = static_cast<const KeyboardEvent&>(ev);
				char newChar = static_cast<char>(keyboardEvent.m_virtualKey);
				if (newChar == '\b' || newChar == '\t' || newChar == '\r' || newChar == '\n')
					return true;

				m_text.insert(m_cursorPosition, 1, newChar);
				++m_cursorPosition;
				ComputeCursorPosition();
				return true;
			}		
		}
		break;

		default:
			return Widget::Handle(ev);
			break;
		}

		return false;
	}

	void TextBox::SetText(const std::string& text)
	{
		m_text = text;
	}

	//void TextBox::OnValidate(const OnValidateCallback& callback)
	//{
	//	m_onValidateCallback = callback;
	//}

	const std::string& TextBox::GetText() const
	{
		return m_text;
	}

	bool TextBox::OnGetFocusCallback()
	{
		m_pCursorIcon->Enable(); 
		m_currentState = EDIT; 
		m_cursorPosition = 0;
		ComputeCursorPosition();
		return true;
	}

	bool TextBox::OnLoseFocusCallback()
	{
		m_pCursorIcon->Disable(); 
		m_currentState = DEFAULT;
		m_cursorLastBlinkChange = 0;

		return true;
	}

	void TextBox::ComputeCursorPosition()
	{
		DirectX::XMINT2 cursorOffset(4, 2);
		Rendering::FontId fid = WidgetMgr::Get().GetUIFontId();
		const Rendering::Font* pFont = Rendering::FontMgr::Get().GetFont(fid);

		std::string textToCursor = m_text.substr(0, m_cursorPosition);
		DirectX::XMUINT2 textSize;
		pFont->ComputeRect(textToCursor, textSize);

		int xPosition = cursorOffset.x + textSize.x;

		m_pCursorIcon->SetX(cursorOffset.x + textSize.x);
		m_pCursorIcon->Resize(m_absPos, m_size);
	}
}
