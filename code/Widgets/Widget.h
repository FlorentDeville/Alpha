/********************************************************************/
/* © 2021 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <string>
#include <vector>

#include <DirectXMath.h>
#include <functional>
#include "MessageId.h"

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
			FIT = HSIZE_FIT | VSIZE_FIT
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

		Widget();
		Widget(uint32_t w, uint32_t h, int32_t x, int32_t y);
		virtual ~Widget();

		virtual void Update(uint64_t dt);	//Handle events and propagate to children
		virtual void Draw(const DirectX::XMFLOAT2& windowSize);	//Draw the widgets using the gfx api.

		virtual void Resize(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize);

		virtual bool Handle(const Message& msg);

		virtual void AddWidget(Widget* pWidget);
		virtual void DeleteChild(Widget* pWidget);
		virtual void DeleteAllChildren();

		virtual void Enable(bool recursive = true);
		virtual void Disable(bool recursive = true);

		// Set local x coordinate from the top left corner of the parent widget
		void SetX(int32_t x);

		// Set local y coordinate from the top left corner of the parent widget
		void SetY(int32_t y);

		void SetSize(const DirectX::XMUINT2& size);

		void SetFocus();

		void SetBackgroundColor(const DirectX::XMVECTOR& color);
		void SetSizeStyle(int sizeStyle);
		void SetPositionStyle(HPOSITION_STYLE hStyle, VPOSITION_STYLE vStyle);

		void SetName(const std::string& name);

		int32_t GetX() const; //Get the local x coordinate from the top left corner
		int32_t GetY() const; //Get the local y coordinate from the top left corner

		DirectX::XMINT2 GetPosition() const;
		DirectX::XMUINT2 GetSize() const;

		int32_t GetScreenX() const; //Get the x coordinate from the top left corner
		int32_t GetScreenY() const; //Get the y coordinate from the top left corner

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;

		const std::vector<Widget*>& GetChildren() const;

		bool IsInside(uint32_t screenX, uint32_t screenY) const;
		bool IsEnabled() const;

		void CaptureMouse();	//Receive the mouse events events if the mouse is not on the widget
		void ReleaseMouse();	//Stop receiving the mouse events

		void OnMouseMove(const std::function<bool(int, int, MouseKey)>& callback);
		void OnLeftMouseUp(const std::function<bool(int, int)>& callback);
		void OnMouseEnter(const std::function<bool()>& callback);
		void OnMouseExit(const std::function<bool()>& callback);
		void OnClick(const std::function<bool(int, int)>& callback);
		void OnGetFocus(const std::function<bool()>& callback);
		void OnLoseFocus(const std::function<bool()>& callback);

		virtual void ReComputeSize(const DirectX::XMUINT2& parentSize);
		virtual void ReComputeSize_PostChildren(); //recompute the size after the resize children for the FIT style.
		virtual void ReComputePosition(const DirectX::XMINT3& parentAbsPos, const DirectX::XMUINT2& parentSize);
		virtual void ResizeChildren();

	protected:
		DirectX::XMUINT2 m_size; //current size in pixels
		DirectX::XMUINT2 m_defaultSize; // size provided by the client and used when style is default
		DirectX::XMINT3 m_locPos; //local position of the top left corner in pixel
		DirectX::XMINT3 m_absPos; //absolute position (from the top left corner)

		char m_padding[8];
		DirectX::XMVECTOR m_backgroundColor;
		DirectX::XMVECTOR m_borderColor;
		bool m_showBorder;
		int m_borderWidth;

		bool m_hover;	//Indicate if the mouse is hovering the widget;
		bool m_enabled;	//If a widget is disabled, it is not rendered and doesn't receive messages

		std::vector<Widget*> m_children;

		int m_sizeStyle;
		HPOSITION_STYLE m_hPositionStyle;
		VPOSITION_STYLE m_vPositionStyle;

		std::function<bool(int x, int y, MouseKey)> m_onMouseMove;
		std::function<bool(int x, int y)>			m_onLeftMouseUp;
		std::function<bool()>						m_onMouseEnter;
		std::function<bool()>						m_onMouseExit;
		std::function<bool(int x, int y)>			m_onClick;
		std::function<bool()>						m_onGetFocus;
		std::function<bool()>						m_onLoseFocus;
		std::string m_name;

		void ComputeWVPMatrix(const DirectX::XMFLOAT2& windowSize, DirectX::XMMATRIX& wvp) const;

	private:
		virtual void Internal_Render(); //only used by widgets rendering 3d like a viewport

	};

}

