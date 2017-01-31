#include "AnagramsDiag.h"
#include "wx/progdlg.h"

#include <string>
#include <sstream>
#ifndef SSTR
    #define SSTR(x) static_cast<std::ostringstream &>((std::ostringstream()<<std::dec<<x)).str()
#endif // SSTR

AnagramsDiag::AnagramsDiag(wxFrame * parent,const wxString& title)
:wxDialog(parent,wxID_ANY,title,wxDefaultPosition,wxSize(500,400))
{
    wxPanel * diagPanel=new wxPanel(this,wxID_ANY);

    searchBox=new wxTextCtrl(diagPanel,wxID_ANY);
    wxButton  * checkBtn=new wxButton(diagPanel,wxID_ANY,wxT("is on List?"));
    cList=new wxListCtrl(diagPanel,wxID_ANY,wxDefaultPosition,wxDefaultSize,
                                    wxLC_LIST);

    wxBoxSizer * upperBox=new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer * mBox=new wxBoxSizer(wxVERTICAL);

    upperBox->Add(searchBox,3,wxEXPAND);
    upperBox->Add(checkBtn,1,wxEXPAND);


    Connect(checkBtn->GetId(),wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(AnagramsDiag::onList));
    mBox->Add(upperBox,0,wxEXPAND | wxLEFT | wxTOP | wxRIGHT,10);
    mBox->Add(-1,10);
    mBox->Add(cList,3,wxALL | wxEXPAND,10);

    diagPanel->SetSizer(mBox);
    Centre();
}
/**
@name AddAnagram  -adds an anagram to the list box control
@param wxString anagram
@return void
*/
void AnagramsDiag::AddAnagram(wxString anagram)
{
        this->cList->InsertItem(this->cList->GetItemCount(),anagram);
}
void AnagramsDiag :: onList(wxCommandEvent& WXUNUSED(event))
{
    wxString itemStr=this->searchBox->GetValue();
    if((this->cList->FindItem(-1,(itemStr.Lower()),false))!=wxNOT_FOUND){
        wxMessageBox(itemStr+" is in the list",wxT("Search - Match"));
    }
    else{
         wxMessageBox(itemStr+" is not in the list",wxT("Search - no match"));
    }
}
