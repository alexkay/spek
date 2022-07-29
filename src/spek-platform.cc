#include <cstring>

#ifdef OS_OSX
#include <ApplicationServices/ApplicationServices.h>
#endif

#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/utils.h>

#include "spek-platform.h"

void spek_platform_init()
{
#ifdef OS_OSX
    ProcessSerialNumber PSN;
    GetCurrentProcess(&PSN);
    TransformProcessType(&PSN, kProcessTransformToForegroundApplication);
#endif
}

wxString spek_platform_config_path(const wxString& app_name)
{
#ifdef OS_WIN
    wxFileName file_name(wxStandardPaths::Get().GetUserConfigDir(), wxEmptyString);
#else
    wxFileName file_name;
    wxString xdg_config_home;
    if (wxGetEnv("XDG_CONFIG_HOME", &xdg_config_home) && !xdg_config_home.IsEmpty()) {
        file_name = wxFileName(xdg_config_home, wxEmptyString);
    } else {
        file_name = wxFileName(wxGetHomeDir(), wxEmptyString);
        file_name.AppendDir(".config");
    }
#endif
    file_name.AppendDir(app_name);
    file_name.Mkdir(0755, wxPATH_MKDIR_FULL);
    file_name.SetFullName("preferences");
    return file_name.GetFullPath();
}

bool spek_platform_can_change_language()
{
#ifdef OS_UNIX
    return false;
#else
    return true;
#endif
}

double spek_platform_font_scale()
{
#ifdef OS_OSX
    return 1.3;
#else
    return 1.0;
#endif
}
