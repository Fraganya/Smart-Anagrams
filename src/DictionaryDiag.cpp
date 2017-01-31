#include "DictionaryDiag.h"
#include "wx/filefn.h"

#include <string>
#include <sstream>
#include <sqlite_modern_cpp.h>


DictionaryDiag::DictionaryDiag(const wxString& title,wxFrame * parent):
wxDialog(parent,wxID_ANY,title,wxDefaultPosition,wxSize(300,200))
{
    wxPanel * mPanel=new wxPanel(this,wxID_ANY);

    wxBoxSizer * mBox=new wxBoxSizer(wxVERTICAL);
    wxBoxSizer * upperBox=new wxBoxSizer(wxVERTICAL);
    wxBoxSizer * lowerBox=new wxBoxSizer(wxHORIZONTAL);


    wxStaticText * helperLabel=new wxStaticText(mPanel,wxID_ANY,wxT("Language"));
    this->inputBox=new wxTextCtrl(mPanel,wxID_ANY);
    wxStaticText * tableHelper=new wxStaticText(mPanel,wxID_ANY,wxT("Table"));
    this->tableBox=new wxTextCtrl(mPanel,wxID_ANY);

    wxStaticText * dictLabel=new wxStaticText(mPanel,wxID_ANY,wxT("Dictionary File"));
    this->file=new wxFilePickerCtrl(mPanel,wxID_ANY,"",wxT("Select Dictionary"),"Sqlite files (*.sqlite) | *.sqlite");

    wxButton * addBtn=new wxButton(mPanel,wxID_ANY,wxT("&Add"));

    upperBox->Add(helperLabel,0,wxEXPAND);
    upperBox->Add(inputBox,0,wxEXPAND);
    upperBox->Add(tableHelper,0,wxEXPAND);
    upperBox->Add(tableBox,0,wxEXPAND);
    upperBox->Add(dictLabel,0,wxEXPAND);
    upperBox->Add(file,0,wxEXPAND);

    lowerBox->Add(addBtn);

    mBox->Add(upperBox,0,wxEXPAND | wxLEFT | wxTOP | wxRIGHT,10);
    mBox->Add(-1,10);
    mBox->Add(lowerBox,0,wxLEFT,10);
    Connect(addBtn->GetId(),wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(DictionaryDiag::DictAdd));
    mPanel->SetSizer(mBox);
    this->Centre();
}

DictionaryDiag::DictionaryDiag(const wxString& title,wxFrame * parent,bool removeDiag):
wxDialog(parent,wxID_ANY,title,wxDefaultPosition,wxSize(400,200))
{
    wxPanel * mPanel=new wxPanel(this,wxID_ANY);

    wxBoxSizer * mBox=new wxBoxSizer(wxHORIZONTAL);
    this->lanList=new wxListBox(mPanel,wxID_ANY);
    wxButton  * delBtn =new wxButton(mPanel,wxID_ANY,wxT("Remove"));

    try{
          sqlite::database maindb("./dicts.sqlite");
          maindb<<"SELECT language from dictionaries;">>[&](std::string language){
          this->lanList->Append(language);
         };
    }
    catch(sqlite::sqlite_exception& e){
          wxMessageBox(e.what(),wxT("Error"),wxOK | wxICON_INFORMATION);
    }

     Connect(delBtn->GetId(),wxEVT_COMMAND_BUTTON_CLICKED,wxCommandEventHandler(DictionaryDiag::DictRemove));
     mBox->Add(lanList,3,wxALL | wxEXPAND,5);
     mBox->Add(delBtn,1,wxRIGHT | wxTOP,3);
     mPanel->SetSizer(mBox);
     this->Centre();
}
void DictionaryDiag::DictAdd(wxCommandEvent& WXUNUSED(event))
{
    std::string language=(std::string)this->inputBox->GetValue();
    std::string targetTable=(std::string)this->tableBox->GetValue();
    std::string filePath=(std::string)this->file->GetPath();
    std::string errorMsg("");
    int errorCount=0;
    if(!wxFile::Exists(filePath)){
            errorCount++;
            errorMsg="File does not exist";
    }
    else{
        try{
            sqlite::database * db=new sqlite::database(filePath);
            //check if table really exists
            int count=0;
            (*db)<<"SELECT count(type) from sqlite_master where type='table' and name='"+targetTable+"';">>count;
            if(count==0){
                errorMsg="Table ["+targetTable+"] does not exists";
                errorCount++;
            }
            else{
                sqlite::database maindb("./dicts.sqlite");
                (maindb)<<"INSERT INTO dictionaries(language,filename,table_name) values(?,?,?);"<<language<<language+"_dict.sqlite"
                        <<targetTable;
                if(wxCopyFile(filePath,"./dictionaries/"+language+"_dict.sqlite")){
                     this->inputBox->Clear();
                     this->tableBox->Clear();
                     this->file->GetTextCtrl()->Clear();
                     wxMessageBox(wxT("Dictionary successfully added!Restart App for changes."),wxT("Success"),wxOK | wxICON_INFORMATION);
                }
                else{
                    errorCount++;
                    errorMsg="Could not copy dictionary file";
                }

            }
        }
        catch(sqlite::sqlite_exception& e){
           errorMsg=e.what();
           errorMsg+="/The language dictionary already exists";
           errorCount++;
        }
    }

    if(errorCount!=0){
        wxMessageDialog * diag=new wxMessageDialog(this,errorMsg,wxT("Error"),wxOK | wxICON_ERROR);
        diag->ShowModal();
    }
}

void DictionaryDiag::DictRemove(wxCommandEvent& WXUNUSED(event))
{
    int selection=this->lanList->GetSelection();
    try{
        sqlite::database maindb("./dicts.sqlite");
        if(selection!=-1){
           std::string language=(std::string) this->lanList->GetString(selection);
           maindb<<"delete from dictionaries where language=?"<<language;
           if(wxRemoveFile("./dictionaries/"+language+"_dict.sqlite")){
             wxMessageBox(wxT("Dictionary successfully removed!Please Restart App"),wxT("Success"),wxOK | wxICON_INFORMATION);
           }
           else{
             wxMessageBox(wxT("Dictionary successfully removed but failed to remove file !"),wxT("Success"),wxOK | wxICON_ERROR);
           }
        this->lanList->Delete(selection);
        }
    }
    catch(sqlite::sqlite_exception& e){
          wxMessageBox(e.what(),wxT("Error"),wxOK | wxICON_INFORMATION);
    }
}
