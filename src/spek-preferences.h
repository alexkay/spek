#pragma once

#include <wx/fileconf.h>
#include <wx/intl.h>

class SpekPreferences
{
public:
    static SpekPreferences& get();

    void init();
    bool get_check_update();
    void set_check_update(bool value);
    long get_last_update();
    void set_last_update(long value);
    wxString get_language();
    void set_language(const wxString& value);

private:
    SpekPreferences();
    SpekPreferences(const SpekPreferences&);
    void operator=(const SpekPreferences&);

    wxLocale *locale;
    wxFileConfig *config;
};
