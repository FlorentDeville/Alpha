/********************************************************************/
/* � 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include <assert.h>
#include <string>

#include "Editors/Property.h"

namespace Editors
{
	// This is the base class for a model in a model view pattern.
	// The goal of the model is to be the interface between the data of the application
	// and the view (widgets) : data -> model -> widget.
	// The widget should not contain any logic about how to get/set or convert the data, the model
	// takes care of it.
	// A model is specific to a data. But technically, any widget using the model can use it to render
	// the data.
	// A Model can work with tree-like structures. GetSubRowCount is used to find the number of sub row for a row.
	// GetSubModel return the BaseModel to use to access the data of the sub row.

	class BaseModel
	{
	public:
		BaseModel() {}
		virtual ~BaseModel() {}

		virtual int GetRowCount() const = 0;
		virtual int GetColumnCount() const = 0;
		virtual int GetChildrenCount(int /*rowId*/) const { return 0; }

		virtual const std::string& GetData(int rowId = 0, int columnId = 0) const = 0;
		virtual PropertyType GetDataType(int /*rowId = 0*/, int /*columnId = 0*/) const { return PropertyType::kUnknown; }

		virtual BaseModel* GetSubModel(int /*rowId*/, int /*columnId*/) { return nullptr; }

		virtual void SetData(int /*rowId*/, int /*columnId*/, const std::string& /*value*/) {};
	};
}
