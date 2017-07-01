#include <new>
#include <cstring>
#include <cstdlib>

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
    wxFileName file_name(wxGetHomeDir(), wxEmptyString);
    file_name.AppendDir(".config");
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

void *spek_platform_aligned_alloc(size_t alignment, size_t size)
{
    void *ptr;
#ifdef OS_WIN
    ptr = _aligned_malloc(size, alignment);
    if (!ptr)
        throw std::bad_alloc();
#else
    if (posix_memalign(&ptr, alignment, size) != 0)
        throw std::bad_alloc();
#endif
    return ptr;
}

void spek_platform_aligned_free(void *mem)
{
#ifdef OS_WIN
     _aligned_free(mem);
#else
    free(mem);
#endif
}
