/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"
#include "Editors/RawDb.h"

#include <string>
#include <vector>

namespace Widgets
{
	class Layout;
	class Text;
	class Widget;
}

namespace Editors
{
	class ShaderEntry
	{
	public:
		std::string m_rawFilename;
		Systems::AssetId m_assetId;

		ShaderEntry();
	};

	class ShaderEditorParameter
	{
	public:
		Widgets::Widget* m_pParent;
		std::string m_rawShaderPath;
		std::string m_dataShaderPath;
		std::string m_shaderCompilerPath;
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

		std::vector<ShaderEntry> m_allShaders;

		Widgets::Layout* m_pShaderListLayout;
		Widgets::Text* m_pLogText;
		int m_selectedShader;

		std::string m_rawShaderPath;
		std::string m_dataShaderPath;
		std::string m_shaderCompilerPath;

		RawDb m_shaderRawDb;

		void CreateMenu(Widgets::Widget* pParent);

		void OnMenuFile_NewShader_Clicked();

		bool OnShaderEntryClicked(int index);
		bool OnCompileClicked();
	};
}
