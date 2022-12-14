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

		uint32_t get_state() final
		{
			return preferences_state::resettable | preferences_state::dark_mode_supported;
		}

		void apply() final {}

		void reset() final
		{
			m_list.init_default();
		}

	private:
		CFont m_font;
		PresetList m_list;
		fb2k::CCoreDarkModeHooks m_hooks;
		preferences_page_callback::ptr m_callback;
	};

	class PreferencesPageImpl : public preferences_page_impl<CDialogPreferences>
	{
	public:
		GUID get_guid() final
		{
			return guids::preferences_page;
		}

		GUID get_parent_guid() final
		{
			return preferences_page::guid_tools;
		}

		bool get_help_url(pfc::string_base& out) final
		{
			out = Component::home_page;
			return true;
		}

		const char* get_name() final
		{
			return Component::name.data();
		}
	};

	FB2K_SERVICE_FACTORY(PreferencesPageImpl);
}
