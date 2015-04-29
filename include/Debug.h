
// -----------------------------------------------------------------------------
// libNITRO -- Debug.h
// -----------------------------------------------------------------------------
//
// Debug macro
//

#ifndef DDG_DEBUG_H
#define DDG_DEBUG_H

#include <cstdio>

#if defined(DEBUG)
#define debug(x...) fprintf(stdout,x)
#else
#define debug(x...) 
#endif


#endif
