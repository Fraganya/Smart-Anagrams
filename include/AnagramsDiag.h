#ifndef ANAGRAMSDIAG_H
#define ANAGRAMSDIAG_H

#include "wx/wx.h"
#include "wx/listctrl.h"

class AnagramsDiag:public wxDialog
{
    public:
        AnagramsDiag(wxFrame * parent,const wxString& title);
        void AddAnagram(wxString anagram);
        void onList(wxCommandEvent&);
    protected:

    private:
         wxListCtrl * cList;
         wxTextCtrl * searchBox;
};

#endif // ANAGRAMSDIAG_H
