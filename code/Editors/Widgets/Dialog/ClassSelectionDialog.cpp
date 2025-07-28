/********************************************************************/
/* © 2025 Florent Devillechabrol <florent.devillechabrol@gmail.com>	*/
/********************************************************************/

#include "Editors/Widgets/Dialog/ClassSelectionDialog.h"

#include "Systems/Reflection/ReflectionMgr.h"

#include "Widgets/Button.h"
#include "Widgets/Layout.h"
#include "Widgets/Models/AbstractViewModel.h"
#include "Widgets/Models/ModelIndex.h"
#include "Widgets/Models/SelectionModel.h"
#include "Widgets/Models/SelectionRow.h"
#include "Widgets/Widgets/TableView.h"
#include "Widgets/WidgetMgr.h"

namespace Editors
{
	class ClassModel : public Widgets::AbstractViewModel
	{
	public:
		ClassModel(const Core::Sid& baseClassSid)
			: Widgets::AbstractViewModel()
		{
			Systems::ReflectionMgr::Get().ForEachType([this, baseClassSid](const Systems::TypeDescriptor* pType)
				{
					bool validType = false;
					const Systems::TypeDescriptor* pParentType = pType;
					while (pParentType)
					{
						if (pParentType->GetSid() == baseClassSid)
						{
							m_cache.PushBack(pType);
							return;
						}

						pParentType = pParentType->GetBaseType();
					}
				});
		}

		~ClassModel()
		{ }

		Widgets::ModelIndex GetParent(const Widgets::ModelIndex& child) const override
		{
			return Widgets::ModelIndex();
		}

		Widgets::ModelIndex GetIndex(int row, int column, const Widgets::ModelIndex& parent) const override
		{
			if (parent.IsValid())
				return Widgets::ModelIndex();

			if(!m_cache.IsValidIndex(row))
				return Widgets::ModelIndex();

			return CreateIndex(row, column, m_cache[row]);
		}

		int GetRowCount(const Widgets::ModelIndex& parent) const override
		{
			if (parent.IsValid())
				return 0;

			return m_cache.GetSize();
		}

		int GetColumnCount(const Widgets::ModelIndex& parent) const override
		{
			return 1;
		}

		std::string GetData(const Widgets::ModelIndex& index) override
		{
			if (!index.IsValid())
				return "";

			int row = index.GetRow();
			if (!m_cache.IsValidIndex(row))
				return "";

			return m_cache[row]->GetName();
		}

		std::string GetHeaderData(int columnIndex)
		{
			if (columnIndex == 0)
				return "Class Name";

			return "";
		}

		const Systems::TypeDescriptor* GetType(const Widgets::ModelIndex& index)
		{
			if (!index.IsValid())
				return nullptr;

			return m_cache[index.GetRow()];
		}

	private:
		Core::Array<const Systems::TypeDescriptor*> m_cache;
	};

	ClassSelectionDialog::ClassSelectionDialog(const Core::Sid& baseClassSid)
		: Widgets::ModalWindow("Select class")
	{
		const int WIDTH = 500;
		const int HEIGHT = 500;

		Widgets::Layout* pLayout = new Widgets::Layout(Widgets::Layout::Vertical_Reverse, Widgets::Widget::STRETCH);
		AddWidget(pLayout);

		//ok/cancel button
		Widgets::Layout* pButtonLayout = new Widgets::Layout(Widgets::Layout::Horizontal, (Widgets::Widget::SIZE_STYLE)(Widgets::Widget::HSIZE_STRETCH | Widgets::Widget::VSIZE_FIT));
		pLayout->AddWidget(pButtonLayout);

		const int BUTTON_HEIGHT = 40;

		Widgets::Button* pOkButton = new Widgets::Button("Ok", WIDTH / 2, BUTTON_HEIGHT);
		pOkButton->OnClick([this]() { Internal_OnOk(); });
		pButtonLayout->AddWidget(pOkButton);

		//create cancel button
		Widgets::Button* pCancelButton = new Widgets::Button("Cancel", WIDTH / 2, BUTTON_HEIGHT);
		pCancelButton->OnClick([this]() { Internal_OnCancel(); });
		pButtonLayout->AddWidget(pCancelButton);

		// class table
		m_pClassView = new Widgets::TableView();
		m_pClassView->OnItemDoubleClick([this](const Widgets::ModelIndex& index) { Internal_OnOk(); });
		pLayout->AddWidget(m_pClassView);

		m_pClassModel = new ClassModel(baseClassSid);
		m_pClassView->SetModel(m_pClassModel);

		SetSize(Core::UInt2(WIDTH, HEIGHT));
		SetSizeStyle(Widgets::Widget::DEFAULT);
		SetPositionStyle(Widgets::Widget::HPOSITION_STYLE::CENTER, Widgets::Widget::VPOSITION_STYLE::MIDDLE);
	}

	ClassSelectionDialog::~ClassSelectionDialog()
	{ }

	void ClassSelectionDialog::Open()
	{
		Widgets::WidgetMgr::Get().OpenModalWindow(this);
	}

	void ClassSelectionDialog::Close() const
	{
		Widgets::WidgetMgr::Get().CloseModalWindow();
	}

	void ClassSelectionDialog::Internal_OnOk()
	{
		Widgets::SelectionModel* pSelection = m_pClassModel->GetSelectionModel();
		if (!pSelection)
		{
			Close();
			return;
		}

		const std::list<Widgets::SelectionRow>& selection = pSelection->GetSelectedRows();
		if (selection.empty())
		{
			Close();
			return;
		}

		const Widgets::SelectionRow& row = selection.back();
		const Systems::TypeDescriptor* pType = m_pClassModel->GetType(row.GetStartIndex());
		if (!pType)
		{
			Close();
			return;
		}

		m_onOk(pType->GetSid());
		Close();
	}

	void ClassSelectionDialog::Internal_OnCancel()
	{
		Close();
	}
}
