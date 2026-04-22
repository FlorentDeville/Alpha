/********************************************************************************/
/* Copyright (C) 2022 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************************/

#pragma once

#include "Core/Math/Vec4f.h"
#include "Core/Math/Vectors.h"

#include "Editors/BaseEditor.h"
#include "Editors/ObjectWatcher/ObjectWatcherCallbackId.h"

#include "Systems/Assets/NewAssetId.h"

#include <vector>

namespace Rendering
{
	class Mesh;
}

namespace Systems
{
	class FieldDescriptor;
	class Object;
}

namespace Widgets
{
	class Layout;
	class MenuItem;
	class Text;
	class Viewport;
	class Widget;
}

namespace Editors
{
	class PropertyGridPopulator;
	class PropertyGridWidget;
	class MaterialListModel;
	class WidgetController;

	class MaterialEditor : public BaseEditor
	{
	public:
		MaterialEditor();
		~MaterialEditor();

		void CreateEditor(const EditorParameter& param) override;

	private:
		enum DisplayMesh
		{
			Unknown = -1,
			Sphere = 0,
			Cube,

			Count
		};

		Widgets::Layout* m_pShaderListLayout;
		Widgets::Text* m_pLogText;

		PropertyGridWidget* m_pPropertyGrid;
		PropertyGridPopulator* m_pPropertyGridPopulator;

		MaterialListModel* m_pMaterialListModel;

		Systems::NewAssetId m_selectedMaterialId;

		//camera
		float m_cameraDistance;
		float m_aspectRatio;
		Core::Vec4f m_cameraTarget;
		Core::Vec4f m_cameraEuler;

		//viewport inputs (mouse and keyboard)
		bool m_firstFrameMouseDown;
		Core::Int2 m_mousePreviousPos;

		Rendering::Mesh* m_pMeshes[DisplayMesh::Count]; //list of possible meshes to render in the viewport
		Widgets::MenuItem* m_pMeshesMenuItem[DisplayMesh::Count];
		DisplayMesh m_selectedMesh;

		ObjectWatcherCallbackId m_objWatcherCid;

		WidgetController* m_pController;

		Widgets::Viewport* m_pViewport;

		void CreateFileMenu();
		void CreateMeshMenu();
		void CreateToolsMenu();

		void MenuFile_NewMaterial_OnClicked();
		void MenuFile_NewMaterialInstance_OnClicked();
		void MenuFile_Save_OnClicked();
		void MenuFile_Delete_OnClicked();
		void MenuFile_Rename_OnClicked();

		void MenuMesh_OnClicked(DisplayMesh mesh);

		void MenuTools_ResaveAllInstances_OnClicked();

		bool OnShaderEntryClicked(Systems::NewAssetId id);
		bool OnCompileClicked();
		bool OnCompileDebugClicked();
		bool OnNewHlslClicked();
		bool OnRefreshClicked();

		void PropertyGridPopulator_OnDataChanged();

		void DeleteSelectedShader();

		void CreateShadersList();

		void Viewport_OnRender();
		void Viewport_OnUpdate(uint64_t dt);

		void RefreshPropertyGrid();

		bool InternalOnCompileClicked(bool debug);
	};
}
