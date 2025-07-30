/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/Dialog/AssetDialog.h"

#include "Core/Collections/Array.h"

#include "Systems/Assets/AssetMgr.h"

#include "Widgets/Button.h"
#include "Widgets/Label.h"
#include "Widgets/Layout.h"
#include "Widgets/Models/AbstractViewModel.h"
#include "Widgets/Models/SelectionModel.h"
#include "Widgets/Models/SelectionRow.h"
#include "Widgets/TextBox.h"
#include "Widgets/WidgetMgr.h"
#include "Widgets/Widgets/TableView.h"

#include <list>

namespace Editors
{
	class AssetListModel : public Widgets::AbstractViewModel
	{
	public:
		enum Columns
		{
			Name,
			Type,
			Id,

			Count
		};

		AssetListModel(Core::Sid type)
		{
			Systems::AssetMgr::Get().ForEachMetadata([this, type](const Systems::AssetMetadata& metadata)
				{
					if (metadata.GetAssetType() != type && type != Core::INVALID_SID)
						return;

					MetadataCache cache;
					cache.m_AssetId = metadata.GetAssetId();
					cache.m_id = metadata.GetAssetId().ToString();
					cache.m_name = metadata.GetVirtualName();

					const Systems::NewAssetType& type = Systems::AssetMgr::Get().GetAssetType(metadata.GetAssetType());
					cache.m_type = type.GetName();

					m_cachedAsset.PushBack(cache);
				});
		}

		~AssetListModel()
		{ }

		Widgets::ModelIndex GetParent(const Widgets::ModelIndex& child) const override
		{
			return Widgets::ModelIndex();
		}

		Widgets::ModelIndex GetIndex(int row, int column, const Widgets::ModelIndex& parent) const override
		{
			if (parent.IsValid())
				return Widgets::ModelIndex();

			if (!m_cachedAsset.IsValidIndex(row))
				return Widgets::ModelIndex();

			if (column < 0 || column >= Columns::Count)
				return Widgets::ModelIndex();

			return CreateIndex(row, column, &(m_cachedAsset[row]));
		}

		int GetRowCount(const Widgets::ModelIndex& parent) const override
		{
			if (parent.IsValid())
				return 0;

			return m_cachedAsset.GetSize();
		}

		int GetColumnCount(const Widgets::ModelIndex& parent) const override
		{
			if (parent.IsValid())
				return 0;

			return Columns::Count;
		}

		std::string GetHeaderData(int columnIndex)
		{
			switch (columnIndex)
			{
			case Columns::Id:
				return "Id";
				break;

			case Columns::Name:
				return "Name";
				break;

			case Columns::Type:
				return "Type";
				break;
			}

			return "__ERROR__";
		}

		std::string GetData(const Widgets::ModelIndex& idx) override
		{
			const MetadataCache* pMetadata = idx.GetConstDataPointer<MetadataCache>();
			if (!pMetadata)
				return "__ERROR__";

			switch (idx.GetColumn())
			{
			case Columns::Id:
				return pMetadata->m_id;
				break;

			case Columns::Name:
				return pMetadata->m_name;
				break;

			case Columns::Type:
				return pMetadata->m_type;
				break;
			}

			return "__ERROR__";
		}

	private:
		struct MetadataCache
		{
			std::string m_name;
			std::string m_id;
			std::string m_type;
			Systems::NewAssetId m_AssetId;
		};

		Core::Array<MetadataCache> m_cachedAsset;
	};

	AssetDialog::AssetDialog(Core::Sid assetType)
		: Widgets::ModalWindow("Select Asset")
	{
		const int WINDOW_WIDTH = 500;
		const int WINDOW_HEIGHT = 500;
		const int NEW_ASSET_HEIGHT = 20;
		const int OK_CANCEL_HEIGHT = 30;

		int TABLE_HEIGHT = WINDOW_HEIGHT - OK_CANCEL_HEIGHT;

		SetSize(Core::UInt2(WINDOW_WIDTH, WINDOW_HEIGHT));
		SetSizeStyle(Widgets::Widget::DEFAULT);
		SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::CENTER, Widgets::Widget::VPOSITION_STYLE::MIDDLE);

		//vlayout
		Widgets::Layout* pVLayout = new Widgets::Layout();
		pVLayout->SetDirection(Widgets::Layout::Vertical_Reverse);
		pVLayout->SetSizeStyle(Widgets::Widget::STRETCH);
		AddWidget(pVLayout);

		{
			//button ok escape
			Widgets::Layout* pHLayout = new Widgets::Layout();
			pHLayout->SetDirection(Widgets::Layout::Horizontal);
			pHLayout->SetSizeStyle(Widgets::Widget::HSTRETCH_VFIT);
			pVLayout->AddWidget(pHLayout);

			std::string okLabel = "Select";
			Widgets::Button* pOkButton = new Widgets::Button(250, OK_CANCEL_HEIGHT, 0, 0);
			Widgets::Label* pOkLabel = new Widgets::Label(0, 0, 1, okLabel);
			pOkLabel->SetSizeStyle(Widgets::Widget::FIT);
			pOkLabel->SetPositionStyle(Widget::HPOSITION_STYLE::CENTER, Widget::VPOSITION_STYLE::MIDDLE);
			pOkButton->AddWidget(pOkLabel);
			pOkButton->SetSizeStyle(Widgets::Widget::DEFAULT);
			pOkButton->OnClick([this]() { OnOk(); });

			pHLayout->AddWidget(pOkButton);

			Widgets::Button* pCancelButton = new Widgets::Button(250, OK_CANCEL_HEIGHT, 0, 0);
			Widgets::Label* pCancelLabel = new Widgets::Label(0, 0, 1, "Cancel");
			pCancelLabel->SetSizeStyle(Widgets::Widget::FIT);
			pCancelLabel->SetPositionStyle(Widget::HPOSITION_STYLE::CENTER, Widget::VPOSITION_STYLE::MIDDLE);
			pCancelButton->AddWidget(pCancelLabel);
			pCancelButton->SetSizeStyle(Widgets::Widget::DEFAULT);
			pCancelButton->OnClick([this]() { OnCancel(); });
			pHLayout->AddWidget(pCancelButton);
		}

		//list
		m_pAssetView = new Widgets::TableView();
		m_pAssetView->SetSizeStyle(Widgets::Widget::STRETCH);
		m_pAssetView->OnItemDoubleClick([this](const Widgets::ModelIndex& index) { OnOk(); });

		m_pAssetViewModel = new Editors::AssetListModel(assetType);
		m_pAssetView->SetModel(m_pAssetViewModel);
		m_pAssetView->SetColumnWidth(0, 300);

		pVLayout->AddWidget(m_pAssetView);
	}

	AssetDialog::~AssetDialog()
	{}

	void AssetDialog::Open()
	{
		Widgets::WidgetMgr::Get().OpenModalWindow(this);
	}

	void AssetDialog::Close() const
	{
		Widgets::WidgetMgr::Get().CloseModalWindow();
	}

	void AssetDialog::OnOk()
	{
		const std::list<Widgets::SelectionRow>& selection = m_pAssetViewModel->GetSelectionModel()->GetSelectedRows();
		if (selection.empty())
			return;

		const void* pData = selection.back().GetStartIndex().GetConstDataPointer();
		const Systems::NewAssetId* pAsset = static_cast<const Systems::NewAssetId*>(pData);
		m_onOk(*pAsset);

		Close();
	}

	void AssetDialog::OnCancel()
	{
		Close();
	}
}
