#include "stdafx.hpp"

namespace
{
	DECLARE_COMPONENT_VERSION(
		component_name,
		"1.1.0",
		"Copyright (C) 2022 marc2003\n\n"
		"Build: " __TIME__ ", " __DATE__
	);

	VALIDATE_COMPONENT_FILENAME("foo_playlist_fix.dll");
}

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
		items.emplace_back(ListItem("Default", component_default_pattern));
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

void set_pattern_name(const std::string& name)
{
	fb2k::configStore::get()->setConfigString("foo_playlist_fix.pattern_name", name.c_str());
}
