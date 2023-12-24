/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/LevelEditor/GizmoAxis.h"

namespace Editors
{
	GizmoAxis::GizmoAxis()
		: m_axis(GizmoAxisEnum::kEmpty)
	{}

	GizmoAxis::GizmoAxis(GizmoAxisEnum axis)
		: m_axis(axis)
	{}

	GizmoAxis::~GizmoAxis()
	{}

	bool GizmoAxis::Contains(GizmoAxisEnum axis) const
	{
		return (m_axis & axis) == axis;
	}

	bool GizmoAxis::IsSingleAxis() const
	{
		if (m_axis == GizmoAxisEnum::kXAxis || m_axis == GizmoAxisEnum::kYAxis || m_axis == GizmoAxisEnum::kZAxis)
			return true;

		return false;
	}

	bool GizmoAxis::IsEmpty() const
	{
		return m_axis == GizmoAxisEnum::kEmpty;
	}

	int GizmoAxis::GetAxisIndex() const
	{
		if (m_axis == GizmoAxisEnum::kXAxis)
			return 0;

		if (m_axis == GizmoAxisEnum::kYAxis)
			return 1;

		if (m_axis == GizmoAxisEnum::kZAxis)
			return 2;
			
		return -1;
	}

	GizmoAxis::GizmoAxisEnum operator&(GizmoAxis::GizmoAxisEnum a, GizmoAxis::GizmoAxisEnum b)
	{
		return static_cast<GizmoAxis::GizmoAxisEnum>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
	}
}
