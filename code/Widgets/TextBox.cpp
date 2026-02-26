/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/TextBox.h"

#include "OsWin/Clipboard.h"
#include "OsWin/Input.h"
#include "OsWin/VirtualKeyCode.h"

#include "Widgets/Events/BaseEvent.h"
#include "Widgets/Events/KeyboardEvent.h"
#include "Widgets/Icon.h"
#include "Widgets/WidgetMgr.h"

#include "Rendering/Font/FontMgr.h"
#include "Rendering/Mesh/MeshId.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/PipelineState/PipelineStateMgr.h"

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
		, m_textAlignment(Left)
		, m_textLocalPos()
	{
		m_hoverStyle.SetBackgroundColor(Color(0.12f, 0.12f, 0.12f));
		m_hoverStyle.SetBorderColor(Color(0.6f, 0.6f, 0.6f));
		m_hoverStyle.SetBorderSize(1);
		m_hoverStyle.SetShowBorder(true);

		m_readOnlyStyle.SetBorderColor(Color(46, 46, 46));
		m_readOnlyStyle.SetShowBorder(true);
		m_readOnlyStyle.SetBorderSize(1);
		m_readOnlyStyle.SetBackgroundColor(Color(31, 31, 31));

		m_editStyle = m_hoverStyle;

		m_pCursorIcon = new Icon(WidgetMgr::Get().GetIconTextureId(IconId::kIconCursor));
		m_pCursorIcon->SetX(4);
		m_pCursorIcon->SetY(2);
		m_pCursorIcon->SetSize(Core::UInt2(2, 16));
		m_pCursorIcon->Disable();
		AddWidget(m_pCursorIcon);

		OnFocusGained([this](const FocusEvent& ev) { OnGetFocusCallback(); });
		OnFocusLost([this](const FocusEvent& ev) { OnLoseFocusCallback(); });

		SetSize(Core::UInt2(100, 20));
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

	void TextBox::Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor)
	{
		TextBoxStyle* pCurrentStyle = &m_defaultStyle;
		if (m_currentState == HOVER)
			pCurrentStyle = &m_hoverStyle;
		else if (m_currentState == EDIT)
			pCurrentStyle = &m_editStyle;
		else if (m_currentState == READONLY)
			pCurrentStyle = &m_readOnlyStyle;

		DirectX::XMMATRIX wvp;
		ComputeWVPMatrix(windowSize, wvp);
		int valueShowBorder = pCurrentStyle->m_showBorder ? 1 : 0;
		float rect[2] = { (float)m_size.x, (float)m_size.y };

		{
			WidgetMgr& widgetMgr = WidgetMgr::Get();
			Rendering::RenderModule& render = Rendering::RenderModule::Get();
			Rendering::PipelineStateMgr& psoMgr = Rendering::PipelineStateMgr::Get();

			const Rendering::PipelineState* pPso = psoMgr.GetPipelineState(widgetMgr.GetBaseWidgetPsoId());
			render.BindMaterial(*pPso);

			render.SetConstantBuffer(0, sizeof(wvp), &wvp, 0);
			render.SetConstantBuffer(1, sizeof(pCurrentStyle->m_backgroundColor), &pCurrentStyle->m_backgroundColor, 0);
			render.SetConstantBuffer(2, sizeof(valueShowBorder), &valueShowBorder, 0);
			render.SetConstantBuffer(3, sizeof(pCurrentStyle->m_borderColor), &pCurrentStyle->m_borderColor, 0);
			render.SetConstantBuffer(4, sizeof(rect), &rect, 0);
			render.SetConstantBuffer(5, sizeof(pCurrentStyle->m_borderSize), &pCurrentStyle->m_borderSize, 0);

			const Rendering::Mesh* pMesh = Rendering::MeshMgr::Get().GetMesh(widgetMgr.m_quadMeshId);
			render.RenderMesh(*pMesh);
		}

		{
			DrawText(scissor);
		}

		Widgets::Widget::Draw(windowSize, scissor);	
	}

	void TextBox::ResizeChildren()
	{
		m_pCursorIcon->Resize(m_absPos, m_size);
	}

	bool TextBox::Handle(const BaseEvent& ev)
	{
		if (m_currentState == READONLY)
			return true;

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
				else if (keyboardEvent.m_virtualKey == Os::VKeyCodes::Vk_V && Os::IsKeyDown(Os::VKeyCodes::LeftControl)) // ctrl+v
				{
					std::string textToCopy;
					bool res = Os::GetClipboardAsText(textToCopy);
					if (!res)
						return true;

					m_text.insert(m_cursorPosition, textToCopy.c_str(), textToCopy.size());
					m_cursorPosition += static_cast<int>(textToCopy.size());
					ComputeCursorPosition();
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
				ComputeTextLocalPosition();
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
		ComputeTextLocalPosition();
	}

	void TextBox::SetTextAlignment(TextAlignment alignment)
	{
		if (m_textAlignment != alignment)
		{
			m_textAlignment = alignment;
			ComputeTextLocalPosition();
		}
	}

	void TextBox::SetReadOnly(bool readOnly)
	{
		if (readOnly)
			m_currentState = READONLY;
		else
			m_currentState = DEFAULT;
	}

	void TextBox::Enable(bool recursive)
	{
		m_enabled = true;
	}

	const std::string& TextBox::GetText() const
	{
		return m_text;
	}

	bool TextBox::OnGetFocusCallback()
	{
		if (m_currentState == READONLY)
			return true;

		m_pCursorIcon->Enable(); 
		m_currentState = EDIT; 
		m_cursorPosition = 0;
		ComputeCursorPosition();
		return true;
	}

	bool TextBox::OnLoseFocusCallback()
	{
		if (m_currentState == READONLY)
			return true;

		m_pCursorIcon->Disable(); 
		m_currentState = DEFAULT;
		m_cursorLastBlinkChange = 0;

		return true;
	}

	void TextBox::ComputeCursorPosition()
	{
		DirectX::XMINT2 cursorOffset(-1, 2);
		
		Rendering::FontId fid = WidgetMgr::Get().GetUIFontId();
		const Rendering::Font* pFont = Rendering::FontMgr::Get().GetFont(fid);

		std::string textToCursor = m_text.substr(0, m_cursorPosition);
		DirectX::XMUINT2 textSize;
		pFont->ComputeRect(textToCursor, textSize);

		int xPosition = cursorOffset.x + m_textLocalPos.x + textSize.x;

		m_pCursorIcon->SetX(xPosition);
		m_pCursorIcon->Resize(m_absPos, m_size);
	}

	void TextBox::ComputeTextLocalPosition()
	{
		const int textXOffset = 5;
		const int textYOffset = 0;

		switch (m_textAlignment)
		{
		case Left:
		{
			m_textLocalPos.x = textXOffset;
			m_textLocalPos.y = textYOffset;
		}
		break;

		case Center:
		{
			m_textLocalPos.y = textYOffset;

			DirectX::XMUINT2 textRect;
			const Rendering::Font* pFont = Rendering::FontMgr::Get().GetFont(WidgetMgr::Get().GetUIFontId());
			pFont->ComputeRect(m_text, textRect);

			float halfWidth = static_cast<float>(m_size.x * 0.5f);
			float halfTextWidth = static_cast<float>(textRect.x * 0.5f);
			m_textLocalPos.x = static_cast<uint32_t>(halfWidth - halfTextWidth);
		}
		break;

		case Right:
		{
			m_textLocalPos.y = textYOffset;

			DirectX::XMUINT2 textRect;
			const Rendering::Font* pFont = Rendering::FontMgr::Get().GetFont(WidgetMgr::Get().GetUIFontId());
			pFont->ComputeRect(m_text, textRect);

			m_textLocalPos.x = m_size.x - textRect.x;
		}
		break;

		}
	}

	void TextBox::DrawText(const D3D12_RECT& scissor)
	{
		float fontScale = 1.f;
		DirectX::XMFLOAT3 uiPos((float)m_absPos.x + m_textLocalPos.x, (float)m_absPos.y + m_textLocalPos.y, (float)m_absPos.z - 1);
		DirectX::XMUINT4 localScissor(m_absPos.x, m_absPos.y, m_size.x, m_size.y);

		int32_t bottom = localScissor.y + localScissor.w;
		if (bottom <= scissor.bottom)
			Rendering::RenderModule::Get().PrepareRenderText(m_text, WidgetMgr::Get().GetUIFontId(), uiPos, DirectX::XMFLOAT2(fontScale, fontScale), localScissor, Widget::NEAR_CAMERA_PLANE, Widget::FAR_CAMERA_PLANE);
	}
}
