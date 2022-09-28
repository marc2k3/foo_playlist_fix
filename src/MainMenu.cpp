#include "stdafx.hpp"

namespace
{
	struct MenuItem
	{
		const GUID* guid;
		const pfc::string8 name, desc;
	};

	static const std::vector<MenuItem> menu_items =
	{
		{ &g_guid_main_menu_fix_active, "Fix active playlist", "Fix active playlist." },
		{ &g_guid_main_menu_fix_all, "Fix all playlists", "Fixes all playlists." },
		{ &g_guid_main_menu_check_active, "Check active playlist", "Generates a report but does not make any changes." },
		{ &g_guid_main_menu_check_all, "Check all playlists", "Generates a report but does not make any changes." },
	};

	class MainMenu : public mainmenu_commands
	{
	public:
		GUID get_command(uint32_t index) override
		{
			if (index >= menu_items.size()) uBugCheck();

			return *menu_items[index].guid;
		}

		GUID get_parent() override
		{
			return g_guid_main_menu_group;
		}

		bool get_description(uint32_t index, pfc::string_base& out) override
		{
			if (index >= menu_items.size()) uBugCheck();

			out = menu_items[index].desc;
			return true;
		}

		bool get_display(uint32_t index, pfc::string_base& out, uint32_t& flags) override
		{
			if (index >= menu_items.size()) uBugCheck();

			auto api = playlist_manager::get();
			if ((index == 0 || index == 2) && api->get_active_playlist() == SIZE_MAX) flags = mainmenu_commands::flag_disabled;
			if ((index == 1 || index == 3) && api->get_playlist_count() == 0) flags = mainmenu_commands::flag_disabled;
			get_name(index, out);
			return true;
		}

		uint32_t get_command_count() override
		{
			return static_cast<uint32_t>(menu_items.size());
		}

		void execute(uint32_t index, service_ptr_t<service_base> callback) override
		{
			if (index >= menu_items.size()) uBugCheck();

			const bool preview = index > 1;
			auto api = playlist_manager::get();
			g_report_items.clear();

			if (index == 0 || index == 2)
			{
				const size_t playlistIndex = api->get_active_playlist();
				if (playlistIndex == SIZE_MAX) return;

				if (!build_map()) return;
				fix(playlistIndex, preview);
			}
			else if (index == 1 || index == 3)
			{
				const size_t count = api->get_playlist_count();
				if (count == 0) return;

				if (!build_map()) return;
				for (size_t i = 0; i < count; ++i)
				{
					fix(i, preview);
				}
			}

			if (g_report_items.empty())
			{
				popup_message::g_show("No dead items found.", component_name);
			}
			else
			{
				CDialogReport dlg(preview);
				dlg.DoModal(core_api::get_main_window());
				g_report_items.clear();
			}

			m_map.clear();
		}

		void get_name(uint32_t index, pfc::string_base& out) override
		{
			if (index >= menu_items.size()) uBugCheck();
			out = menu_items[index].name;
		}

	private:
		bool build_map()
		{
			CDialogPreset dlg;
			if (dlg.DoModal(core_api::get_main_window()) == IDCANCEL) return false;
			titleformat_compiler::get()->compile_safe(m_obj, dlg.m_edit_text.c_str());

			m_map.clear();
			metadb_handle_list items;
			library_manager::get()->get_all_items(items);
			items.sort_by_format(m_obj, nullptr, 1);

			auto api = metadb_v2::get();
			api->queryMultiParallel_(items, [&](size_t idx, const metadb_v2::rec_t& rec)
				{
					pfc::string8 tf;
					api->formatTitle_v2(items[idx], rec, nullptr, tf, m_obj, nullptr);
					m_map.insert({ tf.get_ptr(), items[idx] });
				});

			return true;
		}

		bool is_file(const char* path)
		{
			try
			{
				return filesystem::g_exists(path, fb2k::noAbort);
			}
			catch (...) {}
			return false;
		}

		void fix(size_t playlistIndex, bool preview)
		{
			auto plman = playlist_manager::get();

			const size_t count = plman->playlist_get_item_count(playlistIndex);
			if (count == 0) return;

			pfc::string8 playlistName;
			plman->playlist_get_name(playlistIndex, playlistName);

			if (plman->playlist_lock_get_filter_mask(playlistIndex) & playlist_lock::filter_replace)
			{
				FB2K_console_formatter() << component_name << ": A playlist lock prevents replacing items on playlist named \"" << playlistName << "\"";
				return;
			}

			metadb_handle_list items;
			plman->playlist_get_all_items(playlistIndex, items);
			concurrency::concurrent_vector<pfc::string8> tfs(count);

			auto api = metadb_v2::get();
			api->queryMultiParallel_(items, [&](size_t idx, const metadb_v2::rec_t& rec)
				{
					api->formatTitle_v2(items[idx], rec, nullptr, tfs[idx], m_obj, nullptr);
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

					const auto it = m_map.find(tfs[i].get_ptr());
					if (it != m_map.end())
					{
						item.newPath = it->second->get_path();
						if (!preview)
						{
							plman->playlist_replace_item(playlistIndex, i, it->second);
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

		concurrency::concurrent_unordered_map<std::string, metadb_handle_ptr> m_map;
		titleformat_object_ptr m_obj;
	};

	static mainmenu_group_popup_factory g_main_menu_group(g_guid_main_menu_group, mainmenu_groups::file, mainmenu_commands::sort_priority_base, component_name);
	FB2K_SERVICE_FACTORY(MainMenu);
}
