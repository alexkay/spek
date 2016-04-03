#pragma once

#include <wx/string.h>

// Platform-specific initialisation code.
void spek_platform_init();

// Not quite XDG-compatible, but close enough.
wxString spek_platform_config_path(const wxString& app_name);

// Setting non-default locale under GTK+ is tricky (see e.g. how FileZilla does it). We will
// just disable the language setting for GTK+ users and will always use the system locale.
bool spek_platform_can_change_language();

// Fonts are smaller on OSX.
double spek_platform_font_scale();
