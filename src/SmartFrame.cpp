#include "SmartFrame.h"
#include "AnagramsDiag.h"
#include "DictionaryDiag.h"

#include "wx/progdlg.h"

#include <algorithm>
#include <string>
#include <sstream>



#ifndef SSTR
    #define SSTR(x) static_cast<std::ostringstream &>((std::ostringstream()<<std::dec<<x)).str()
#endif // SSTR

DECLARE_APP(wxApp)

SmartFrame::SmartFrame(const wxString& title):wxFrame(nullptr,wxID_ANY,title,wxPoint(20,20),wxSize(350,200))
{
    //set window menus and connect events
    wxMenuBar * appMenuBar=new wxMenuBar();

    wxMenu * file=new wxMenu();
    wxMenu * help=new wxMenu();
    wxMenu * dictionary=new wxMenu();

    dictionary->Append(APP_IDS::ID_ADD_DICT,wxT("Add"));
    dictionary->Append(APP_IDS::ID_REMOVE_DICT,wxT("Remove"));
    file->AppendSubMenu(dictionary,wxT("Dictionary"));
    file->Append(wxID_EXIT,wxT("Quit"));
    help->Append(APP_IDS::ID_HELP,wxT("Help"));
    help->Append(APP_IDS::ID_ABOUT,wxT("About"));

    appMenuBar->Append(file,wxT("&File"));
    appMenuBar->Append(help,wxT("&Help"));


    //set window components
    wxPanel * mPanel=new wxPanel(this,-1);

    wxStaticText * helperLabel=new wxStaticText(mPanel,wxID_ANY,
                                               wxT("Enter the letters (Spaces will be truncated)"));
    this->inputBox=new wxTextCtrl(mPanel,wxID_ANY);

    wxArrayString * options=new wxArrayString();
    //get available dictionaries
    try{
         this->dictionary=new sqlite::database("./dicts.sqlite");
         *(this->dictionary)<<"Select language from dictionaries">>[&](std::string language){
              options->Add((wxString)language);
         };
    }
    catch(sqlite::sqlite_exception& e){
        wxMessageBox(e.what(),wxT("DB Error"),wxOK | wxICON_ERROR);
    }
    wxStaticText * lanLabel=new wxStaticText(mPanel,wxID_ANY,wxT("Language (Only available dictionaries)"));
    this->lanBox=new wxComboBox(mPanel,wxID_ANY,"",wxDefaultPosition,wxDefaultSize,(*options),wxCB_READONLY);
    getValidOnly=new wxCheckBox(mPanel,wxID_ANY,wxT("Show valid words only"));




    wxButton * findBtn=new wxButton(mPanel,APP_IDS::ID_FIND,wxT("&Find"));
    wxButton * clearBtn=new wxButton(mPanel,APP_IDS::ID_CLEAR,wxT("&Clear"));


    wxBoxSizer * mBox=new wxBoxSizer(wxVERTICAL);
    wxBoxSizer * upperBox=new wxBoxSizer(wxVERTICAL);
    wxBoxSizer * lowerBox=new wxBoxSizer(wxHORIZONTAL);

    upperBox->Add(helperLabel,0,wxEXPAND);
    upperBox->Add(inputBox,0,wxEXPAND);
    upperBox->Add(lanLabel,0,wxEXPAND);
    upperBox->Add(lanBox,0,wxEXPAND);
    upperBox->Add(getValidOnly,0,wxEXPAND);

    lowerBox->Add(findBtn);
    lowerBox->Add(clearBtn,0,wxLEFT,5);

    mBox->Add(upperBox,0,wxEXPAND | wxLEFT | wxTOP | wxRIGHT,10);
    mBox->Add(-1,10);
    mBox->Add(lowerBox,0,wxALIGN_RIGHT | wxRIGHT,10);

    mPanel->SetSizer(mBox);
    SetMenuBar(appMenuBar);

    Connect(wxID_EXIT,wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(SmartFrame::OnQuit));
    Connect(APP_IDS::ID_ADD_DICT,wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(SmartFrame::OnAddDict));
    Connect(APP_IDS::ID_HELP,wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(SmartFrame::OnHelp));
    Connect(APP_IDS::ID_ABOUT,wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(SmartFrame::OnAbout));
    Connect(APP_IDS::ID_REMOVE_DICT,wxEVT_COMMAND_MENU_SELECTED,wxCommandEventHandler(SmartFrame::OnRemoveDict));
    Connect(APP_IDS::ID_CLEAR,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(SmartFrame::OnClear));
    Connect(APP_IDS::ID_FIND,wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(SmartFrame::OnFind));

    //set icon and sizes
    this->SetIcon(wxIcon(wxT("./smart-anagram.ico"),wxBITMAP_TYPE_ICO));
    this->SetMaxSize(wxSize(350,200));
    this->SetMinSize(wxSize(350,200));
    this->Center();
}
void SmartFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void SmartFrame::OnClear(wxCommandEvent& WXUNUSED(event))
{
    this->inputBox->SetValue(wxT(""));
    this->inputBox->SetFocus();
}

void SmartFrame::OnFind(wxCommandEvent& WXUNUSED(event))
{
    std::string anagram=(std::string)(this->inputBox->GetValue()); //get character set
    int maxAnagrams=this->getMaxPermutations(anagram);
    AnagramsDiag * cDiag=new AnagramsDiag(this,wxT("Anagrams"));
    wxProgressDialog * pDiag=new wxProgressDialog(wxT("Computing Anagrams"),
                                                wxT("Running permutations..."),
                                                maxAnagrams,
                                                this,
                                                wxPD_CAN_ABORT | wxPD_APP_MODAL | wxPD_ELAPSED_TIME |
                                                wxPD_ESTIMATED_TIME| wxPD_REMAINING_TIME | wxPD_AUTO_HIDE| wxPD_SMOOTH);

    bool proceed=true; // status of progress dialog
    //compute if the anagrams should be validated
    bool validate =(this->validateAnagram()) ? true : false;
    std::string language;
    std::string filename="en_dict.sqlite";
    std::string table="entries";
    if(validate && this->dictionary){
       // this->dictionary=new sqlite::database("./dicts.sqlite");
        language=(std::string)this->lanBox->GetValue();
        try{
                   *(this->dictionary)<<"SELECT filename,table_name from dictionaries where language='"+language+"';">>tie(filename,table);
        }
        catch(sqlite::sqlite_exception& e){
            wxMessageBox("Error Getting Dictionary info.using (Default)",wxT("Error"),wxOK | wxICON_ERROR);
        }
        this->dictionary=new sqlite::database("./dictionaries/"+filename);
    }

    int count=1;
    std::sort(anagram.begin(),anagram.end()); //lexicographically sort the character set for permutation
    //add the anagrams to the anagrams list box
    do{

        wxMilliSleep(10); // wait 10 ms for user interaction
        proceed=pDiag->Update(count,("Generating permutations..."+SSTR(count)+" of "+SSTR(maxAnagrams)));
        //filter valid language words if getValidOnly is specified
        if(validate){
            if(this->runValidation(anagram,(wxString)table,validate)) cDiag->AddAnagram(((wxString)anagram).Lower());
        }
        else{
            cDiag->AddAnagram(((wxString)anagram).Lower());
        }

        if(!proceed){
            if(wxMessageBox(wxT("Are you sure you want to abort?"),wxT("Abort?"),wxYES_NO | wxICON_QUESTION )== wxYES){
                break;
            }
            pDiag->Resume();
        }
        wxGetApp().Yield(); //flush events
        count++;
    }while(std::next_permutation(anagram.begin(),anagram.end()));

    if(!proceed){
        pDiag->Destroy();
        cDiag->Destroy();
    }
    else{
        if(pDiag) pDiag->Destroy();
        cDiag->ShowModal();
        cDiag->Destroy();
    }
    this->dictionary=new sqlite::database("./dicts.sqlite");

}
void SmartFrame::OnAddDict(wxCommandEvent& WXUNUSED(event))
{
    DictionaryDiag * addDictDiag=new DictionaryDiag(wxT("Add Dictionary"),this);
    addDictDiag->ShowModal();
    addDictDiag->Destroy();
}
void SmartFrame::OnRemoveDict(wxCommandEvent& WXUNUSED(event))
{
    DictionaryDiag * addDictDiag=new DictionaryDiag(wxT("Remove Dictionary"),this,true);
    addDictDiag->ShowModal();
    addDictDiag->Destroy();
}
void SmartFrame::OnHelp(wxCommandEvent& WXUNUSED(event))
{
   wxMessageDialog * diag=new wxMessageDialog(this,wxT("Type the character set to get anagrams or contact the developer for more information: Ganyaf@gmail.com"),wxT("Help"),wxOK);
   diag->ShowModal();
}
void SmartFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
   wxMessageDialog * diag=new wxMessageDialog(this,wxT("Smart Anagram 1.0 by Francis Ganya : Ganyaf@gmail.com"),wxT("About"),wxOK);
   diag->ShowModal();
}
inline bool SmartFrame ::runValidation(wxString anagram,wxString table,bool& validate)
{

        int count = 0; //number of entries in the dictionary
        try{
        std::string query="select count(*) from "+(std::string)table+" where word='"+(std::string)(anagram.Capitalize())+
                          "' or word='"+(std::string)(anagram.Upper())+
                          "' or word='"+(std::string)(anagram.Lower())+"';" ;
            *(this->dictionary)<< query >> count;
        }
		catch(sqlite::sqlite_exception& e){
            wxMessageBox("A Database error occured.Subsequent words will not be validated.",wxT("Error"));
            validate=false;
		}
		return (count!=0) ? true : false;
}

bool SmartFrame::validateAnagram()
{
    return (this->getValidOnly->IsChecked());
}
int SmartFrame::getMaxPermutations(wxString charSet)
{
    //compute maximum number of anagrams that can be generated from the character set
    int maxAnagrams=1;
    for(int i=charSet.length();i>=1;i--)
    {
        maxAnagrams*=i;
    }
    return maxAnagrams;
}
