#pragma once

struct ReportItem
{
	size_t playlistItemIndex{};
	pfc::string8 playlistName, deadPath, newPath;
};

static std::vector<ReportItem> g_report_items;

class CDialogReport : public CDialogImpl<CDialogReport>
{
public:
	CDialogReport(bool preview) : m_preview(preview) {}

	BEGIN_MSG_MAP_EX(CDialogReport)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
		END_MSG_MAP()

	enum { IDD = IDD_DIALOG_REPORT };

	BOOL OnInitDialog(CWindow, LPARAM)
	{
		if (m_preview)
		{
			SetWindowTextW(L"Playlist Fix (Preview, no changes were made)");
		}

		m_list_report.CreateInDialog(*this, IDC_LIST_REPORT);
		m_list_report.SetSelectionModeNone();
		const SIZE DPI = m_list_report.GetDPI();
		m_list_report.AddColumn("Playlist Name", MulDiv(200, DPI.cx, 96));
		m_list_report.AddColumn("Item Index", MulDiv(80, DPI.cx, 96));
		m_list_report.AddColumn("Dead Path", MulDiv(300, DPI.cx, 96));
		m_list_report.AddColumnAutoWidth("New Path");

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

		::SetWindowLongPtrW(m_list_report, GWL_EXSTYLE, 0);

		m_hooks.AddDialogWithControls(*this);
		CenterWindow();
		return TRUE;
	}

	void OnCancel(UINT, int nID, CWindow)
	{
		EndDialog(nID);
	}

private:
	CListControlSimple m_list_report;
	bool m_preview{};
	fb2k::CCoreDarkModeHooks m_hooks;
};
