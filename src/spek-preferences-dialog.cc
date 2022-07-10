#include "spek-platform.h"
#include "spek-preferences.h"

#include "spek-preferences-dialog.h"

// List all languages with a decent (e.g. 80%) number of translated
// strings. Don't translate language names. Keep the first line intact.
static const char *available_languages[] =
{
    "", "",
    "bs", "Bosanski",
    "ca", "Català",
    "cs", "Čeština",
    "da", "Dansk",
    "de", "Deutsch",
    "el", "Ελληνικά",
    "en", "English",
    "eo", "Esperanto",
    "es", "Español",
    "fi", "Suomi",
    "fr", "Français",
    "gl", "Galego",
    "he", "עברית",
    "hr", "Hrvatski",
    "hu", "Magyar",
    "id", "Bahasa Indonesia",
    "it", "Italiano",
    "ja", "日本語",
    "ko", "한국어",
    "lv", "Latviešu",
    "nb", "Norsk (bokmål)",
    "nl", "Nederlands",
    "nn", "Norsk (nynorsk)",
    "pl", "Polski",
    "pt_BR", "Português brasileiro",
    "ru", "Русский",
    "sk", "Slovenčina",
    "sr@latin", "Srpski",
    "sv", "Svenska",
    "th", "ไทย",
    "tr", "Türkçe",
    "uk", "Українська",
    "vi", "Tiếng Việt",
    "zh_CN", "中文(简体)",
    "zh_TW", "中文(台灣)",
    NULL
};

#define ID_LANGUAGE (wxID_HIGHEST + 1)
#define ID_CHECK (wxID_HIGHEST + 2)

BEGIN_EVENT_TABLE(SpekPreferencesDialog, wxDialog)
    EVT_CHOICE(ID_LANGUAGE, SpekPreferencesDialog::on_language)
    EVT_CHECKBOX(ID_CHECK, SpekPreferencesDialog::on_check)
END_EVENT_TABLE()

SpekPreferencesDialog::SpekPreferencesDialog(wxWindow *parent) :
    wxDialog(parent, -1, _("Preferences"))
{
    for (int i = 0; available_languages[i]; ++i) {
        this->languages.Add(wxString::FromUTF8(available_languages[i]));
    }
    this->languages[1] = _("(system default)");

    wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    wxSizer *inner_sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(inner_sizer, 1, wxALL, 12);

    // TRANSLATORS: The name of a section in the Preferences dialog.
    wxStaticText *general_label = new wxStaticText(this, -1, _("General"));
    wxFont font = general_label->GetFont();
    font.SetWeight(wxFONTWEIGHT_BOLD);
    general_label->SetFont(font);
    inner_sizer->Add(general_label);

    if (spek_platform_can_change_language()) {
        wxSizer *language_sizer = new wxBoxSizer(wxHORIZONTAL);
        inner_sizer->Add(language_sizer, 0, wxLEFT | wxTOP, 12);
        wxStaticText *language_label = new wxStaticText(this, -1, _("Language:"));
        language_sizer->Add(language_label, 0, wxALIGN_CENTER_VERTICAL);

        wxChoice *language_choice = new wxChoice(this, ID_LANGUAGE);
        language_sizer->Add(language_choice, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 12);
        int active_index = 0;
        wxString active_language = SpekPreferences::get().get_language();
        for (unsigned int i = 0; i < this->languages.GetCount(); i += 2) {
            language_choice->Append(this->languages[i + 1]);
            if (this->languages[i] == active_language) {
                active_index = i / 2;
            }
        }
        language_choice->SetSelection(active_index);
    }

    wxCheckBox *check_update = new wxCheckBox(this, ID_CHECK, _("Check for &updates"));
    inner_sizer->Add(check_update, 0 ,wxLEFT | wxTOP, 12);
    check_update->SetValue(SpekPreferences::get().get_check_update());

    sizer->Add(CreateButtonSizer(wxOK), 0, wxALIGN_RIGHT | wxBOTTOM | wxRIGHT, 12);
    sizer->SetSizeHints(this);
    SetSizer(sizer);
}

void SpekPreferencesDialog::on_language(wxCommandEvent& event)
{
    SpekPreferences::get().set_language(this->languages[event.GetSelection() * 2]);
}

void SpekPreferencesDialog::on_check(wxCommandEvent& event)
{
    SpekPreferences::get().set_check_update(event.IsChecked());
}
