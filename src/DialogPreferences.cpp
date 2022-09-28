#include "stdafx.hpp"

namespace
{
	class CDialogPreferences : public CDialogImpl<CDialogPreferences>, public preferences_page_instance
	{
	public:
		CDialogPreferences(preferences_page_callback::ptr callback) : m_callback(callback)
		{
			CSeparator::Register();
		}

		BEGIN_MSG_MAP_EX(CDialogPreferences)
			MSG_WM_INITDIALOG(OnInitDialog)
		END_MSG_MAP()

		enum { IDD = IDD_DIALOG_PREFERENCES };

		BOOL OnInitDialog(CWindow, LPARAM)
		{
			CreatePreferencesHeaderFont(m_font, *this);
			GetDlgItem(IDC_HEADER).SetFont(m_font.m_hFont);

			m_list.CreateInDialog(*this, IDC_LIST_PREFERENCES);
			m_hooks.AddDialogWithControls(*this);
			return FALSE;
		}

		uint32_t get_state() override
		{
			return preferences_state::resettable | preferences_state::dark_mode_supported;
		}

		void apply() override {}

		void reset() override
		{
			m_list.init_default();
		}

	private:
		CFont m_font;
		ListItems m_data;
		PreferencesList m_list;
		fb2k::CCoreDarkModeHooks m_hooks;
		preferences_page_callback::ptr m_callback;
	};

	class PreferencesPageImpl : public preferences_page_impl<CDialogPreferences>
	{
	public:
		GUID get_guid() override
		{
			return g_guid_preferences_page;
		}

		GUID get_parent_guid() override
		{
			return preferences_page::guid_tools;
		}

		bool get_help_url(pfc::string_base& out) override
		{
			out = component_home_page;
			return true;
		}

		const char* get_name() override
		{
			return component_name;
		}
	};

	FB2K_SERVICE_FACTORY(PreferencesPageImpl);
}
