/***************************************************************
 * Name:      BinomCDApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Miguel Valbuena ()
 * Created:   2021-11-06
 * Copyright: Miguel Valbuena ()
 * License:
 **************************************************************/


#include "BinomCDApp.h"
#include "BinomCDMain.h"
#include <wx/image.h>

IMPLEMENT_APP(BinomCDApp)

bool BinomCDApp::OnInit()
{
    wxInitAllImageHandlers();
    BinomCDFrame* frame = new BinomCDFrame(0L,_("BinomCD"));

    frame->Show();

    return true;
}
