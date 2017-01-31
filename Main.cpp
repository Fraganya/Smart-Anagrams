#include "wx/wx.h"
#include "SmartFrame.h"

class SmartMatcher:public wxApp
{
    public:
        virtual bool OnInit();
};

bool SmartMatcher::OnInit()
{
    SmartFrame * appFrame=new SmartFrame(wxT("Smart Anagrams"));
    appFrame->Show(true);
    return true;
}

wxIMPLEMENT_APP(SmartMatcher);
