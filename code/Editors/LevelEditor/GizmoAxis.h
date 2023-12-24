/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <cstdint>

namespace Editors
{
	class GizmoAxis
	{
	public:
		enum class GizmoAxisEnum : uint8_t
		{
			kEmpty = 0,
			kXAxis = 0x1,
			kYAxis = 0x2,
			kZAxis = 0x4,
		};

		GizmoAxis();
		GizmoAxis(GizmoAxisEnum axis);
		~GizmoAxis();

		bool Contains(GizmoAxisEnum axis) const;
		bool IsSingleAxis() const;
		bool IsEmpty() const;

		int GetAxisIndex() const;

		void SetAxisIndex(uint8_t index);

	private:
		GizmoAxisEnum m_axis;
	};

	GizmoAxis::GizmoAxisEnum operator&(GizmoAxis::GizmoAxisEnum a, GizmoAxis::GizmoAxisEnum b);
}
