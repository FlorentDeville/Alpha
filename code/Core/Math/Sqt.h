/********************************************************************/
/* © 2023 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Math/Mat44f.h"
#include "Core/Math/Vec4f.h"

namespace Core
{
	//Scale, rotation (quaternion), translation
	class Sqt
	{
	public:
		Sqt();
		Sqt(const Mat44f& transform);

		void SetTranslation(const Vec4f& translation);
		void SetRotationQuaternion(const Vec4f& eulerAngles);
		void SetScale(const Vec4f& scale);

		const Vec4f& GetTranslation() const;
		const Vec4f& GetRotationQuaternion() const;
		const Vec4f& GetScale() const;
		
		const Mat44f& GetMatrix() const;

	private:
		Vec4f m_translation;
		Vec4f m_quat;
		Vec4f m_scale;

		//cached data
		mutable bool m_dirty;
		mutable Mat44f m_matrix;
	};
}
