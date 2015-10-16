

#ifndef MIC_DEFINITIONS_H
#define MIC_DEFINITIONS_H

// MIC_definitions.h

// For MICGM, we want this file to contain NO definitions needed by the components.

// Have changed some of the names to include "MIC_" at the front end to ensure there
// is no place in the components' code where these definitions are used.

//<<QA>>#include "ALLOWTRIGGERHOLDOFF.h"  add this back in

/////////////////////////////////////////////////////////////////////////////
// Defines
//nameshort is used for INI file name
//namelong  is used for window titles
#define MIC_NAMELONG				"Multi-Instrument Collect"
#define MIC_NAMEMEDIUM				"Multi-Inst. Collect"
#define MIC_NAMESHORT				"MIC"

#define INVALIDCHARS			"^&+|<>,=[]?*\\/;\""
#define INVALIDCHARSSUB			"^&+|<>,=[]?*\""
#define INVALIDFILENAMECHARS	"|<>*?:\\"

//#define MIC_MAX_EXTEND				7200
//#define MIC_BUMP_EXTEND				300

//number of characters to make password
#define MIC_PASSWORDLEN				20

//
// MIC default values for ini entries
// These are "tuneable".
//

#define BUTTONSWIDE_DEFAULT				5
#define BUTTONWIDTH_DEFAULT				87
#define BUTTONHEIGHT_DEFAULT			70
#define BUTTONFLAGSIZE_DEFAULT			12
#define DAYSBEFOREDELFILES_DEFAULT		90
#define DEFAULTCOPYPATH_DEFAULT			"A:"
#define DIALOGCLOSE_DEFAULT				30
#define DOINGNETWORK_DEFAULT			0
#define BEATNIK_DEFAULT					0
#define HIDEWINDOWS_DEFAULT				0
#define LOGARCHIVEACT_DEFAULT			false
#define MINUTESTOCHECKARCHIVE_DEFAULT	60
#define LONGNAMEFORM_DEFAULT			"AA%T%I_%y%m%d_%H%M%S"

#endif //MIC_DEFINITIONS_H