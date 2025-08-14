/********************************************************************/
/* © 2024 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Callbacks/CallbackList.h"

//Example of what this macro does
// 
//public:
//  using OnMouseEnterEvent = Core::CallbackList<void(const MouseEvent&)>;
//
//protected
//  OnMouseEnterEvent							m_onMouseEnter;
//
//public:
//	Core::CallbackId OnMouseEnter(const OnMouseEnterEvent::Callback& callback) { return m_onMouseEnter.Connect(callback); }
//  void DisconnectOmMouseEnter(const Core::CallbackId& id) { m_onMouseEnter.Disconnect(id); }

#define EVENT_TYPE_NAME(NAME) On ## NAME ## Event
#define EVENT_VARIABLE(NAME) m_on ## NAME

#define EVENT_TYPE_DECL(NAME, FUNC) using EVENT_TYPE_NAME(NAME) = Core::CallbackList<FUNC>;
#define EVENT_VARIABLE_DECL(NAME) EVENT_TYPE_NAME(NAME) EVENT_VARIABLE(NAME);

#define CONNECT(NAME) \
Core::CallbackId On ## NAME (const EVENT_TYPE_NAME(NAME)::Callback& callback) \
{ \
    return EVENT_VARIABLE(NAME).Connect(callback); \
}

#define DISCONNECT(NAME) \
void DisconnectOn ## NAME (const Core::CallbackId& id) \
{ \
    EVENT_VARIABLE(NAME).Disconnect(id); \
}

#define CLEAR(NAME) \
void ClearOn ## NAME () \
{ \
    EVENT_VARIABLE(NAME).Clear(); \
}

//This is the macro you want to use to declare a public event
// Example : EVENT_DECL(MouseMove, void(const MouseEvent&))
#define EVENT_DECL(NAME, FUNC) \
public: \
    EVENT_TYPE_DECL(NAME, FUNC) \
protected: \
    EVENT_VARIABLE_DECL(NAME) \
public: \
    CONNECT(NAME) \
    DISCONNECT(NAME) \
    CLEAR(NAME)

//This is the macro you want to use to declare a private event
// Example : PRIVATE_EVENT_DECL(MouseMove, void(const MouseEvent&))
#define PRIVATE_EVENT_DECL(NAME, FUNC) \
private: \
    EVENT_TYPE_DECL(NAME, FUNC) \
    EVENT_VARIABLE_DECL(NAME) \
    CONNECT(NAME) \
    DISCONNECT(NAME) \
    CLEAR(NAME)
