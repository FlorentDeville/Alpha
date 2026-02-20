/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#pragma once

#include "Editors/BaseEditor.h"
#include "Editors/ObjectWatcher/ObjectWatcherCallbackId.h"

#include "Systems/Assets/AssetObjects/Mesh/MeshAsset.h"
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
	class MaterialListModel;

	class MaterialEditor : public BaseEditor
	{
	public:
		MaterialEditor();
		~MaterialEditor();

		void CreateEditor(Widgets::Widget* pParent) override;

	private:
		Widgets::Layout* m_pShaderListLayout;
		Widgets::Text* m_pLogText;

		PropertyGridWidget* m_pPropertyGrid;
		PropertyGridPopulator* m_pPropertyGridPopulator;

		MaterialListModel* m_pMaterialListModel;

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

		ObjectWatcherCallbackId m_objWatcherCid;

		void CreateMenu();

		void MenuFile_NewMaterial_OnClicked();
		void MenuFile_NewMaterialInstance_OnClicked();
		void MenuFile_Save_OnClicked();
		void MenuFile_Delete_OnClicked();
		void MenuFile_Rename_OnClicked();

		bool OnShaderEntryClicked(Systems::NewAssetId id);
		bool OnCompileClicked();
		bool OnCompileDebugClicked();
		bool OnNewHlslClicked();

		void PropertyGridPopulator_OnDataChanged();

		void DeleteSelectedShader();

		void CreateShadersList();

		void Viewport_OnRender();
		void Viewport_OnUpdate(uint64_t dt);

		void RefreshPropertyGrid();

		bool InternalOnCompileClicked(bool debug);
	};
}
