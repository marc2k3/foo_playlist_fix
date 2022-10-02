#pragma once

class CDialogPreset : public CDialogImpl<CDialogPreset>
{
public:
	BEGIN_MSG_MAP_EX(CDialogPreset)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_RANGE_HANDLER_EX(IDOK, IDCANCEL, OnCloseCmd)
		COMMAND_HANDLER_EX(IDC_COMBO_PATTERN, CBN_SELENDOK, OnPatternChange)
	END_MSG_MAP()

	enum { IDD = IDD_DIALOG_PRESET };

	BOOL OnInitDialog(CWindow, LPARAM)
	{
		m_items = get_patterns();

		m_combo_pattern = GetDlgItem(IDC_COMBO_PATTERN);
		m_edit_pattern = GetDlgItem(IDC_EDIT_PATTERN);

		for (auto&& item : m_items)
		{
			m_combo_pattern.AddString(pfc::wideFromUTF8(item.name.c_str()));
		}

		int sel = 0;
		const auto it = std::find_if(m_items.begin(), m_items.end(), [name = get_pattern_name()](const ListItem& item) { return item.name == name; });
		if (it != m_items.end())
		{
			sel = static_cast<int>(std::distance(m_items.begin(), it));
		}

		m_combo_pattern.SetCurSel(sel);
		m_edit_text = m_items[sel].pattern;
		pfc::setWindowText(m_edit_pattern, m_edit_text.c_str());

		m_hooks.AddDialogWithControls(*this);
		CenterWindow();
		return TRUE;
	}

	void OnCloseCmd(UINT, int nID, CWindow)
	{
		if (nID == IDOK)
		{
			const int sel = m_combo_pattern.GetCurSel();
			set_pattern_name(m_items[sel].name);
		}
		EndDialog(nID);
	}

	void OnPatternChange(UINT, int, CWindow)
	{
		const int sel = m_combo_pattern.GetCurSel();
		m_edit_text = m_items[sel].pattern;
		pfc::setWindowText(m_edit_pattern, m_edit_text.c_str());
	}

	CComboBox m_combo_pattern;
	CEdit m_edit_pattern;
	ListItems m_items;
	fb2k::CCoreDarkModeHooks m_hooks;
	std::string m_edit_text;
};
