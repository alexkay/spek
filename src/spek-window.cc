#include <wx/aboutdlg.h>
#include <wx/artprov.h>
#include <wx/dnd.h>
#include <wx/filename.h>
#include <wx/protocol/http.h>
#include <wx/sstream.h>

// WX on WIN doesn't like it when pthread.h is included first.
#include <pthread.h>

#include <spek-utils.h>

#include "spek-artwork.h"
#include "spek-preferences-dialog.h"
#include "spek-preferences.h"
#include "spek-spectrogram.h"

#include "spek-window.h"

DECLARE_EVENT_TYPE(SPEK_NOTIFY_EVENT, -1)
DEFINE_EVENT_TYPE(SPEK_NOTIFY_EVENT)

BEGIN_EVENT_TABLE(SpekWindow, wxFrame)
    EVT_MENU(wxID_OPEN, SpekWindow::on_open)
    EVT_MENU(wxID_SAVE, SpekWindow::on_save)
    EVT_MENU(wxID_EXIT, SpekWindow::on_exit)
    EVT_MENU(wxID_PREFERENCES, SpekWindow::on_preferences)
    EVT_MENU(wxID_HELP, SpekWindow::on_help)
    EVT_MENU(wxID_ABOUT, SpekWindow::on_about)
    EVT_COMMAND(-1, SPEK_NOTIFY_EVENT, SpekWindow::on_notify)
END_EVENT_TABLE()

// Forward declarations.
static void * check_version(void *);

class SpekDropTarget : public wxFileDropTarget
{
public:
    SpekDropTarget(SpekWindow *window) : wxFileDropTarget(), window(window) {}

protected:
    virtual bool OnDropFiles(wxCoord, wxCoord, const wxArrayString& filenames){
        if (filenames.GetCount() == 1) {
            window->open(filenames[0], "");
            return true;
        }
        return false;
    }

private:
    SpekWindow *window;
};

SpekWindow::SpekWindow(const wxString& path, const wxString& device) :
    wxFrame(NULL, -1, wxEmptyString, wxDefaultPosition, wxSize(640, 480)), path(path), device(device)
{
    this->description = _("Spek - Acoustic Spectrum Analyser");
    SetTitle(this->description);

#ifndef OS_OSX
    SetIcons(wxArtProvider::GetIconBundle(ART_SPEK, wxART_FRAME_ICON));
#endif

    wxMenuBar *menu = new wxMenuBar();

    wxMenu *menu_file = new wxMenu();
    wxMenuItem *menu_file_open = new wxMenuItem(menu_file, wxID_OPEN);
    menu_file->Append(menu_file_open);
    wxMenuItem *menu_file_save = new wxMenuItem(menu_file, wxID_SAVE);
    menu_file->Append(menu_file_save);
    menu_file->AppendSeparator();
    menu_file->Append(wxID_EXIT);
    menu->Append(menu_file, _("&File"));

    wxMenu *menu_edit = new wxMenu();
    wxMenuItem *menu_edit_prefs = new wxMenuItem(menu_edit, wxID_PREFERENCES);
    menu_edit_prefs->SetItemLabel(menu_edit_prefs->GetItemLabelText() + "\tCtrl-E");
    menu_edit->Append(menu_edit_prefs);
    menu->Append(menu_edit, _("&Edit"));

    wxMenu *menu_help = new wxMenu();
    wxMenuItem *menu_help_contents = new wxMenuItem(
        menu_help, wxID_HELP, wxString(_("&Help")) + "\tF1");
    menu_help->Append(menu_help_contents);
    wxMenuItem *menu_help_about = new wxMenuItem(menu_help, wxID_ABOUT);
    menu_help_about->SetItemLabel(menu_help_about->GetItemLabelText() + "\tShift-F1");
    menu_help->Append(menu_help_about);
    menu->Append(menu_help, _("&Help"));

    SetMenuBar(menu);

    wxToolBar *toolbar = CreateToolBar();
    toolbar->AddTool(
        wxID_OPEN,
        wxEmptyString,
        wxArtProvider::GetBitmap(ART_OPEN, wxART_TOOLBAR),
        menu_file_open->GetItemLabelText()
    );
    toolbar->AddTool(
        wxID_SAVE,
        wxEmptyString,
        wxArtProvider::GetBitmap(ART_SAVE, wxART_TOOLBAR),
        menu_file_save->GetItemLabelText()
    );
    toolbar->AddStretchableSpace();
    toolbar->AddTool(
        wxID_HELP,
        wxEmptyString,
        wxArtProvider::GetBitmap(ART_HELP, wxART_TOOLBAR),
        _("Help")
    );
    toolbar->Realize();

    wxSizer *sizer = new wxBoxSizer(wxVERTICAL);

    // wxInfoBar is too limited, construct a custom one.
    wxPanel *info_bar = new wxPanel(this);
    info_bar->Hide();
    info_bar->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOTEXT));
    info_bar->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_INFOBK));
    wxSizer *info_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText *label = new wxStaticText(
        info_bar, -1, _("A new version of Spek is available, click to download."));
    label->SetCursor(*new wxCursor(wxCURSOR_HAND));
    label->Connect(wxEVT_LEFT_DOWN, wxCommandEventHandler(SpekWindow::on_visit));
    // This second Connect() handles clicks on the border
    info_bar->Connect(wxEVT_LEFT_DOWN, wxCommandEventHandler(SpekWindow::on_visit));
    info_sizer->Add(label, 1, wxALIGN_CENTER_VERTICAL | wxALL, 6);
    wxBitmapButton *button = new wxBitmapButton(
        info_bar, -1, wxArtProvider::GetBitmap(ART_CLOSE, wxART_BUTTON),
        wxDefaultPosition, wxDefaultSize, wxNO_BORDER);
    button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SpekWindow::on_close));
    info_sizer->Add(button, 0, wxALIGN_CENTER_VERTICAL);
    info_bar->SetSizer(info_sizer);
    sizer->Add(info_bar, 0, wxEXPAND);

    this->spectrogram = new SpekSpectrogram(this);
    sizer->Add(this->spectrogram, 1, wxEXPAND);

    this->cur_dir = wxGetHomeDir();

    if (!path.IsEmpty() || !device.IsEmpty()) {
        open(path, device);
    }

    SetDropTarget(new SpekDropTarget(this));

    SetSizer(sizer);

    pthread_t thread;
    pthread_create(&thread, NULL, &check_version, this);
}

void SpekWindow::open(const wxString& path, const wxString& device)
{
    wxFileName file_name(path);
    if (file_name.FileExists() || !device.IsEmpty()) {
        this->path = path;
        this->device = device;
        wxString full_name = file_name.GetFullName();
        // TRANSLATORS: window title, %s is replaced with the file name
        wxString title = wxString::Format(_("Spek - %s"), full_name.c_str());
        SetTitle(title);

        this->spectrogram->open(path, device);
    }
}

// TODO: s/audio/media/
static const char *audio_extensions[] = {
    "3gp",
    "aac",
    "aif",
    "aifc",
    "aiff",
    "amr",
    "awb",
    "ape",
    "au",
    "dts",
    "flac",
    "flv",
    "gsm",
    "m4a",
    "m4p",
    "mp3",
    "mp4",
    "mp+",
    "mpc",
    "mpp",
    "oga",
    "ogg",
    "opus",
    "ra",
    "ram",
    "snd",
    "wav",
    "wma",
    "wv",
    NULL
};

void SpekWindow::on_open(wxCommandEvent&)
{
    static wxString filters = wxEmptyString;
    static int filter_index = 1;

    if (filters.IsEmpty()) {
        filters.Alloc(1024);
        filters += _("All files");
        filters += "|*.*|";
        filters += _("Audio files");
        filters += "|";
        for (int i = 0; audio_extensions[i]; ++i) {
            if (i) {
                filters += ";";
            }
            filters += "*.";
            filters += wxString::FromAscii(audio_extensions[i]);
        }
        filters.Shrink();
    }

    wxFileDialog *dlg = new wxFileDialog(
        this,
        _("Open File"),
        this->cur_dir,
        wxEmptyString,
        filters,
        wxFD_OPEN
    );
    dlg->SetFilterIndex(filter_index);

    if (dlg->ShowModal() == wxID_OK) {
        this->cur_dir = dlg->GetDirectory();
        filter_index = dlg->GetFilterIndex();
        open(dlg->GetPath(), "");
    }

    dlg->Destroy();
}

void SpekWindow::on_save(wxCommandEvent&)
{
    static wxString filters = wxEmptyString;

    if (filters.IsEmpty()) {
        filters = _("PNG images");
        filters += "|*.png";
    }

    wxFileDialog *dlg = new wxFileDialog(
        this,
        _("Save Spectrogram"),
        this->cur_dir,
        wxEmptyString,
        filters,
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT
    );

    // Suggested name is <file_name>.png
    wxString name = _("Untitled");
    if (!this->path.IsEmpty()) {
        wxFileName file_name(this->path);
        name = file_name.GetFullName();
    }
    name += ".png";
    dlg->SetFilename(name);

    if (dlg->ShowModal() == wxID_OK) {
        this->cur_dir = dlg->GetDirectory();
        this->spectrogram->save(dlg->GetPath());
    }

    dlg->Destroy();
}

void SpekWindow::on_exit(wxCommandEvent&)
{
    Close(true);
}

void SpekWindow::on_preferences(wxCommandEvent&)
{
    SpekPreferencesDialog dlg(this);
    dlg.ShowModal();
}

void SpekWindow::on_help(wxCommandEvent&)
{
    wxLaunchDefaultBrowser(
        wxString::Format("http://spek.cc/man-%s.html", PACKAGE_VERSION)
    );
}

void SpekWindow::on_about(wxCommandEvent&)
{
    wxAboutDialogInfo info;
    info.AddDeveloper("Alexander Kojevnikov");
    info.AddDeveloper("Andreas Cadhalpun");
    info.AddDeveloper("Colin Watson");
    info.AddDeveloper("Daniel Hams");
    info.AddDeveloper("Fabian Deutsch");
    info.AddDeveloper("Jonathan Gonzalez V");
    info.AddDeveloper("Simon Ruderich");
    info.AddDeveloper("Stefan Kost");
    info.AddDeveloper("Thibault North");
    info.AddArtist("Olga Vasylevska");
    // TRANSLATORS: Add your name here
    wxString translator = _("translator-credits");
    if (translator != "translator-credits") {
        info.AddTranslator(translator);
    }
    info.SetName("Spek");
    info.SetVersion(PACKAGE_VERSION);
    info.SetCopyright(_("Copyright (c) 2010-2013 Alexander Kojevnikov and contributors"));
    info.SetDescription(this->description);
#ifdef OS_UNIX
    info.SetWebSite("http://spek.cc/", _("Spek Website"));
    info.SetIcon(wxArtProvider::GetIcon("spek", wxART_OTHER, wxSize(128, 128)));
#endif
    wxAboutBox(info);
}

void SpekWindow::on_notify(wxCommandEvent&)
{
    this->GetSizer()->Show((size_t)0);
    this->Layout();
}

void SpekWindow::on_visit(wxCommandEvent&)
{
    wxLaunchDefaultBrowser("http://spek.cc");
}

void SpekWindow::on_close(wxCommandEvent& event)
{
    wxWindow *self = ((wxWindow *)event.GetEventObject())->GetGrandParent();
    self->GetSizer()->Hide((size_t)0);
    self->Layout();
}

static void * check_version(void *p)
{
    // Does the user want to check for updates?
    SpekPreferences& prefs = SpekPreferences::get();
    if (!prefs.get_check_update()) {
        return NULL;
    }

    // Calculate the number of days since 0001-01-01, borrowed from GLib.
    wxDateTime now = wxDateTime::Now();
    int year = now.GetYear() - 1;
    int days = year * 365;
    days += (year >>= 2); // divide by 4 and add
    days -= (year /= 25); // divides original # years by 100
    days += year >> 2; // divides by 4, which divides original by 400
    days += now.GetDayOfYear();

    // When was the last update?
    int diff = days - prefs.get_last_update();
    if (diff < 7) {
        return NULL;
    }

    // Get the version number.
    wxString version;
    wxHTTP http;
    if (http.Connect("spek.cc")) {
        wxInputStream *stream = http.GetInputStream("/version");
        if (stream) {
            wxStringOutputStream out(&version);
            stream->Read(out);
            version.Trim();
            delete stream;
        }
    }

    if (version.IsEmpty()) {
        return NULL;
    }

    if (1 == spek_vercmp(version.mb_str(wxConvLibc), PACKAGE_VERSION)) {
        SpekWindow *self = (SpekWindow *)p;
        wxCommandEvent event(SPEK_NOTIFY_EVENT, -1);
        event.SetEventObject(self);
        wxPostEvent(self, event);
    }

    // Update the preferences.
    prefs.set_check_update(true);
    prefs.set_last_update(days);
    return NULL;
}
