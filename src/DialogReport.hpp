#pragma once

struct ReportItem
{
	size_t playlistItemIndex{};
	pfc::string8 playlistName, deadPath, newPath;
};

static std::vector<ReportItem> g_report_items;

static const CDialogResizeHelper::Param resize_data[] =
{
	{ IDC_LIST_REPORT, 0, 0, 1, 1 },
	{ IDCANCEL, 1, 1, 1, 1 },
};

static const CRect resize_min_max(760, 440, 0, 0);

class CDialogReport : public CDialogImpl<CDialogReport>, public cfg_window_placement_v2
{
public:
	CDialogReport(bool preview) : m_preview(preview), m_resizer(resize_data, resize_min_max), cfg_window_placement_v2(guids::window_placement) {}

	BEGIN_MSG_MAP_EX(CDialogReport)
		CHAIN_MSG_MAP_MEMBER(m_resizer)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
	END_MSG_MAP()

	enum { IDD = IDD_DIALOG_REPORT };

	BOOL OnInitDialog(CWindow, LPARAM)
	{
		SetIcon(ui_control::get()->get_main_icon());

		if (m_preview)
		{
			SetWindowTextW(L"Playlist Fix (Preview, no changes were made)");
		}

		m_list_report.CreateInDialog(*this, IDC_LIST_REPORT);
		m_list_report.SetWindowLongPtrW(GWL_EXSTYLE, 0L);
		m_list_report.SetSelectionModeNone();

		const SIZE DPI = m_list_report.GetDPI();
		m_list_report.AddColumn("Playlist Name", MulDiv(200, DPI.cx, 96));
		m_list_report.AddColumn("Item Index", MulDiv(80, DPI.cx, 96));
		m_list_report.AddColumnAutoWidth("Dead Path");
		m_list_report.AddColumnAutoWidth("New Path");
		m_list_report.AddColumn("", 5);

		const size_t count = g_report_items.size();
		m_list_report.SetItemCount(count);

		for (size_t i{}; i < count; ++i)
		{
			auto& item = g_report_items[i];
			m_list_report.SetItemText(i, 0, item.playlistName);
			m_list_report.SetItemText(i, 1, pfc::format_uint(item.playlistItemIndex));
			m_list_report.SetItemText(i, 2, item.deadPath);
			m_list_report.SetItemText(i, 3, item.newPath);
		}

		m_hooks.AddDialogWithControls(*this);
		apply_to_window(*this, false);
		return TRUE;
	}

	void OnCancel(uint32_t, int nID, CWindow)
	{
		read_from_window(*this);
		EndDialog(nID);
	}

private:
	CDialogResizeHelper m_resizer;
	CListControlSimple m_list_report;
	bool m_preview{};
	fb2k::CCoreDarkModeHooks m_hooks;
};
