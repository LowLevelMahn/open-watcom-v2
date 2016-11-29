/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WGML message definition.
*               adapted from wlink file (bld\wl\c\wlnkmsg.c)
*
****************************************************************************/


#include "wgml.h"
#include "wio.h"
#include "wressetr.h"
#include "wresset2.h"
#include "wreslang.h"

#include "clibext.h"

HANDLE_INFO hInstance;

static unsigned MsgShift;               // 0 = english, 1000 for japanese
static bool     res_failure = true;


/***************************************************************************/
/*  Special seek routine, because resource file does not start at offset 0 */
/*  of wgml.exe                                                            */
/***************************************************************************/

static long res_seeek( WResFileID handle, long position, int where )
/******************************************************************/
{
    if( where == SEEK_SET ) {
        return( lseek( handle, position + WResFileShift, where ) - WResFileShift );
    } else {
        return( lseek( handle, position, where ) );
    }
}

WResSetRtns( open, close, read, write, res_seeek, tell, mem_alloc, mem_free );

/***************************************************************************/
/*  initialize messages from resource file                                 */
/***************************************************************************/

bool init_msgs( void )
{
    char        fname[_MAX_PATH];

    hInstance.handle = WRES_NIL_HANDLE;
    if( _cmdname( fname ) != NULL && !OpenResFile( &hInstance, fname ) ) {
        res_failure = false;
        if( !FindResources( &hInstance ) && !InitResources( &hInstance ) ) {
            MsgShift = _WResLanguage() * MSG_LANG_SPACING;
            if( get_msg( ERR_DUMMY, fname, sizeof( fname ) ) ) {
                return( true );
            }
        }
        fini_msgs();
    }
    out_msg( "Resources not found\n" );
    res_failure = true;
    g_suicide();
    return( false );
}


/***************************************************************************/
/*  get a msg text string                                                  */
/***************************************************************************/

bool get_msg( msg_ids resid, char *buff, size_t buff_len )
{
    if( res_failure || WResLoadString( &hInstance, resid + MsgShift, buff, buff_len ) <= 0 ) {
        buff[0] = '\0';
        return( false );
    }
    return( true );
}

/***************************************************************************/
/*  end of msg processing                                                  */
/***************************************************************************/

void fini_msgs( void )
{
    if( hInstance.handle != WRES_NIL_HANDLE ) {
        CloseResFile( &hInstance );
        hInstance.handle = WRES_NIL_HANDLE;
    }
}
