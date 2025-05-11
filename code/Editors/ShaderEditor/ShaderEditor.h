/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"
#include "Systems/Assets/NewAssetId.h"

#include <string>
#include <vector>

namespace Systems
{
	class FieldDescriptor;
	class Object;
}

namespace Widgets
{
	class Layout;
	class Text;
	class Widget;
}

namespace Editors
{
	class PropertyGridPopulator;
	class PropertyGridWidget;
	class ShaderListModel;

	class ShaderEditorParameter
	{
	public:
		Widgets::Widget* m_pParent;
	};

	class ShaderEditor : public Core::Singleton<ShaderEditor>
	{
	public:
		ShaderEditor();
		~ShaderEditor();

		void CreateEditor(const ShaderEditorParameter& parameter);

		void Update();
		void Render();

	private:
		Widgets::Layout* m_pShaderListLayout;
		Widgets::Text* m_pLogText;

		PropertyGridWidget* m_pPropertyGrid;
		PropertyGridPopulator* m_pPropertyGridPopulator;

		ShaderListModel* m_pShaderListModel;

		Systems::NewAssetId m_selectedMaterialId;

		void CreateMenu(Widgets::Widget* pParent);

		void OnMenuFile_NewShader_Clicked();
		void OnMenuFile_Save_Clicked();
		void MenuFile_Delete_OnClicked();

		bool OnShaderEntryClicked(Systems::NewAssetId id);
		bool OnCompileClicked();

		void PropertyGridPopulator_OnDataChanged(Systems::Object* pObject, const Systems::FieldDescriptor* pField);

		void CreateShadersList();
	};
}
