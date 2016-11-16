/*HEADER ******************************************************************
*
* FILENAME : inquire_object.c
*
* SCCS Reference :
*    %W%    %G%
*
* DESCRIPTION :
*
* PUBLIC FUNCTIONS :-
*
* NOTES :
*
* REFERENCES :
*
**HEADER ******************************************************************/

/*---- compilation control switches --------------------------------------*/

/*INCLUDE FILES ***********************************************************/

/*---- system and platform files -----------------------------------------*/

#include <stdlib.h>
#include <stdio.h>

/*---- program files -----------------------------------------------------*/

#include "e3user.h" 

/*INCLUDE FILES ***********************************************************/

/*---- data structures ---------------------------------------------------*/

/*---- data descriptions -------------------------------------------------*/

#define    TRUE 1
#define    FALSE 0

/*---- macros ------------------------------------------------------------*/

/*FUNCTION *****************************************************************
*
* DESCRIPTION :
*    Function to add User information to inquire object window.
*
* INPUT PARAMETERS :
*    id        Id of object,
*    type    type of object ( ie Joint, Bolt, etc )
*
* OUTPUT PARAMETERS :
*    None
*
* INPUT GLOBALS :
*    None
*
* OUTPUT GLOBALS :
*    None
*
* RETURN :
*    Always 1.
*
* NOTES :
*    Function is not directly called byuser joints but run by xsteel during
*    object inquire.
*
*    xs_add_info_text - is used to put the text into window.
*
**FUNCTION ****************************************************************/

int xs_inquire_object_info(int id, int type)
{
    FILE *Joint_Data = NULL;
    FILE *Tmp_Data = NULL;

    char Grade1[21] = "";
    char Grade2[21] = "";
    char Field[82] = "";
    char Path[1280] = ""; /* 81 -> 1280 - 170599 the */
    char TmpPath[1280] = ""; /* 81 -> 1280 - 170599 the */
    char Display_String[82] = "";
    char Directory_Name[1024] = "";
    char Database_Name[255] = "";

    int joint_no;
    int prim;
    int nosec;
    int nogrades;
    int idsecs[100];
    int length;
    int first_mark = TRUE;
    int Same_Grade = TRUE;

    xs_object_t prim_obj;
    xs_object_t sec1_obj;
    xs_object_t sec2_obj;
    
    xs_part_t prim_info;
    xs_part_t sec1_info;
    xs_part_t sec2_info;

    xs_assembly_t prim_ass;
    xs_assembly_t sec1_ass;
    xs_assembly_t sec2_ass;
/*
** Ensure strings null terminated if extend beyonf limits
*/
    Field[81] = '\0';        
    Display_String[81] = '\0';
           
    if (type == 3) 
    {
        xs_get_database_directory( Directory_Name, Database_Name );

        sprintf( Path, "%s/DesignFiles/j%i.dat", Directory_Name, id ); 
        if(!(Joint_Data  = fopen( Path, "r" )))
        {
            sprintf( Path, "%s/j%i.txt", Directory_Name, id ); 
            Joint_Data  = fopen( Path, "r" );
        }
        if( !Joint_Data )
        {
            sprintf( Display_String,"No Design Information for Joint %i.", id);
            xs_add_info_text( "" );
            xs_add_info_text( Display_String );
        }
        else
        {
            sprintf( TmpPath, "%s/j%i.tmp", Directory_Name, id ); 
            Tmp_Data = fopen( TmpPath, "w" );

            /*
                Get Data for Primary and Secondary members
            */

            xs_get_joint_prim_and_secs( id, &joint_no, &prim, &nosec, idsecs );
            xs_select_part( prim, &prim_info );
            xs_select_object( prim, &prim_obj );
            xc_select_assembly( prim_obj.assembly, &prim_ass );
            if( nosec > 0 )
            {
                xs_select_part( idsecs[0], &sec1_info );
                xs_select_object( idsecs[0], &sec1_obj );
                xc_select_assembly( sec1_obj.assembly, &sec1_ass );
                if( nosec > 1 )
                {
                    xs_select_part( idsecs[1], &sec2_info );
                    xs_select_object( idsecs[1], &sec2_obj );
                    xc_select_assembly( sec2_obj.assembly, &sec2_ass );
                }
            }
            
            /*
                Find the Grade of the Primary and Secondary Members
                If these differ from current Grades do not display 
                Design Information.
            */

            while( fgets( Display_String, 81, Joint_Data ) )
            {
                sscanf( Display_String,"%s", Field );
                if( ! strcmp( Field, "Grade" ) )
                {
                    nogrades = sscanf( strchr( Display_String, ':' ) + 1, "%s%s",
                                       Grade1,Grade2);
                    if( first_mark )
                    {
                        if( strcmp( Grade1, prim_info.mat ) )
                        {
                            Same_Grade = FALSE;
                        }
                        first_mark = FALSE;
                        if ( nosec == 0 ) break;
                    }
                    else
                    {
                        if( strcmp( Grade1, sec1_info.mat ) )
                        {
                            Same_Grade = FALSE;
                        }
                        if( nosec > 1 && strcmp( Grade2, sec2_info.mat ) )
                        {
                            Same_Grade = FALSE;
                        }
                        break;
                    }
                    strcpy( Grade1,"\0" ); strcpy( Grade2,"\0" );
                }
                strcpy( Field, "\0" );
            }
            first_mark = TRUE;
            if( Same_Grade == FALSE )
            {
                sprintf( Display_String,
                         "One or More members has changed grade." );
                xs_add_info_text( "" );
                xs_add_info_text( Display_String );
                sprintf( Display_String, "Joint Must Be reapplied");
                xs_add_info_text( "" );
                xs_add_info_text( Display_String );
                return( 1 );
            }


            fclose( Joint_Data );
            Joint_Data  = fopen( Path, "r" );

            /*
                Is the Numbering Up to date.
            */

            if( !xs_numbering_up_to_date( prim ) )
            {
                sprintf( Display_String,"Numbering Not Up to date." );
                xs_add_info_text( "" );
                xs_add_info_text( Display_String );
            }

            xs_add_info_text( "" );
            while( fgets( Display_String, 81, Joint_Data ) )
            {
                if ( strchr( Display_String, '\n') )
                    strcpy( strchr( Display_String, '\n'), "\0" );
                sscanf( Display_String,"%s", Field );
                if( ! strcmp( Field, "Mark" ) )
                {
                    strcpy( strchr( Display_String, ':' ) + 1 , "\0" );
                    if( first_mark )
                    {
                        sprintf( Display_String, "%s%10s/%i",
                                 Display_String, prim_ass.prefix,
                                 prim_ass.pos_no + prim_ass.start_no - 1 );
                        first_mark = FALSE;
                    }
                    else
                    {
                        
                        sprintf( Display_String, "%s%10s/%i",
                                 Display_String, sec1_ass.prefix,
                                 sec1_ass.pos_no + sec1_ass.start_no - 1 );
                        if( nosec > 1 )
                        {
                            length = (int)strlen(Display_String);
                            sprintf( Display_String, "%s%*s/%i",
                                     Display_String, ( 45 - length),
                                     sec2_ass.prefix,
                                     sec2_ass.pos_no + sec2_ass.start_no - 1 );
                        }
                        sprintf( Display_String, "%s", Display_String );
                    }
                    strcpy( Field, "\0" );
                }
                fprintf( Tmp_Data, "%s\n", Display_String );
                xs_add_info_text( Display_String );
            }
            if ( Joint_Data ) fclose( Joint_Data );
            if ( Tmp_Data ) fclose( Tmp_Data );

            /*
                Copy Tmp file to j<id>.txt so if file is printed 
                Marks are displayed, and delete tmp file.
            */

            Joint_Data  = fopen( Path, "w" );
            Tmp_Data = fopen( TmpPath, "r" );
	    if( Joint_Data && Tmp_Data )
            {
                while( fgets( Display_String, 81, Tmp_Data ) )
                {
                    fprintf( Joint_Data, "%s", Display_String );
                }
            }
            if ( Joint_Data ) fclose( Joint_Data );
            if ( Tmp_Data ) 
            {
                fclose( Tmp_Data );
                remove( TmpPath );
            }
        }
    }

    return( 1 );
}
