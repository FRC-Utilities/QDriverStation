/*
 * The Driver Station Library (LibDS)
 * Copyright (c) 2015-2017 Alex Spataru <alex_spataru@outlook>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the 'Software'),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "DS_Utils.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <windows.h>
    #ifndef __MINGW32__
        #pragma comment (lib, "user32.lib")
    #endif
#endif

/**
 * Returns a single byte value that represents the ratio between the
 * given \a value and the maximum number specified.
 */
uint8_t DS_FloatToByte (const float value, const float max)
{
    if (value != 0 && max != 0 && value <= max) {
        int percent = (int) ((value / max) * (0xFF / 2));
        return (uint8_t) (percent & 0xFF);
    }

    return 0;
}

/**
 * Returns a string in the format of NET.TE.AM.HOST, examples include
 *    - \c DS_GetStaticIP (10, 3794, 2) will return \c 10.37.94.2
 *    - \c DS_GetStaticIP (10, 18, 1) will return \c 10.0.18.1
 *    - and so on...
 *
 * This function is used to simplify the design and implementation of
 * different communication protocols.
 *
 * If you call this function outside the scope of the \c LibDS, remember to
 * call \c DS_FREE (STR() to avoid memory leaks.
 *
 * \param net the desired first octet of the IP
 * \param team the team number, used in second and third octets
 * \param host the host byte (or the last octet) of the IP
 */
DS_String DS_GetStaticIP (const int net, const int team, const int host)
{
    int te = team / 100;
    int am = team - (te * 100);
    return DS_StrFormat ("%d.%d.%d.%d", net, te, am, host);
}

/**
 * Shows a GUI message box if GUI options are enabled
 * during the compilation time.
 *
 * Otherwise, this function shall print to \a stderr
 */
void DS_ShowMessageBox (const DS_String* caption,
                        const DS_String* message,
                        const DS_IconType icon)
{
    assert (caption);
    assert (message);

    /* Get native strings */
    char* ccap = DS_StrToChar (caption);
    char* cmsg = DS_StrToChar (message);

#ifdef _WIN32
    /* Convert strings to wstrings */
    wchar_t* wcap = calloc (caption->len + 1, sizeof (wchar_t));
    wchar_t* wmsg = calloc (message->len + 1, sizeof (wchar_t));
    mbstowcs_s (NULL, wcap, caption->len + 1, ccap, caption->len);
    mbstowcs_s (NULL, wmsg, message->len + 1, cmsg, message->len);

    /* Get icon type */
    UINT ico;
    switch (icon) {
    case DS_ICON_ERROR:
        ico = MB_ICONERROR;
        break;
    case DS_ICON_INFORMATION:
        ico = MB_ICONINFORMATION;
        break;
    case DS_ICON_WARNING:
        ico = MB_ICONWARNING;
        break;
    default:
        ico = MB_ICONINFORMATION;
        break;
    }

    /* Display message box */
    MessageBox (NULL, wmsg, wcap, ico | MB_OK);

    /* Free buffers */
    DS_FREE (wcap);
    DS_FREE (wmsg);
#else
    /* Get icon text */
    char* cico;
    switch (icon) {
    case DS_ICON_ERROR:
        cico = "ERROR";
        break;
    case DS_ICON_INFORMATION:
        cico = "INFORMATION";
        break;
    case DS_ICON_WARNING:
        cico = "WARNING";
        break;
    default:
        cico = "INFORMATION";
        break;
    }

    /* Log message to stderr */
    fprintf (stderr, "%s: %s\n", cico, cmsg);

    /* Free icon text memory */
    DS_FREE (cico);
#endif

    /* Free resources */
    DS_FREE (ccap);
    DS_FREE (cmsg);
}
