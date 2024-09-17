/***************************************************************
 * Name:      BinomCDMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Miguel Valbuena ()
 * Created:   2021-11-06
 * Copyright: Miguel Valbuena ()
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#include "BinomCDMain.h"
#include "BinomCD.h"
#include <wx/bitmap.h>
#include <wx/icon.h>
#include <wx/image.h>

#include <val_utils.h>
#include <rational.h>
#include <thread>
#include <fstream>

#include "valDialogs.cpp"
#include "valControls.cpp"




BinomCDFrame::BinomCDFrame(wxFrame *frame, const wxString& title)
    : wxFrame(frame,-1,title)
{

    if (val::FileExists(iconpath)) {
        wxIcon FrameIcon;
        FrameIcon.CopyFromBitmap(wxBitmap(wxImage(iconpath)));
        SetIcon(FrameIcon);
    }
    else if (val::FileExists(alticonpath)) {
        wxIcon FrameIcon;
        FrameIcon.CopyFromBitmap(wxBitmap(wxImage(alticonpath)));
        SetIcon(FrameIcon);
    }

    GetSettings();

    // create a menu bar
    wxMenuBar* mbar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu(_T(""));
    fileMenu->Append(1001, _("&Quit\tAlt-F4"), _("Quit the application"));
    mbar->Append(fileMenu, _("&File"));

    wxMenu* helpMenu = new wxMenu(_T(""));
    helpMenu->Append(10001, _("&About\tF1"), _("Show info about this application"));
    mbar->Append(helpMenu, _("&Help"));
    SetMenuBar(mbar);

    //
    wxBoxSizer *boxsizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *in_boxsizer = new wxBoxSizer(wxHORIZONTAL);
    //
    wxStaticText *p_text = new wxStaticText(this,201,_("p:"));
    wxStaticText *n_text = new wxStaticText(this,202,_("n:"));
    wxStaticText *k_text = new wxStaticText(this,203,_("k1, k2:"));

    input_p = new wxTextCtrl(this,101,_("0,5"),wxDefaultPosition,wxDefaultSize,wxTE_PROCESS_ENTER|wxTE_RIGHT);
    input_n = new wxTextCtrl(this,102,_("100"),wxDefaultPosition,wxDefaultSize,wxTE_PROCESS_ENTER|wxTE_RIGHT,wxTextValidator(wxFILTER_NUMERIC));
    in_boxsizer->Add(p_text,0,wxALL,2);
    in_boxsizer->Add(input_p,0,wxALL,5);
    in_boxsizer->Add(n_text,0,wxALL,2);
    in_boxsizer->Add(input_n,0,wxALL,5);

    boxsizer->Add(in_boxsizer,0,wxALL|wxEXPAND,5);
    boxsizer->Add(k_text,0,wxALL,5);

    islider = new val::IntervalSlider(this,301,270,15);
    boxsizer->Add(islider,0,wxALL,5);

    output = new wxTextCtrl(this,103,_(""),wxDefaultPosition,wxSize(output_width,output_height),wxTE_MULTILINE|wxTE_READONLY);
    h_text = val::ToString(alpha) + "\n" + val::ToString(n) + "\n";
    h_text += "p <= " + val::ToString(p);
    {
        wxFont myfont(output->GetFont());
        myfont.SetPointSize(textsize);
        output->SetFont(myfont);

    }
    boxsizer->Add(output,1,wxALL|wxEXPAND,5);


    // create a status bar with some information about the used wxWidgets version
    CreateStatusBar(2);
    //
    SetSizer(boxsizer);
    boxsizer->Fit(this);
    boxsizer->SetSizeHints(this);


    // Bind handlers
    Bind(wxEVT_COMMAND_MENU_SELECTED,&BinomCDFrame::OnQuit,this,1001);
    Bind(wxEVT_COMMAND_MENU_SELECTED,&BinomCDFrame::OnAbout,this,10001);
    Bind(wxEVT_TEXT_ENTER,&BinomCDFrame::Onparchanged,this,101);
    Bind(wxEVT_TEXT_ENTER,&BinomCDFrame::Onparchanged,this,102);
    input_p->Bind(wxEVT_KILL_FOCUS,&BinomCDFrame::OnFocuslost,this);
    input_n->Bind(wxEVT_KILL_FOCUS,&BinomCDFrame::OnFocuslost,this);
    Bind(val_EVENT_SLIDER,&BinomCDFrame::Onborderschanged,this,301);
    //Bind(IS_EVENT,&BinomCDFrame::Onborderschanged,this);
    Bind(MY_EVENT,&BinomCDFrame::WriteResults,this);

    Bind(wxEVT_COMMAND_MENU_SELECTED,&BinomCDFrame::OnChangeTextSize,this,20001);
    Bind(wxEVT_COMMAND_MENU_SELECTED,&BinomCDFrame::OnChangeTextSize,this,20002);
    Bind(wxEVT_COMMAND_MENU_SELECTED,&BinomCDFrame::OnHypothesisTest,this,20003);
    //

    wxAcceleratorEntry entries[3];
    entries[0].Set(wxACCEL_CTRL, (int) '+', 20001);
    entries[1].Set(wxACCEL_CTRL, (int) '-', 20002);
    entries[2].Set(wxACCEL_CTRL, (int) 'T', 20003);
    wxAcceleratorTable accel(3, entries);
    SetAcceleratorTable(accel);
    //
    MyFrame = this;
    SetSize(framesize);
    Move(position);
}

void BinomCDFrame::GetSettings()
{
    //std::ifstream file(val::GetExeDir()+filesep+"settings.txt",std::ios::in);
    std::ifstream file(settingsfile,std::ios::in);
    if (!file) {
        if (!val::DirExists(valdir)) {
            if (!val::CreateDir(valdir)) wxMessageBox("Cannot Create\n" + valdir);
        }
        if (!val::DirExists(settingsdir)) {
             if (!val::CreateDir(settingsdir)) wxMessageBox("Cannot Create\n" + settingsdir);
        }
        return;
    }
    int x,y;
    file>>position.x>>position.y>>x>>y>>textsize;
    framesize.SetWidth(x); framesize.SetHeight(y);
    file.close();
}


void BinomCDFrame::Onparchanged(wxCommandEvent&)
{
    n = val::FromString<int>(std::string(input_n->GetValue()));
    if (n<0) {
        n=100;
        input_n->SetValue(val::ToString(n));
    }
    p = double(val::FromString<val::rational>(std::string(input_p->GetValue())));
    if (p<0 || p>1) {
        p=1;
        input_p->SetValue(val::ToString(p));
    }
    islider->setlimits(0,n);
    Compute();
}

void BinomCDFrame::OnFocuslost(wxFocusEvent &event)
{
    n = val::FromString<int>(std::string(input_n->GetValue()));
    if (n<0) {
        n=100;
        input_n->SetValue(val::ToString(n));
    }
    p = double(val::FromString<val::rational>(std::string(input_p->GetValue())));
    if (p<0 || p>1) {
        p=1;
        input_p->SetValue(val::ToString(p));
    }
    islider->setlimits(0,n);
    event.Skip();
    Compute();
}

void BinomCDFrame::Onborderschanged(wxCommandEvent&)
{
    Compute();
}

void BinomCDFrame::Compute()
{
    if (iscomputing) return;
    iscomputing=1;
    k1 = islider->getleftvalue();
    k2 = islider->getrightvalue();
    //wxMessageBox("Computing");
    std::thread t(computeCD,std::cref(p),n,k1,k2,std::ref(s_output));
    t.detach();
}


void BinomCDFrame::OnChangeTextSize(wxCommandEvent &event)
{
    int id=event.GetId();
    wxFont myfont(output->GetFont());
    //int mysize=myfont.GetPointSize ();

    if (id==20001) textsize++;
    else textsize--;

    if (textsize>20) textsize=20;
    if (textsize<9) textsize=9;
    myfont.SetPointSize(textsize);
    output->SetFont(myfont);
}

void BinomCDFrame::OnHypothesisTest(wxCommandEvent &event)
{
    wxString entry = L"\u03B1; n; p<= [p>=] p[=]", relation;
    test_type type = test_type::R;

    val::MultiLineDialog dialog(this,h_text,entry,240,100,"Hypothesentest",textsize);
    if (dialog.ShowModal()==wxID_CANCEL) return;
    h_text = dialog.GetSettingsText();

    val::d_array<char> separators{' ', '\n'};
    auto values = getwordsfromstring(std::string(h_text), separators);
    int m = values.length();
    if (m >= 1) alpha = val::FromString<double>(values[0]);
    if (m >=2 ) n = val::FromString<int>(values[1]);
    if (m >= 4) {
        if (values[3] == "<" || values[3] == "<=") {
            type = test_type::R;
            relation = "<=";
        }
        else if (values[3] == ">" || values[3] == ">=") {
            type = test_type::L;
            relation = ">=";
        }
        else {
            type = test_type::B;
            relation = "==";
        }
    }
    if (m >= 5) p = double(val::FromString<val::rational>(values[4]));

    if (alpha > 0.5 || alpha < 0) alpha = 0.05;
    if (p < 0 || p > 1) p = 0.5;
    if (n < 0) n = 100;
    h_text = val::ToString(alpha) + "\n" + val::ToString(n) +  "\np " + relation + " " + val::ToString(p);
    std::thread t(hypothesentest,p,n,alpha,type);
    t.detach();
}


void BinomCDFrame::WriteResults(MyThreadEvent& event)
{
    if (event.GetId() == event_type::TEST) {
        input_p->SetValue(val::ToString(p));
        input_n->SetValue(val::ToString(n));
        //int k1 = event.GetLeftNumber(), k2 = event.GetRightNumber();
        //wxMessageBox(val::ToString(k1) + " , " + val::ToString(k2));
        islider->setallvalues(0,n,event.GetLeftNumber(), event.GetRightNumber());
        //Compute();
        return;
    }
    output->SetValue(s_output);
    iscomputing=0;
}

BinomCDFrame::~BinomCDFrame()
{
    using namespace std;
    std::ofstream file(settingsfile,ios::out | ios::trunc);
    if (file) {
        wxSize MySize=GetSize();
        wxPoint MyPoint=GetPosition();
        file<<MyPoint.x<<endl;
        file<<MyPoint.y<<endl;
        file<<MySize.x<<endl;
        file<<MySize.y<<endl;
        file<<output->GetFont().GetPointSize();
        file.close();
    }
}

void BinomCDFrame::OnClose(wxCloseEvent &event)
{
    Destroy();
}

void BinomCDFrame::OnQuit(wxCommandEvent &event)
{
    Destroy();
}

void BinomCDFrame::OnAbout(wxCommandEvent &event)
{
    wxMessageBox(_("programmed by Miguel Valbuena"),_("BinomCD"));
}
