#define _WIN32_WINNT _WIN32_WINNT_WIN7
#define WINVER _WIN32_WINNT_WIN7

#include <algorithm>
#include <vector>
#include <concurrent_unordered_map.h>
#include <concurrent_vector.h>

#define JSON_USE_IMPLICIT_CONVERSIONS 0
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <helpers/foobar2000+atl.h>
#include <helpers/atl-misc.h>
#include <helpers/window_placement_helper.h>
#include <libPPUI/CDialogResizeHelper.h>
#include <libPPUI/CListControlComplete.h>
#include <libPPUI/CListControl-Cells.h>
#include <libPPUI/CListControlSimple.h>
#include <libPPUI/Controls.h>
#include <SDK/coreDarkMode.h>
#include <pfc/string-conv-lite.h>

#include "resource.hpp"
#include "foo_playlist_fix.hpp"
#include "DialogPreset.hpp"
#include "DialogReport.hpp"
#include "PreferencesList.hpp"

#pragma comment(lib, "shlwapi.lib")
