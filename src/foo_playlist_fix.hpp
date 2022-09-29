#pragma once

static constexpr const char* component_name = "Playlist Fix";
static constexpr const char* component_home_page = "https://marc2k3.github.io/component/playlist-fix/";
static constexpr const char* component_default_pattern = "%codec%|%album artist%|%album%|%date%|%discnumber%|%totaldiscs%|%tracknumber%|%artist%|%title%";

static constexpr GUID g_guid_main_menu_group = { 0x134518f8, 0xd897, 0x46fb, { 0xb2, 0xe3, 0x82, 0x7a, 0x45, 0xae, 0x7a, 0xd4 } };
static constexpr GUID g_guid_main_menu_fix_active = { 0xb229e300, 0x73f9, 0x4429, { 0x9f, 0x7c, 0xff, 0x61, 0x17, 0x15, 0xf1, 0x56 } };
static constexpr GUID g_guid_main_menu_fix_all = { 0xb0e848ff, 0x877e, 0x4f7c, { 0xbd, 0x21, 0x8c, 0xb3, 0x15, 0xd9, 0xf1, 0x80 } };
static constexpr GUID g_guid_main_menu_check_active = { 0xcc0e6885, 0xa2dc, 0x4390, { 0xa9, 0x93, 0x4a, 0x7a, 0xf4, 0x9d, 0xda, 0x63 } };
static constexpr GUID g_guid_main_menu_check_all = { 0x120edd18, 0x871b, 0x493c, { 0xb8, 0xb1, 0xf6, 0x7e, 0xe, 0xb3, 0x7d, 0x90 } };
static constexpr GUID g_guid_preferences_page = { 0x921e2802, 0x3da9, 0x4f10, { 0xbb, 0x7d, 0x26, 0x66, 0x52, 0xd7, 0xf4, 0x55 } };

struct ListItem
{
	std::string name, pattern;
};

using ListItems = std::vector<ListItem>;

ListItems get_patterns();
std::string get_pattern_name();
void set_patterns(const ListItems& patterns);
void set_pattern_name(const std::string& name);
