#pragma once

class PresetList : public CListControlComplete
{
public:
	BEGIN_MSG_MAP_EX(PresetList)
		CHAIN_MSG_MAP(CListControlComplete);
		MSG_WM_CREATE(OnCreate)
		MSG_WM_CONTEXTMENU(OnContextMenu)
	END_MSG_MAP()

	bool AllowScrollbar(bool) const override
	{
		return true;
	}

	bool CanSelectItem(size_t row) const override
	{
		return row < m_items.size();
	}

	bool GetCellTypeSupported() const override
	{
		return true;
	}

	bool GetSubItemText(size_t row, size_t column, pfc::string_base& out) const override
	{
		if (row < m_items.size())
		{
			if (column == 0) out = m_items[row].name;
			if (column == 1) out = m_items[row].pattern;
			return true;
		}
		else if (column == 0)
		{
			out = "+ Add new pattern";
			return true;
		}
		return false;
	}

	bool TableEdit_IsColumnEditable(size_t) const override
	{
		return true;
	}

	cellType_t GetCellType(size_t row, size_t column) const override
	{
		if (row == m_items.size())
		{
			if (column == 0) return &PFC_SINGLETON(CListCell_Button);
			return nullptr;
		}

		return &PFC_SINGLETON(CListCell_Text);
	}

	int OnCreate(LPCREATESTRUCT)
	{
		InitializeHeaderCtrl(HDS_NOSIZING);

		AddColumn("Name", MulDiv(150, m_dpi.cx, 96));
		AddColumnAutoWidth("Pattern");

		init_data();
		return 0;
	}

	size_t GetItemCount() const override
	{
		return m_items.size() + 1;
	}

	size_t GetSubItemSpan(size_t row, size_t column) const override
	{
		if (row == m_items.size() && column == 0) return 2;
		return 1;
	}

	void ExecuteDefaultAction(size_t row) override
	{
		if (row == m_items.size()) add_new_item();
	}

	void OnContextMenu(CWindow, CPoint point)
	{
		if (m_items.empty()) return;

		CMenu menu = CreatePopupMenu();
		menu.AppendMenuW(MF_STRING, ID_SELECTALL, L"Select all\tCtrl+A");
		menu.AppendMenuW(MF_STRING, ID_SELECTNONE, L"Select none");
		menu.AppendMenuW(MF_STRING, ID_INVERTSEL, L"Invert selection");
		menu.AppendMenuW(MF_SEPARATOR);
		menu.AppendMenuW(GetSelectedCount() ? MF_STRING : MF_GRAYED, ID_REMOVE, L"Remove\tDel");

		point = this->GetContextMenuPoint(point);
		const int idx = TrackPopupMenuEx(menu, TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, point.x, point.y, m_hWnd, nullptr);
		switch (idx)
		{
		case ID_SELECTALL:
			this->SelectAll();
			break;
		case ID_SELECTNONE:
			this->SelectNone();
			break;
		case ID_INVERTSEL:
			this->SetSelection(pfc::bit_array_true(), pfc::bit_array_not(this->GetSelectionMask()));
			break;
		case ID_REMOVE:
			RequestRemoveSelection();
			break;
		}
	}

	void OnItemsRemoved(const pfc::bit_array& mask, size_t oldCount) override
	{
		__super::OnItemsRemoved(mask, oldCount);
		set_patterns(m_items);
	}

	void OnSubItemClicked(size_t row, size_t column, CPoint) override
	{
		if (row < m_items.size())
		{
			TableEdit_Start(row, column);
		}
		else
		{
			add_new_item();
		}
	}

	void RequestRemoveSelection() override
	{
		const pfc::bit_array_bittable mask = GetSelectionMask();
		const size_t old_count = GetItemCount();
		pfc::remove_mask_t(m_items, mask);
		this->OnItemsRemoved(mask, old_count);
	}

	void RequestReorder(size_t const*, size_t) override {}

	void TableEdit_SetField(size_t row, size_t column, const char* value) override
	{
		if (column == 0) m_items[row].name = value;
		else if (column == 1) m_items[row].pattern = value;
		ReloadItem(row);
		set_patterns(m_items);
	}

	void add_new_item()
	{
		SelectNone();
		m_items.emplace_back(ListItem("New pattern", ""));
		OnItemsInserted(m_items.size(), 1, true);
		set_patterns(m_items);
	}

	void init_data()
	{
		m_items = get_patterns();
	}

	void init_default()
	{
		m_items.clear();
		m_items.emplace_back(ListItem("Default", Component::default_pattern.data()));
		set_patterns(m_items);
		ReloadData();
	}

	ListItems m_items;
};
