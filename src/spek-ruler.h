#pragma once

#include <wx/dc.h>
#include <wx/string.h>

class SpekRuler
{
public:
    enum Position
    {
        TOP,
        RIGHT,
        BOTTOM,
        LEFT
    };

    typedef wxString (*formatter_cb)(int unit);

    SpekRuler(
        int x, int y, Position pos, wxString sample_label,
        int *factors, int min_units, int max_units, double spacing,
        double scale, double offset, formatter_cb formatter
    );

    void draw(wxDC& dc);

protected:
    void draw_tick(wxDC& dc, int tick);

    int x;
    int y;
    Position pos;
    wxString sample_label;
    int *factors;
    int min_units;
    int max_units;
    double spacing;
    double scale;
    double offset;
    formatter_cb formatter;
};
