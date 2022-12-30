#include "stdafx.hpp"

namespace Component
{
	DECLARE_COMPONENT_VERSION(
		name,
		"1.1.4",
		"Copyright (C) 2022-2023 marc2003\n\n"
		"Build: " __TIME__ ", " __DATE__
	);

	VALIDATE_COMPONENT_FILENAME("foo_playlist_fix.dll");
}

cfg_window_placement_v2 g_window_placement(guids::window_placement);

ListItems get_patterns()
{
	ListItems items;

	const std::string str = fb2k::configStore::get()->getConfigString("foo_playlist_fix.patterns", "[]")->c_str();
	json patterns = json::parse(str, nullptr, false);
	if (patterns.is_array())
	{
		for (auto&& pattern : patterns)
		{
			if (pattern.is_array() && pattern.size() == 2 && pattern[0].is_string() && pattern[1].is_string())
			{
				const std::string name = pattern[0].get<std::string>();
				const std::string value = pattern[1].get<std::string>();
				if (!name.empty() && !value.empty())
				{
					items.emplace_back(ListItem(name, value));
				}
			}
		}
	}

	if (items.empty())
	{
		items.emplace_back(ListItem("Default", Component::default_pattern.data()));
		set_patterns(items);
	}
	return items;
}

std::string get_pattern_name()
{
	return fb2k::configStore::get()->getConfigString("foo_playlist_fix.pattern_name", "Default")->c_str();
}

void set_patterns(const ListItems& patterns)
{
	json j = json::array();
	for (const auto& [name, pattern] : patterns)
	{
		if (!name.empty() && !pattern.empty())
		{
			json a = json::array({ name, pattern });
			j.emplace_back(a);
		}
	}

	fb2k::configStore::get()->setConfigString("foo_playlist_fix.patterns", j.dump().c_str());
}

void set_pattern_name(wil::zstring_view name)
{
	fb2k::configStore::get()->setConfigString("foo_playlist_fix.pattern_name", name.data());
}
