/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>
#include <vector>

#include <DirectXMath.h>
#include <d3d12.h>
#include <functional>
#include "MessageId.h"

#include "Core/Callbacks/CallbackList.h"
#include "Core/Callbacks/CallbackMacro.h"
#include "Core/Math/VecInt.h"

#include "Widgets/Tools/Color.h"

struct Message;

/********************************************************************/
/* Base class for Widget											*/		
/* Every widget local frame of reference is its top left corner.	*/
/* X goes right, Y goes down.										*/	
/* Every position is relative to the parent widget.					*/
/*																	*/
/* Screen coordinate :  origin : center of the screen				*/ 
/*						range : [-1, 1]								*/
/*																	*/		
/* UI coordinate :	origin : top left of the screen					*/
/*					range : [0, window size]						*/
/*																	*/
/* All the widgets dimensions are in pixels and independent of the	*/
/* main window size and resolution									*/
/* The positions are in UI coordinate.								*/
/********************************************************************/
namespace Widgets
{
	class BaseEvent;
	class FocusEvent;
	class MouseEvent;

	class Widget
	{
		friend class WidgetMgr;

	public:

		//All the widgets (including the text) are between the near and far plane. The distances are in world space.
		static const float NEAR_CAMERA_PLANE;
		static const float FAR_CAMERA_PLANE;

		// Defines how to compute the size of a widget:
		// DEFAULT	: fixed size.
		// STRETCH	: stretch the widget to take all its parent's space.
		// FIT		: take the size of its content.
		enum SIZE_STYLE
		{
			HSIZE_DEFAULT	= 0x00000001,
			VSIZE_DEFAULT	= 0x00000010,

			HSIZE_STRETCH	= 0x00000100,
			VSIZE_STRETCH	= 0x00001000,

			HSIZE_FIT		= 0x00010000,
			VSIZE_FIT		= 0x00100000,

			DEFAULT = HSIZE_DEFAULT | VSIZE_DEFAULT,
			STRETCH = HSIZE_STRETCH | VSIZE_STRETCH,
			FIT = HSIZE_FIT | VSIZE_FIT,

			HSTRETCH_VFIT = HSIZE_STRETCH | VSIZE_FIT
		};

		enum class HPOSITION_STYLE
		{
			LEFT,
			CENTER,
			RIGHT,
			NONE		//Use the provided position
		};

		enum class VPOSITION_STYLE
		{
			TOP,
			MIDDLE,
			BOTTOM,
			NONE		//Use the provided position
		};

		enum class FOCUS_POLICY
		{
			DEFAULT,
			NO_FOCUS
		};

		Widget();
		Widget(uint32_t w, uint32_t h, int32_t x, int32_t y);
		virtual ~Widget();

		virtual void Update(uint64_t dt); //Real time update called every frame.

		// Draw the widgets using the gfx api.
		// windowSize : size of the entire window
		// scissor : rectangle from which the widgets must be cropped.
		virtual void Draw(const DirectX::XMFLOAT2& windowSize, const D3D12_RECT& scissor);

		virtual void Resize(const Core::Int3& parentAbsPos, const Core::UInt2& parentSize);

		virtual bool Handle(const BaseEvent& event); //Handle events and propagate to children

		virtual void AddWidget(Widget* pWidget);
		virtual void InsertWidget(Widget* pWidget, int position);
		virtual void DeleteChild(Widget* pWidget);
		virtual void DeleteAllChildren();

		virtual void Enable(bool recursive = true);
		virtual void Disable(bool recursive = true);

		// Set local x coordinate from the top left corner of the parent widget
		void SetX(int32_t x);

		// Set local y coordinate from the top left corner of the parent widget
		void SetY(int32_t y);

		void SetSize(const Core::UInt2& size);
		void SetWidth(uint32_t width);

		void SetFocus();

		void SetBackgroundColor(const Color& color);
		void SetSizeStyle(int sizeStyle);
		void SetPositionStyle(HPOSITION_STYLE hStyle, VPOSITION_STYLE vStyle);

		void SetName(const std::string& name);

		void SetFocusPolicy(FOCUS_POLICY policy);

		int32_t GetX() const; //Get the local x coordinate from the top left corner
		int32_t GetY() const; //Get the local y coordinate from the top left corner

		DirectX::XMINT2 GetPosition() const;
		Core::UInt2 GetSize() const;

		int32_t GetScreenX() const; //Get the x coordinate from the top left corner
		int32_t GetScreenY() const; //Get the y coordinate from the top left corner

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;

		FOCUS_POLICY GetFocusPolicy() const;
		bool HasFocus() const;

		const std::vector<Widget*>& GetChildren() const;
		uint32_t GetChildrenCount() const;

		Widget* GetParent();

		bool IsInside(uint32_t screenX, uint32_t screenY) const;
		bool IsEnabled() const;

		void CaptureMouse();	//Receive the mouse events events if the mouse is not on the widget
		void ReleaseMouse();	//Stop receiving the mouse events

		virtual void ReComputeSize(const Core::UInt2& parentSize);
		virtual void ReComputeSize_PostChildren(); //recompute the size after the resize children for the FIT style.
		virtual void ReComputePosition(const Core::Int3& parentAbsPos, const Core::UInt2& parentSize);
		virtual void ResizeChildren();

		/* Let's put all events declaration here*/
		EVENT_DECL(MouseEnter, void(const MouseEvent&))
		EVENT_DECL(MouseExit, void(const MouseEvent&))
		EVENT_DECL(MouseMove, void(const MouseEvent&))
		EVENT_DECL(MouseDown, void(const MouseEvent&))
		EVENT_DECL(MouseUp, void(const MouseEvent&))
		EVENT_DECL(MouseDoubleClick, void(const MouseEvent&))
		EVENT_DECL(FocusGained, void(const FocusEvent&))
		EVENT_DECL(FocusLost, void(const FocusEvent&))

	protected:
		Core::UInt2 m_size; //current size in pixels
		Core::UInt2 m_defaultSize; // size provided by the client and used when style is default
		Core::Int3 m_locPos; //local position of the top left corner in pixel
		Core::Int3 m_absPos; //absolute position (from the top left corner)

		char m_padding[8];
		Color m_backgroundColor;
		Color m_borderColor;
		bool m_showBorder;
		int m_borderWidth;

		bool m_hover;	//Indicate if the mouse is hovering the widget;
		bool m_enabled;	//If a widget is disabled, it is not rendered and doesn't receive messages

		std::vector<Widget*> m_children;
		Widget* m_pParent;

		int m_sizeStyle;
		HPOSITION_STYLE m_hPositionStyle;
		VPOSITION_STYLE m_vPositionStyle;

		FOCUS_POLICY m_focusPolicy;

		std::string m_name;

		void ComputeWVPMatrix(const DirectX::XMFLOAT2& windowSize, DirectX::XMMATRIX& wvp) const;

	private:
		virtual void Internal_Render(); //only used by widgets rendering 3d like a viewport

	};

}

