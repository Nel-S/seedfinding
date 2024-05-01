#ifndef _SETTINGS_H
#define SETTINGS_H

#include "utilities/cubiomes/finders.h"

/* The list of all known stronghold generation points. (These are the values that would be returned by /locate [structure] stronghold.)
   You can list multiple ones, but beware that every single one must be exactly correct; incorrect values will lead to incorrect worldseeds or no worldseeds at all.*/
const Pos STRONGHOLD_GENERATION_POINTS[] = {
	{-1256, -1432},
};

// The version the world was originally generated under. This may not necessarily be the same as the version one's playing the world in.
const int VERSION = MC_1_16_5;

#endif