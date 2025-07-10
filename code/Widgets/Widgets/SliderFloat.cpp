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
	{
		m_defaultStyle.SetBackgroundColor(Widgets::Color(0.22f, 0.22f, 0.22f));
		m_defaultStyle.SetBorderColor(Widgets::Color(0.26f, 0.26f, 0.26f));
		m_defaultStyle.SetShowBorder(true);
		m_defaultStyle.SetBorderSize(1);
		m_defaultStyle.SetSliderColor(Widgets::Color(77, 77, 77));

		m_hoverStyle.SetBackgroundColor(Widgets::Color(0.12f, 0.12f, 0.12f));
		m_hoverStyle.SetBorderColor(Widgets::Color(0.6f, 0.6f, 0.6f));
		m_hoverStyle.SetBorderSize(1);
		m_hoverStyle.SetShowBorder(true);

		m_hoverSliderStyle = m_hoverStyle;
		m_hoverSliderStyle.SetSliderColor(Widgets::Color(153, 153, 153));

		//first make the slider
		m_pSlider = new Container(10, 18);
		m_pSlider->SetPositionStyle(Widget::HPOSITION_STYLE::NONE, Widget::VPOSITION_STYLE::MIDDLE);
		m_pSlider->GetDefaultStyle().SetBackgroundColor(m_defaultStyle.m_sliderColor);
		m_pSlider->SetX(CalculateSliderLocalX());

		m_pSlider->OnMouseEnter([this](const Widgets::MouseEvent& ev) 
			{ 
				m_pSlider->GetDefaultStyle().SetBackgroundColor(m_hoverSliderStyle.m_sliderColor); 
			});

		m_pSlider->OnMouseExit([this](const Widgets::MouseEvent& ev) 
			{ 
				m_pSlider->GetDefaultStyle().SetBackgroundColor(m_defaultStyle.m_sliderColor); 
			});

		m_pSlider->OnMouseDown([this](const Widgets::MouseEvent& ev) 
			{ 
				if (m_isSliderDragging)
					return;

				m_isSliderDragging = true;
				m_mouseDragPreviousX = ev.GetX(); 

				m_pSlider->CaptureMouse();
			});

		m_pSlider->OnMouseUp([this](const Widgets::MouseEvent& ev) 
			{ 
				m_isSliderDragging = false;
				m_pSlider->ReleaseMouse();
			});

		AddWidget(m_pSlider);
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

	void SliderFloat::Draw(const DirectX::XMFLOAT2& windowSize, const D3D12_RECT& scissor)
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

		//draw background
		{
			DirectX::XMMATRIX mvpMatrix;
			ComputeWVPMatrix(windowSize, mvpMatrix);

			WidgetMgr& widgetMgr = WidgetMgr::Get();
			Rendering::RenderModule& render = Rendering::RenderModule::Get();
			Rendering::MaterialMgr& materialMgr = Rendering::MaterialMgr::Get();

			render.SetScissorRectangle(localScissorRect);

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

		Widget::Draw(windowSize, scissor);

		{
			//All of this should be cached
			std::string strValue = std::to_string(m_currentValue);

			const Rendering::Font* pFont = Rendering::FontMgr::Get().GetFont(WidgetMgr::Get().GetUIFontId());
			DirectX::XMUINT2 textRect;
			pFont->ComputeRect(strValue, textRect);
			// until here at least

			float fontScale = 1.f;
			const int textXOffset = 5;
			const int textYOffset = 0;

			//center the text
			float x = (float)m_absPos.x + m_size.x * 0.5f - textRect.x * 0.5f;
			float y = (float)m_absPos.y + m_size.y * 0.5f - textRect.y * 0.5f;
			DirectX::XMFLOAT3 uiPos(x, y, (float)m_absPos.z - 2);

			//compute a local scissor being the intersection of the 
			DirectX::XMUINT4 localScissor(localScissorRect.left, localScissorRect.top, localScissorRect.right - localScissorRect.left, localScissorRect.bottom - localScissorRect.top);

			int32_t bottom = static_cast<int32_t>(y) + textRect.y;
			if (bottom <= localScissorRect.bottom)
				Rendering::RenderModule::Get().PrepareRenderText(strValue, WidgetMgr::Get().GetUIFontId(), uiPos, DirectX::XMFLOAT2(fontScale, fontScale), localScissor, Widget::NEAR_CAMERA_PLANE, Widget::FAR_CAMERA_PLANE);
		}
	}

	int SliderFloat::CalculateSliderLocalX()
	{
		float range = m_maxValue - m_minValue;
		float positionRatio = ((m_currentValue / range) * (m_size.x - m_pSlider->GetWidth() - 2)) + 1; //-2 for the borders
		return static_cast<int>(positionRatio);
	}
}
