//Diet Paint
//Author: Connor Booth
//
//This program emulates some of the basic features of a drawing program, allowing one to draw curves, lines, boxes, et cetera.
//The user can save and load bitmaps, and they can draw with a number of colors, with a range of pen sizes.
//
//The purpose of this personal project is satisfy an interest in learning GUI programming.
//I use the WxWidgets library for the GUI elements, as it is supported on most Operating Systems through its use of 
//native OS function calls to display "widgets".


#include <wx/wxprec.h>
#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include <vector>
#include <string>
#include <wx/file.h>
#include <wx/filedlg.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif


const std::vector<wxColour> colors = { wxColour(237, 28, 36), wxColour(255, 255, 128), wxColour(128, 255, 128), wxColour(0, 255, 128),
                                       wxColour(128, 255, 255), wxColour(0, 128, 255), wxColour(255, 128, 192), wxColour(255, 128, 255),
                                       wxColour(255, 0, 0), wxColour(255, 255, 0), wxColour(128, 255, 0), wxColour(0, 255, 64),
                                       wxColour(0, 255, 255), wxColour(0, 128, 192), wxColour(128, 128, 192), wxColour(255, 0, 255),
                                       wxColour(128, 64, 64), wxColour(255, 128, 64), wxColour(0, 255, 0), wxColour(0, 128, 128),
                                       wxColour(0, 64, 128), wxColour(128, 128, 255), wxColour(128, 0, 64), wxColour(255, 0, 128),
                                       wxColour(128, 0, 0), wxColour(255, 128, 0), wxColour(0, 128, 0), wxColour(0, 128, 64),
                                       wxColour(0, 0, 255), wxColour(0, 0, 160), wxColour(128, 0, 128), wxColour(128, 0, 255),
                                       wxColour(64, 0, 0), wxColour(128, 64, 0), wxColour(0, 64, 0), wxColour(0, 64, 64),
                                       wxColour(0, 0, 128), wxColour(0, 0, 64), wxColour(64, 0, 64), wxColour(64, 0, 128),
                                       wxColour(0, 0, 0), wxColour(128, 128, 0), wxColour(128, 128, 64), wxColour(128, 128, 128),
                                       wxColour(64, 128, 128), wxColour(192, 192, 192), wxColour(111, 0, 111), wxColour(255, 255, 255),
                                       wxColour(47, 208, 136), wxColour(115, 214, 41), wxColour(100, 160, 100)


};

enum
{
    ID_Paint = 7,
    ID_LDown = 6,
    ID_LUp = 8,
    ID_ColButton = 80,
    ID_Pen = 28,
    ID_Line = 29,
    ID_Fill = 30,
    ID_Box = 31,
    ID_Circle = 32,
    ID_Save = 33,
    ID_Load = 34,
    ID_Picker = 63,
    ID_Slider = 310,
    ID_ToolButton = 410
};

//Classes -----------------------------------------------------------------------------------------------------------------------------------/

class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

class MyFrame : public wxFrame
{
public:
    MyFrame();
    int activeTool = ID_Paint;
    wxBitmap bmp = wxBitmap(800, 650, -1);
    wxPen mainPen = wxPen(*wxBLACK, 3);
private:
};

class DrawPanel : public wxPanel
{
public:
    DrawPanel(wxWindow* window, wxWindowID id, wxSize size);
    wxPoint mousePos;
    wxPoint mousePos2;
    std::vector<wxPoint> shapePoints;


private:
    void OnPaint(wxMouseEvent& event);
    void OnLeftDown(wxMouseEvent& event);
};

class ToolButton : public wxButton
{
public:
    ToolButton(wxWindow* window, wxWindowID id, wxSize size, std::string name, wxColour color);
    int toolId;

private:
    void OnClick(wxCommandEvent& event);
};

class ColorButton : public wxButton
{
public:
    ColorButton(wxWindow* window, wxWindowID id, wxSize size, wxColour color);
    wxColour color;

private:
    void OnClick(wxCommandEvent& event);
};

class SidePanel : public wxPanel
{
public:
    SidePanel(wxWindow* window, wxWindowID id, wxSize);

    wxSlider* penSize = new wxSlider(this, ID_Slider, 3, 1, 100, wxDefaultPosition, wxDefaultSize, wxSL_VALUE_LABEL);
private:
    void OnScroll(wxScrollEvent& event);
};

//main function contained in this macro
wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    MyFrame* frame = new MyFrame();
    frame->Show(true);

    return true;
}

//Ctors ------------------------------------------------------------------------------------------------------------------------/

MyFrame::MyFrame()
    : wxFrame(NULL, ID_Paint, "Diet Paint", wxDefaultPosition, wxSize(1200, 800))
{
    SetBackgroundColour(wxColour("#555555"));
    DrawPanel* panel = new DrawPanel(this, ID_Paint, wxSize(800, 650));
    panel->SetBackgroundColour(*wxWHITE);
    SidePanel* sPanel = new SidePanel(this, wxID_ANY, wxSize(200, 450));
    sPanel->SetBackgroundColour(wxColour("#009900"));
    sPanel->SetMaxSize(wxSize(200, -1));
    sPanel->SetMinSize(wxSize(200, -1));

    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(sPanel, 0, wxEXPAND, 0);
    wxBoxSizer* drawSizer = new wxBoxSizer(wxVERTICAL);
    drawSizer->Add(panel, 0, wxALIGN_CENTER, 0);
    mainSizer->Add(drawSizer, 1, wxALIGN_CENTER, 0);

    this->SetSizer(mainSizer);
}

DrawPanel::DrawPanel(wxWindow* window, wxWindowID id, wxSize size)
    : wxPanel(window, id, wxDefaultPosition, size)
{
    wxClientDC cdc(this);
    wxBufferedDC bdc(&cdc, ((MyFrame*)GetParent())->bmp);
    bdc.Clear();
    Bind(wxEVT_LEFT_DOWN, &DrawPanel::OnLeftDown, this, ID_Paint);
    Bind(wxEVT_MOTION, &DrawPanel::OnPaint, this, ID_Paint);
}

SidePanel::SidePanel(wxWindow* window, wxWindowID id, wxSize size)
    : wxPanel(window, id, wxDefaultPosition, size)
{
    wxBoxSizer* trueSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* toolSizer = new wxBoxSizer(wxVERTICAL);

    toolSizer->Add(new ToolButton(this, ID_Pen, wxSize(50, 30), "Pen", wxColour(255, 0, 0)), 0, wxEXPAND, 0);
    toolSizer->Add(new ToolButton(this, ID_Picker, wxSize(50, 30), "Color Picker", wxColour(0, 255, 0)), 0, wxEXPAND, 0);
    toolSizer->Add(new ToolButton(this, ID_Fill, wxSize(50, 30), "Fill Tool", wxColour(0, 0, 255)), 0, wxEXPAND, 0);
    toolSizer->Add(new ToolButton(this, ID_Line, wxSize(50, 30), "Line Tool", wxColour(255, 255, 255)), 0, wxEXPAND, 0);
    toolSizer->Add(new ToolButton(this, ID_Box, wxSize(50, 30), "Box Tool", wxColour(127, 127, 127)), 0, wxEXPAND, 0);
    toolSizer->Add(new ToolButton(this, ID_Circle, wxSize(50, 30), "Circle Tool", wxColour(0, 255, 255)), 0, wxEXPAND, 0);
    toolSizer->Add(new ToolButton(this, ID_Save, wxSize(50, 30), "Save", wxColour(255, 255, 0)), 0, wxEXPAND, 0);
    toolSizer->Add(new ToolButton(this, ID_Load, wxSize(50, 30), "Load", wxColour(255, 0, 255)), 0, wxEXPAND, 0);

    trueSizer->Add(toolSizer, 0, wxEXPAND, 0);
    trueSizer->Add(penSize, 0, wxEXPAND, 0);

    wxGridSizer* colorSizer = new wxGridSizer(13, 4, 1, 1);
    for (int i = 0; i < colors.size(); i++)
    {
        colorSizer->Add(new ColorButton(this, ID_ColButton + i, wxSize(40, 40), colors[i]), 0, wxEXPAND, 0);
    }

    trueSizer->Add(colorSizer, 0, wxALIGN_CENTER, 0);
    this->SetSizer(trueSizer);

    Bind(wxEVT_SCROLL_THUMBRELEASE, &SidePanel::OnScroll, this, ID_Slider);
}

ColorButton::ColorButton(wxWindow* window, wxWindowID id, wxSize size, wxColour color)
    : wxButton(window, id, "", wxDefaultPosition, size)
{
    SetBackgroundColour(color);

    Bind(wxEVT_BUTTON, &ColorButton::OnClick, this, ID_ColButton, ID_ColButton + 50);
}

ToolButton::ToolButton(wxWindow* window, wxWindowID id, wxSize size, std::string name, wxColour color/*, wxWindowID toolId*/)
    : wxButton(window, id, name, wxDefaultPosition, size)
{
    SetBackgroundColour(color);
    this->toolId = id;
    Bind(wxEVT_BUTTON, &ToolButton::OnClick, this, ID_Pen, ID_Picker);
}

//Event Handlers -----------------------------------------------------------------------------------------------------------------------/

void DrawPanel::OnLeftDown(wxMouseEvent& event)
{
    wxClientDC cdc(this);
    wxBufferedDC bdc(&cdc, ((MyFrame*)GetParent())->bmp);
    bdc.SetPen(((MyFrame*)GetParent())->mainPen);

    shapePoints.push_back(event.GetPosition());



    if (shapePoints.size() == 2)
    {
        switch (((MyFrame*)GetParent())->activeTool)
        {
        case ID_Line:
            bdc.DrawLine(shapePoints[0], shapePoints[1]);
            break;
        case ID_Box:
            bdc.DrawRectangle(wxRect(shapePoints[0], shapePoints[1]));
            break;
        case ID_Circle:
            bdc.DrawEllipse(wxRect(shapePoints[0], shapePoints[1]));
            break;
        }

        shapePoints.clear();
    }



    else if (((MyFrame*)GetParent())->activeTool == ID_Picker)
    {
        wxColour* color = new wxColour();
        bdc.GetPixel(event.GetPosition().x, event.GetPosition().y, color);
        ((MyFrame*)GetParent())->mainPen.SetColour(*color);
    }
    else
        mousePos = event.GetPosition();
}

void DrawPanel::OnPaint(wxMouseEvent& event)
{
    wxClientDC cdc(this);
    wxBufferedDC bdc(&cdc, ((MyFrame*)GetParent())->bmp);
    bdc.SetPen(((MyFrame*)GetParent())->mainPen);

    switch (((MyFrame*)GetParent())->activeTool)
    {
    case ID_Pen:
        shapePoints.clear();
        if (event.LeftIsDown())
        {
            mousePos2 = event.GetPosition();
            bdc.DrawLine(mousePos, mousePos2);
            mousePos = mousePos2;
        }
        break;
    case ID_Fill:
        if (event.LeftIsDown())
        {
            wxColour* color = new wxColour(*wxWHITE);
            bdc.GetPixel(event.GetPosition().x, event.GetPosition().y, color);
            bdc.SetBrush(((MyFrame*)GetParent())->mainPen.GetColour());
            bdc.FloodFill(event.GetPosition(), *color, wxFLOOD_SURFACE);
        }
        break;
    case ID_Picker:
        if (event.LeftDown())
        {
            wxColour* color = new wxColour();
            bdc.GetPixel(event.GetPosition().x, event.GetPosition().y, color);
            ((MyFrame*)GetParent())->mainPen.SetColour(*color);
        }
        break;
    }
}

void SidePanel::OnScroll(wxScrollEvent& event)
{
    ((MyFrame*)GetParent())->mainPen.SetWidth(penSize->GetValue());
}

void ColorButton::OnClick(wxCommandEvent& event)
{
    ((MyFrame*)GetGrandParent())->mainPen.SetColour(colors[event.GetId() - ID_ColButton]);
}

void ToolButton::OnClick(wxCommandEvent& event)
{
    if (this->toolId == ID_Save)
    {
        wxFile file;
        wxFileDialog dialog(this, "Save Image", "", "", "BMP files (*.bmp)|*.bmp", wxFD_SAVE | wxFD_OVERWRITE_PROMPT | wxFD_CHANGE_DIR);
        if (dialog.ShowModal() == wxID_OK)
        {
            file.Create(dialog.GetPath(), wxPOSIX_USER_WRITE);
            ((MyFrame*)GetGrandParent())->bmp.SaveFile(dialog.GetPath(), wxBITMAP_TYPE_BMP);
        }
    }
    else if (this->toolId == ID_Load)
    {
        wxFileDialog dialog(this, "Load Image", "", "", "BMP files(*.bmp) | *.bmp", wxFD_OPEN);
        if (dialog.ShowModal() == wxID_OK)
        {
            ((MyFrame*)GetGrandParent())->bmp.LoadFile(dialog.GetPath(), wxBITMAP_TYPE_BMP);
        }
    }
    else
        ((MyFrame*)GetGrandParent())->activeTool = this->toolId;
}