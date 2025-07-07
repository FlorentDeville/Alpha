/********************************************************************/
/* © 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Core/Singleton.h"
#include "Systems/Assets/AssetObjects/MeshAsset.h"
#include "Systems/Assets/NewAssetId.h"

#include <DirectXMath.h>

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

	class MaterialEditorParameter
	{
	public:
		Widgets::Widget* m_pParent;
	};

	class MaterialEditor : public Core::Singleton<MaterialEditor>
	{
	public:
		MaterialEditor();
		~MaterialEditor();

		void CreateEditor(const MaterialEditorParameter& parameter);

	private:
		Widgets::Layout* m_pShaderListLayout;
		Widgets::Text* m_pLogText;

		PropertyGridWidget* m_pPropertyGrid;
		PropertyGridPopulator* m_pPropertyGridPopulator;

		ShaderListModel* m_pShaderListModel;

		Systems::NewAssetId m_selectedMaterialId;

		//camera
		float m_cameraDistance;
		float m_aspectRatio;
		DirectX::XMVECTOR m_cameraTarget;
		DirectX::XMVECTOR m_cameraEuler;

		//viewport inputs (mouse and keyboard)
		bool m_firstFrameMouseDown;
		DirectX::XMUINT2 m_mousePreviousPos;

		Systems::MeshAsset* m_pMesh; //mesh to render in the viewport

		void CreateMenu(Widgets::Widget* pParent);

		void MenuFile_NewMaterial_OnClicked();
		void MenuFile_Save_OnClicked();
		void MenuFile_Delete_OnClicked();

		bool OnShaderEntryClicked(Systems::NewAssetId id);
		bool OnCompileClicked();

		void PropertyGridPopulator_OnDataChanged();

		void DeleteSelectedShader();

		void CreateShadersList();

		void Viewport_OnRender();
		void Viewport_OnUpdate(uint64_t dt);

		void RefreshPropertyGrid();
	};
}
