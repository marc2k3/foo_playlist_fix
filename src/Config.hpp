#pragma once

struct ListItem
{
	std::string name, pattern;
};

using ListItems = std::vector<ListItem>;

struct ReportItem
{
	size_t playlistItemIndex{};
	pfc::string8 playlistName, deadPath, newPath;
};

static std::vector<ReportItem> g_report_items;

ListItems get_patterns();
std::string get_pattern_name();
void set_patterns(const ListItems& patterns);
void set_pattern_name(wil::zstring_view name);
