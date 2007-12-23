#include "se_utils.h"

#include <string>
#include <sstream>
#include <wx/string.h>
#include "custom_dialogs.h"
#include <wx/utils.h>
#include <wx/log.h>
#include "se_settings.h"
#include "../springunitsynclib.h"

void toString(std::string& s, int t) {
			std::stringstream ss;
			ss << t;
			ss >> s;
}

int fromString(const std::string& s) {
        std::stringstream ss;
        ss << s;
        int r;
        ss >> r;
        return r;
}

void loadUnitsync()
{
	//should be done in susynclib()->Load
	//wxSetWorkingDirectory(OptionsHandler.getUsyncLoc().BeforeLast('\\'));
	
	try
	{
		wxCriticalSection m_lock;
		wxCriticalSectionLocker lock_criticalsection(m_lock);
		susynclib()->Load(OptionsHandler.getUsyncLoc());
	}
	catch (...)
	{
		//TODO log that
	}
}

void openUrl(const wxString& url)
{
    if ( !wxLaunchDefaultBrowser( url ) )
    {
      wxLogWarning( _T("can't launch default browser") );
      customMessageBox(SL_MAIN_ICON, _T("Couldn't launch browser. URL is: ") + url, _T("Couldn't launch browser.")  );
    }
}

