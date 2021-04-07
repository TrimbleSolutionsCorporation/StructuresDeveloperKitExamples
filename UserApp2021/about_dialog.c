
#include "e3user.h"

/*------------------------------------------------------------------------*/
/* Structures application's About dialog can be displayed by pushing the  */
/* button on top of the connection toolbar.                               */
/*                                                                        */
/* The data shown on the About dialog is defined within the               */
/* applications xsDeveloperAboutInfo() function                           */
/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
int xsDeveloperAboutInfo(char aTitle[],     int TitleSize,
                         char aVersion[],   int VersionSize,
                         char aDate[],      int DateSize,
                         char aCopyright[], int CopyrightSize)
{
    STRCPY(aTitle    , "About UserApp application"                , TitleSize    );
    STRCPY(aVersion  , "Tekla Structures 16.0"                    , VersionSize  );
    STRCPY(aDate     , "15.12.2009"                               , DateSize     );
    STRCPY(aCopyright, "Copyright 1992-2010 Tekla Corporation.\n" , CopyrightSize);
    STRCAT(aCopyright, "All rights reserved."                     , CopyrightSize);

    return TRUE;
}
