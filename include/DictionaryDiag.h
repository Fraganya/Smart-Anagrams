#ifndef DICTIONARYDIAG_H
#define DICTIONARYDIAG_H
#include "wx/wx.h"
#include "wx/filepicker.h"

class DictionaryDiag:public wxDialog
{
    public:
        DictionaryDiag(const wxString& title,wxFrame * parent);
        DictionaryDiag(const wxString& title,wxFrame * parent,bool removeDiag);
    protected:
        void DictAdd(wxCommandEvent& event);
        void DictRemove(wxCommandEvent& event);
    private:
        wxTextCtrl * inputBox;
        wxTextCtrl * tableBox;
        wxFilePickerCtrl * file;
        wxListBox  * lanList;
};

#endif // DICTIONARYDIAG_H
