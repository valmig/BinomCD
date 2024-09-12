/***************************************************************
 * Name:      BinomCDMain.h
 * Purpose:   Defines Application Frame
 * Author:    Miguel Valbuena ()
 * Created:   2021-11-06
 * Copyright: Miguel Valbuena ()
 * License:
 **************************************************************/

#ifndef BINOMCDMAIN_H
#define BINOMCDMAIN_H

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/frame.h>

#include "valControls.h"

class MyThreadEvent;

class BinomCDFrame: public wxFrame
{
    public:
        BinomCDFrame(wxFrame *frame, const wxString& title);
        ~BinomCDFrame();
    private:
        wxTextCtrl *input_p=nullptr,*input_n=nullptr,*output=nullptr;
        val::IntervalSlider *islider=nullptr;
        wxString s_output, h_text;
        wxPoint position=wxDefaultPosition;
        wxSize framesize = wxDefaultSize;
        int n=100,k1=0,k2=100,iscomputing=0,output_width=260,output_height=120,textsize=11;
        double p=0.5, alpha = 0.05;
        void OnClose(wxCloseEvent& event);
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void Onparchanged(wxCommandEvent&);
        void OnFocuslost(wxFocusEvent&);
        void OnChangeTextSize(wxCommandEvent &event);
        //void Onborderschanged(ISliderEvent&);
        void Onborderschanged(wxCommandEvent&);
        void OnHypothesisTest(wxCommandEvent&);
        void WriteResults(MyThreadEvent&);
        void Compute();
        void GetSettings();
};


#endif // BINOMCDMAIN_H
