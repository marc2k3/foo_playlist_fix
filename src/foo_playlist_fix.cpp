#define _WIN32_WINNT _WIN32_WINNT_WIN7
#define WINVER _WIN32_WINNT_WIN7

#include <vector>
#include <concurrent_unordered_map.h>
#include <concurrent_vector.h>
#include <helpers/foobar2000+atl.h>
#include <libPPUI/CListControlSimple.h>
#include <SDK/coreDarkMode.h>

#include "resource.hpp"

#pragma comment(lib, "shlwapi.lib")

namespace
{
	static constexpr const char* component_name = "Playlist Fix";

	DECLARE_COMPONENT_VERSION(
		component_name,
		"1.0.0-Beta.3",
		"Copyright (C) 2022 marc2003\n\n"
		"Build: " __TIME__ ", " __DATE__
	);

	VALIDATE_COMPONENT_FILENAME("foo_playlist_fix.dll");

	static constexpr GUID g_guid_main_menu_group = { 0x134518f8, 0xd897, 0x46fb, { 0xb2, 0xe3, 0x82, 0x7a, 0x45, 0xae, 0x7a, 0xd4 } };
	static constexpr GUID g_guid_main_menu_revive_active = { 0xb229e300, 0x73f9, 0x4429, { 0x9f, 0x7c, 0xff, 0x61, 0x17, 0x15, 0xf1, 0x56 } };
	static constexpr GUID g_guid_main_menu_revive_all = { 0xb0e848ff, 0x877e, 0x4f7c, { 0xbd, 0x21, 0x8c, 0xb3, 0x15, 0xd9, 0xf1, 0x80 } };
	static constexpr GUID g_guid_main_menu_revive_active_check = { 0xcc0e6885, 0xa2dc, 0x4390, { 0xa9, 0x93, 0x4a, 0x7a, 0xf4, 0x9d, 0xda, 0x63 } };
	static constexpr GUID g_guid_main_menu_revive_all_check = { 0x120edd18, 0x871b, 0x493c, { 0xb8, 0xb1, 0xf6, 0x7e, 0xe, 0xb3, 0x7d, 0x90 } };
	static constexpr GUID g_guid_config_branch = { 0xa546dafe, 0x513c, 0x435b, { 0xbe, 0x4, 0xf2, 0x5b, 0x1d, 0x52, 0x6a, 0x23 } };
	static constexpr GUID g_guid_config_item = { 0x8d2f71e1, 0x73ce, 0x435d, { 0xac, 0x99, 0x1a, 0xe0, 0xa6, 0x25, 0x0, 0xe4 } };

	static advconfig_branch_factory g_config_branch(component_name, g_guid_config_branch, advconfig_branch::guid_branch_tools, 0);
	static advconfig_string_factory g_config_item("Pattern to match", g_guid_config_item, g_guid_config_branch, 0.0, "%codec%|%album artist%|%album%|%date%|%discnumber%|%totaldiscs%|%tracknumber%|%artist%|%title%");

	concurrency::concurrent_unordered_map<std::string, metadb_handle_ptr> g_map;

	struct ReportItem
	{
		size_t playlistItemIndex{};
		pfc::string8 playlistName, deadPath, newPath;
	};

	std::vector<ReportItem> g_report_items;

	struct MenuItem
	{
		const GUID* guid;
		const pfc::string8 name, desc;
	};

	const std::vector<MenuItem> g_menu_items =
	{
		{ &g_guid_main_menu_revive_active, "Fix active playlist", "Fix active playlist." },
		{ &g_guid_main_menu_revive_all, "Fix all playlists", "Fixes all playlists." },
		{ &g_guid_main_menu_revive_active_check, "Check active playlist", "Generates a report but does not make any changes." },
		{ &g_guid_main_menu_revive_all_check, "Check all playlists", "Generates a report but does not make any changes." },
	};

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
				pfc::setWindowText(m_hWnd, "Playlist Fix (Preview, no changes were made)");
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

	class MainMenu : public mainmenu_commands
	{
	public:
		GUID get_command(uint32_t index) override
		{
			if (index < g_menu_items.size())
			{
				return *g_menu_items[index].guid;
			}
			uBugCheck();
		}

		GUID get_parent() override
		{
			return g_guid_main_menu_group;
		}

		bool get_description(uint32_t index, pfc::string_base& out) override
		{
			if (index < g_menu_items.size())
			{
				out = g_menu_items[index].desc;
			}
			return true;
		}

		bool get_display(uint32_t index, pfc::string_base& out, uint32_t& flags) override
		{
			auto api = playlist_manager::get();
			if ((index == 0 || index == 2) && api->get_active_playlist() == SIZE_MAX) flags = mainmenu_commands::flag_disabled;
			if ((index == 1 || index == 3) && api->get_playlist_count() == 0) flags = mainmenu_commands::flag_disabled;
			get_name(index, out);
			return true;
		}

		uint32_t get_command_count() override
		{
			return static_cast<uint32_t>(g_menu_items.size());
		}

		void execute(uint32_t index, service_ptr_t<service_base> callback) override
		{
			auto api = playlist_manager::get();
			g_report_items.clear();

			if (index == 0 || index == 2)
			{
				const size_t playlistIndex = api->get_active_playlist();
				if (playlistIndex == SIZE_MAX) return;

				build_map();
				fix(playlistIndex, index == 0);
			}
			else if (index == 1 || index == 3)
			{
				const size_t count = api->get_playlist_count();
				if (count == 0) return;

				build_map();
				for (size_t i = 0; i < count; ++i)
				{
					fix(i, index == 1);
				}
			}
			else
			{
				uBugCheck();
			}

			if (g_report_items.empty())
			{
				popup_message::g_show("No dead items found.", component_name);
			}
			else
			{
				CDialogReport dlg(index == 2 || index == 3);
				dlg.DoModal(core_api::get_main_window());
				g_report_items.clear();
			}

			g_map.clear();
		}

		void get_name(uint32_t index, pfc::string_base& out) override
		{
			if (index < g_menu_items.size())
			{
				out = g_menu_items[index].name;
			}
			else
			{
				uBugCheck();
			}
		}

	private:
		bool is_file(const char* path)
		{
			try
			{
				return filesystem::g_exists(path, fb2k::noAbort);
			}
			catch (...) {}
			return false;
		}

		void build_map()
		{
			pfc::string8 pattern;
			g_config_item.get(pattern);
			titleformat_compiler::get()->compile_safe(m_obj, pattern);

			g_map.clear();
			metadb_handle_list items;
			library_manager::get()->get_all_items(items);
			items.sort_by_format(m_obj, nullptr, 1);

			metadb_v2::get()->queryMultiParallel_(items, [&](size_t idx, const metadb_v2::rec_t& rec)
				{
					pfc::string8 tf;
					metadb_v2::get()->formatTitle_v2(items[idx], rec, nullptr, tf, m_obj, nullptr);
					g_map.insert({ tf.get_ptr(), items[idx] });
				});
		}

		void fix(size_t playlistIndex, bool fix)
		{
			auto api = playlist_manager::get();

			const size_t count = api->playlist_get_item_count(playlistIndex);
			if (count == 0) return;
			
			pfc::string8 playlistName;
			api->playlist_get_name(playlistIndex, playlistName);

			if (api->playlist_lock_get_filter_mask(playlistIndex) & playlist_lock::filter_replace)
			{
				FB2K_console_formatter() << component_name << ": A playlist lock prevents replacing items on playlist named \"" << playlistName << "\"";
				return;
			}

			metadb_handle_list items;
			api->playlist_get_all_items(playlistIndex, items);
			concurrency::concurrent_vector<pfc::string8> tfs(count);
			
			metadb_v2::get()->queryMultiParallel_(items, [&](size_t idx, const metadb_v2::rec_t& rec)
				{
					metadb_v2::get()->formatTitle_v2(items[idx], rec, nullptr, tfs[idx], m_obj, nullptr);
				});

			for (size_t i{}; i < count; ++i)
			{
				const pfc::string8 path = items[i]->get_path();
				if (path.startsWith("file://") && !is_file(path))
				{
					ReportItem item;
					item.playlistName = playlistName;
					item.playlistItemIndex = i + 1;
					item.deadPath = path;

					const auto it = g_map.find(tfs[i].get_ptr());
					if (it != g_map.end())
					{
						item.newPath = it->second->get_path();
						if (fix)
						{
							api->playlist_replace_item(playlistIndex, i, it->second);
						}
					}
					else
					{
						item.newPath = "N/A";
					}

					g_report_items.emplace_back(item);
				}
			}
		}

		titleformat_object_ptr m_obj;
	};

	static mainmenu_group_popup_factory g_main_menu_group(g_guid_main_menu_group, mainmenu_groups::file, mainmenu_commands::sort_priority_base, component_name);
	FB2K_SERVICE_FACTORY(MainMenu);
};
