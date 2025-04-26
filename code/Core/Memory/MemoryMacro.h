/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Memory/Allocator.h"
#include "Core/Memory/AllocatorStack.h"

#define MAKENEW(Type) new (Core::AllocatorStack::Get().GetTopAllocator()->Allocate(sizeof(Type), alignof(Type))) Type
#define DELETE(Ptr) Core::AllocatorStack::Get().GetTopAllocator()->Free(Ptr);