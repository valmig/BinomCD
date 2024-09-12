#ifndef BINOMCD_H_INCLUDED
#define BINOMCD_H_INCLUDED

#include <wx/frame.h>
#include <string>
#include <val_filesys.h>
#include <Glist.h>
#include <d_array.h>

extern wxFrame *MyFrame;
extern std::string iconpath,settingsdir,valdir,filesep,settingsfile, alticonpath;

enum test_type{L,R,B};
enum event_type{COMPCD,TEST};

class MyThreadEvent;

wxDECLARE_EVENT(MY_EVENT, MyThreadEvent);

class MyThreadEvent: public wxThreadEvent
{
public:
	MyThreadEvent(wxEventType commandType, int id = COMPCD)
        		:  wxThreadEvent(commandType, id) { }

	// You *must* copy here the data to be transported
	MyThreadEvent(const MyThreadEvent& event)
        		:  wxThreadEvent(event) { this->SetMessage(event.GetMessage()); this->SetNumbers(event.k1, event.k2); }

	// Required for sending with wxPostEvent()
	wxEvent* Clone() const { return new MyThreadEvent(*this); }

	std::string GetMessage() const { return message; }
	void SetMessage(const std::string &s) { message = s; }
	void SetNumbers(int l, int r) {k1 = l; k2 = r;}
	int GetLeftNumber() const {return k1;}
	int GetRightNumber() const {return k2;}

private:
	std::string message;
	int k1,k2;
};

// ----------------------------------------------------------------------------------------


val::Glist<std::string> getwordsfromstring(const std::string &sf,const val::d_array<char>& separators, int emptywords = 0,
                                           const val::d_array<char> &ignore = val::d_array<char>());

void computeCD(const double &p,int n,int k1,int k2,wxString &s_output);

void hypothesentest(double p, int n, double alpha, test_type type);

#endif // BINOMCD_H_INCLUDED
