#ifndef SMARTFRAME_H
#define SMARTFRAME_H

#include "wx/wx.h"
#include "wx/combobox.h"
#include <sqlite_modern_cpp.h>


class SmartFrame:public wxFrame
{
    public:
        SmartFrame(const wxString& title);
    protected:
        void OnQuit(wxCommandEvent&);
        void OnClear(wxCommandEvent&);
        void OnFind(wxCommandEvent&);
        void OnAddDict(wxCommandEvent&);
        void OnRemoveDict(wxCommandEvent&);
        void OnAbout(wxCommandEvent&);
        void OnHelp(wxCommandEvent&);

        bool validateAnagram();
        bool runValidation(wxString,wxString,bool&);

        int getMaxPermutations(wxString);
    private:
        wxTextCtrl * inputBox;
        wxComboBox * lanBox;
        wxCheckBox * getValidOnly;
        sqlite::database * dictionary;
};

enum APP_IDS{
    ID_FILE,
    ID_HELP,
    ID_FIND,
    ID_CLEAR,
    ID_ADD_DICT,
    ID_REMOVE_DICT,
    ID_ABOUT,
};

#endif // SMARTFRAME_H
