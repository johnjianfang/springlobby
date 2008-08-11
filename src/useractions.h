#ifndef USERACTIONS_HH_INCLUDED
#define USERACTIONS_HH_INCLUDED

#include <wx/string.h>
#include <wx/arrstr.h>
#include <map>

class wxColour;

const wxString m_actionNames[] = { _T("none"),_T("highlight"),_T("notify login"),_T("ignore"),_T("autokick"),
        _T("notify hosted battle"),_T("notify status change")};


class UserActions {

public:
    UserActions();
    ~UserActions();

     enum ActionType {
       ActNone = 1,
       ActHighlight = 2,
       ActNotifLogin = 4,
       ActIgnore = 8,
       ActAutokick = 16,
       ActNotifBattle = 32,
       ActNotifStatus = 64
     };
    static const int m_numActions = sizeof(m_actionNames) / sizeof(wxString);
    bool DoActionOnUser( const ActionType action, const wxString& name ) ;
    wxSortedArrayString GetGroupNames() const;
    void AddUserToGroup( const wxString& group, const wxString& name );
    void AddGroup(const wxString& name );
    void DeleteGroup(const wxString& name );
    void RemoveUser(const wxString& name );
    void ChangeAction( const wxString& group, const ActionType action, bool add = true );
    ActionType GetGroupAction( const wxString& group );
    wxString GetGroupOfUser( const wxString& user );
    void SetGroupColor( const wxString& group, const wxColour& color );
    wxColour GetGroupColor( const wxString& group );
    bool IsKnown( const wxString& name );

protected:
    //lotsa maps to keep runtime finds, etc ti a minimum
    typedef std::map<wxString,wxSortedArrayString> GroupMap;
    /// groupname --> array of people in the group
    GroupMap m_groupMap;
    typedef std::map<wxString,ActionType> GroupActionMap;
    /// groupname --> ActionType for that group
    GroupActionMap m_groupActions;
    typedef std::map<ActionType,wxSortedArrayString> ActionGroupsMap;
    /// ActionType --> array of groups with that actiontype
    ActionGroupsMap m_actionsGroups;
    typedef std::map<ActionType,wxSortedArrayString> ActionPeopleMap;
    /// ActionType --> array of people with that actiontype
    ActionPeopleMap m_actionsPeople;
    ///nickname --> group map (we don't allow users to be in more than one group
    typedef std::map<wxString,wxString> PeopleGroupMap;
    PeopleGroupMap m_peopleGroup;
    ///list all known users in groups
    wxSortedArrayString m_knownUsers;

    //reload all maps and stuff
    void Init();

    wxSortedArrayString m_groupNames;
};

UserActions& useractions();
#endif // USERACTIONS_HH_INCLUDED