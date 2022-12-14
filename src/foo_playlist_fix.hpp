#pragma once

namespace Component
{
	static constexpr wil::zstring_view name = "Playlist Fix";
	static constexpr wil::zstring_view home_page = "https://marc2k3.github.io/component/playlist-fix/";
	static constexpr wil::zstring_view default_pattern = "%codec%|%album artist%|%album%|%date%|%discnumber%|%totaldiscs%|%tracknumber%|%artist%|%title%";
}

namespace guids
{
	static constexpr GUID main_menu_group = { 0x134518f8, 0xd897, 0x46fb, { 0xb2, 0xe3, 0x82, 0x7a, 0x45, 0xae, 0x7a, 0xd4 } };
	static constexpr GUID main_menu_fix_active = { 0xb229e300, 0x73f9, 0x4429, { 0x9f, 0x7c, 0xff, 0x61, 0x17, 0x15, 0xf1, 0x56 } };
	static constexpr GUID main_menu_fix_all = { 0xb0e848ff, 0x877e, 0x4f7c, { 0xbd, 0x21, 0x8c, 0xb3, 0x15, 0xd9, 0xf1, 0x80 } };
	static constexpr GUID main_menu_check_active = { 0xcc0e6885, 0xa2dc, 0x4390, { 0xa9, 0x93, 0x4a, 0x7a, 0xf4, 0x9d, 0xda, 0x63 } };
	static constexpr GUID main_menu_check_all = { 0x120edd18, 0x871b, 0x493c, { 0xb8, 0xb1, 0xf6, 0x7e, 0xe, 0xb3, 0x7d, 0x90 } };
	static constexpr GUID preferences_page = { 0x921e2802, 0x3da9, 0x4f10, { 0xbb, 0x7d, 0x26, 0x66, 0x52, 0xd7, 0xf4, 0x55 } };
	static constexpr GUID window_placement = { 0xf6cec1c9, 0xd109, 0x4569, { 0x84, 0xf5, 0xd0, 0xd, 0x2, 0x83, 0x58, 0xf6 } };
}
