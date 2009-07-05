#ifndef SPRINGLOBBY_HEADERGUARD_GLOBALEVENTS_H
#define SPRINGLOBBY_HEADERGUARD_GLOBALEVENTS_H

#include "events.h"

#include <map>

enum GlobalEventsTypes
{
  OnUnitsyncFirstTimeLoad,
  OnUnitsyncReloaded,

  OnTimerUpdates
};

typedef int GlobalEventData;

EventSender<GlobalEventData> &GetGlobalEventSender( GlobalEventsTypes cmd );

#endif // SPRINGLOBBY_HEADERGUARD_GLOBALEVENTS_H
