/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Widgets/Widgets/SliderFloat.h"

#include "Rendering/Font/FontMgr.h"
#include "Rendering/Material/MaterialMgr.h"
#include "Rendering/Mesh/MeshMgr.h"
#include "Rendering/RenderModule.h"

#include "Widgets/Container.h"
#include "Widgets/Label.h"
#include "Widgets/TextBox.h"
#include "Widgets/WidgetMgr.h"

namespace Widgets
{
	SliderFloat::SliderFloat(float minValue, float maxValue, float currentValue)
		: Widget(100, 20, 0, 0)
		, m_minValue(minValue)
		, m_maxValue(maxValue)
		, m_currentValue(currentValue)
		, m_isSliderDragging(false)
		, m_mouseDragPreviousX(0)
		, m_mode(Slider)
	{
		m_defaultStyle.SetBackgroundColor(Widgets::Color(0.22f, 0.22f, 0.22f));
		m_defaultStyle.SetBorderColor(Widgets::Color(0.26f, 0.26f, 0.26f));
		m_defaultStyle.SetShowBorder(true);
		m_defaultStyle.SetBorderSize(1);
		m_defaultStyle.SetSliderColor(Widgets::Color(120, 120, 120));

		m_hoverStyle.SetBackgroundColor(Widgets::Color(0.12f, 0.12f, 0.12f));
		m_hoverStyle.SetBorderColor(Widgets::Color(0.6f, 0.6f, 0.6f));
		m_hoverStyle.SetBorderSize(1);
		m_hoverStyle.SetShowBorder(true);

		m_hoverSliderStyle = m_hoverStyle;
		m_hoverSliderStyle.SetSliderColor(Widgets::Color(170, 170, 170));

		//first make the slider
		m_pSlider = new Container(10, 18);
		m_pSlider->SetPositionStyle(Widget::HPOSITION_STYLE::NONE, Widget::VPOSITION_STYLE::MIDDLE);
		m_pSlider->GetDefaultStyle().SetBackgroundColor(m_defaultStyle.m_sliderColor);
		m_pSlider->SetX(CalculateSliderLocalX());

		m_pSlider->OnMouseEnter([this](const Widgets::MouseEvent& ev) { Slider_OnMouseEnter(); });
		m_pSlider->OnMouseExit([this](const Widgets::MouseEvent& ev) { Slider_OnMouseExit(); });
		m_pSlider->OnMouseDown([this](const Widgets::MouseEvent& ev) { Slider_OnMouseDown(ev); });
		m_pSlider->OnMouseUp([this](const Widgets::MouseEvent& ev) { Slider_OnMouseUp(); });

		AddWidget(m_pSlider);

		m_pTextbox = new TextBox();
		m_pTextbox->Disable();
		m_pTextbox->SetText(std::to_string(m_currentValue));
		m_pTextbox->SetSize(Core::UInt2(100, 20));
		m_pTextbox->SetTextAlignment(TextBox::Center);
		m_pTextbox->OnValidate([this](const std::string& strValue) { TextBox_OnValidate(strValue); });
		m_pTextbox->OnEscape([this]() { TransitionTextToSlider(false); });
		m_pTextbox->OnFocusLost([this](const FocusEvent&) { TextBox_OnFocusLost(); });

		AddWidget(m_pTextbox);

		OnMouseDown([this](const MouseEvent& ev) 
			{ 
				TransitionSliderToText();
			});
	}

	SliderFloat::~SliderFloat()
	{

	}

	void SliderFloat::Update(uint64_t dt)
	{
		if (!m_isSliderDragging)
			return;

		float oldCurrentValue = m_currentValue;

		DirectX::XMINT2 currentMousePosition = WidgetMgr::Get().GetCursorPosition();
		int displacement = currentMousePosition.x - m_mouseDragPreviousX;
		float ratio = static_cast<float>(displacement) / static_cast<float>(m_size.x);
		float range = m_maxValue - m_minValue;
		float offset = range * ratio;
		
		m_currentValue += offset;
		if (m_currentValue > m_maxValue)
			m_currentValue = m_maxValue;
		else if (m_currentValue < m_minValue)
			m_currentValue = m_minValue;

		if (m_currentValue != oldCurrentValue)
		{
			m_pSlider->SetX(CalculateSliderLocalX());
			m_pSlider->ReComputePosition(m_absPos, m_size);

			m_mouseDragPreviousX = currentMousePosition.x;

			m_onValidate(m_currentValue);
		}
	}

	void SliderFloat::Draw(const Core::Float2& windowSize, const D3D12_RECT& scissor)
	{
		//compute local scissor
		D3D12_RECT localScissorRect;
		{
			localScissorRect.left = m_absPos.x;
			localScissorRect.right = m_absPos.x + m_size.x;
			localScissorRect.top = m_absPos.y;
			localScissorRect.bottom = m_absPos.y + m_size.y;

			//intersection with the parent scissor
			if (localScissorRect.left < scissor.left) localScissorRect.left = scissor.left;
			if (localScissorRect.right > scissor.right) localScissorRect.right = scissor.right;
			if (localScissorRect.top < scissor.top) localScissorRect.top = scissor.top;
			if (localScissorRect.bottom > scissor.bottom) localScissorRect.bottom = scissor.bottom;

			if (localScissorRect.left >= localScissorRect.right || localScissorRect.top >= localScissorRect.bottom)
				return;
		}

		switch (m_mode)
		{
		case Slider:
		{
			DrawBackground(windowSize, localScissorRect);

			Widget::Draw(windowSize, scissor);

			DrawSliderText(windowSize, localScissorRect);
		}
		break;

		case Text:
		{
			Widget::Draw(windowSize, scissor);
		}
		break;
		}
		
	}

	void SliderFloat::SetValue(float value)
	{
		if (m_currentValue != value)
		{
			m_currentValue = value;
			m_onValidate(value);
		}
	}

	int SliderFloat::CalculateSliderLocalX()
	{
		float range = m_maxValue - m_minValue;
		float positionRatio = ((m_currentValue / range) * (m_size.x - m_pSlider->GetWidth() - 2)) + 1; //-2 for the borders
		return static_cast<int>(positionRatio);
	}

	void SliderFloat::DrawBackground(const Core::Float2& windowSize, const D3D12_RECT& localScissor)
	{
		DirectX::XMMATRIX mvpMatrix;
		ComputeWVPMatrix(windowSize, mvpMatrix);

		WidgetMgr& widgetMgr = WidgetMgr::Get();
		Rendering::RenderModule& render = Rendering::RenderModule::Get();
		Rendering::MaterialMgr& materialMgr = Rendering::MaterialMgr::Get();

		render.SetScissorRectangle(localScissor);

		const Rendering::Material* pMaterial = materialMgr.GetMaterial(widgetMgr.m_materialId);
		render.BindMaterial(*pMaterial, mvpMatrix);

		SliderFloatStyle* currentStyle = &m_defaultStyle;
		if (m_hover)
			currentStyle = &m_hoverStyle;

		int valueShowBorder = currentStyle->m_showBorder ? 1 : 0;
		float rect[2] = { (float)m_size.x, (float)m_size.y };

		render.SetConstantBuffer(1, sizeof(currentStyle->m_backgroundColor), &currentStyle->m_backgroundColor, 0);
		render.SetConstantBuffer(2, sizeof(valueShowBorder), &valueShowBorder, 0);
		render.SetConstantBuffer(3, sizeof(currentStyle->m_borderColor), &currentStyle->m_borderColor, 0);
		render.SetConstantBuffer(4, sizeof(rect), &rect, 0);
		render.SetConstantBuffer(5, sizeof(currentStyle->m_borderSize), &currentStyle->m_borderSize, 0);

		const Rendering::Mesh* pMesh = Rendering::MeshMgr::Get().GetMesh(widgetMgr.m_quadMeshId);
		render.RenderMesh(*pMesh);
	}

	void SliderFloat::DrawSliderText(const Core::Float2& windowSize, const D3D12_RECT& localScissor)
	{
		//All of this should be cached
		const int BUFFER_SIZE = 64;
		char buffer[BUFFER_SIZE] = { '\0' };
		sprintf_s(buffer, BUFFER_SIZE, "%g", m_currentValue);

		std::string strValue = buffer;

		const Rendering::Font* pFont = Rendering::FontMgr::Get().GetFont(WidgetMgr::Get().GetUIFontId());
		DirectX::XMUINT2 textRect;
		pFont->ComputeRect(strValue, textRect);
		// until here at least

		float fontScale = 1.f;

		//center the text
		float x = (float)m_absPos.x + m_size.x * 0.5f - static_cast<float>(ceil(textRect.x * 0.5f)); // Us ceil here cause if the text is small, half of it could be too small or 0.
		float y = (float)m_absPos.y + m_size.y * 0.5f - textRect.y * 0.5f;
		DirectX::XMFLOAT3 uiPos(x, y, (float)m_absPos.z - 2);

		//compute a local scissor being the intersection of the 
		DirectX::XMUINT4 localScissorText(localScissor.left, localScissor.top, localScissor.right - localScissor.left, localScissor.bottom - localScissor.top);

		int32_t bottom = static_cast<int32_t>(y) + textRect.y;
		if (bottom <= localScissor.bottom)
			Rendering::RenderModule::Get().PrepareRenderText(strValue, WidgetMgr::Get().GetUIFontId(), uiPos, DirectX::XMFLOAT2(fontScale, fontScale), localScissorText, Widget::NEAR_CAMERA_PLANE, Widget::FAR_CAMERA_PLANE);
	}

	void SliderFloat::TransitionTextToSlider(bool valueChanged)
	{
		m_mode = Slider;
		m_pTextbox->Disable();
		m_pSlider->Enable();

		if (valueChanged)
		{
			m_pSlider->SetX(CalculateSliderLocalX());
			m_pSlider->ReComputePosition(m_absPos, m_size);
			m_onValidate(m_currentValue);
		}
	}

	void SliderFloat::TransitionSliderToText()
	{
		m_pTextbox->Enable();
		m_pTextbox->ReComputePosition(m_absPos, m_size);
		m_pTextbox->SetText(std::to_string(m_currentValue));
		m_pSlider->Disable();
		m_mode = Text;
	}

	void SliderFloat::Slider_OnMouseEnter()
	{
		m_pSlider->GetDefaultStyle().SetBackgroundColor(m_hoverSliderStyle.m_sliderColor);
	}

	void SliderFloat::Slider_OnMouseExit()
	{
		m_pSlider->GetDefaultStyle().SetBackgroundColor(m_defaultStyle.m_sliderColor);
	}

	void SliderFloat::Slider_OnMouseDown(const MouseEvent& ev)
	{
		if (m_isSliderDragging)
			return;

		m_isSliderDragging = true;
		m_mouseDragPreviousX = ev.GetX();

		m_pSlider->CaptureMouse();
	}

	void SliderFloat::Slider_OnMouseUp()
	{
		m_isSliderDragging = false;
		m_pSlider->ReleaseMouse();
	}

	void SliderFloat::TextBox_OnValidate(const std::string& strValue)
	{
		char* pEnd = nullptr;
		float value = std::strtof(strValue.c_str(), &pEnd);

		float oldValue = m_currentValue;
		if (pEnd)
		{
			m_currentValue = value;
		}

		bool valueChanged = oldValue != m_currentValue;
		TransitionTextToSlider(valueChanged);
	}

	void SliderFloat::TextBox_OnFocusLost()
	{
		if (m_mode != Slider)
		{
			TransitionTextToSlider(false);
			m_hover = false;
		}
	}
}
