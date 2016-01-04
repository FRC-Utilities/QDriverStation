/*
 * Copyright (c) 2015 WinT 3794 <http://wint3794.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "AssemblyInfo.h"

//=============================================================================
// AssemblyInfo::Name
//=============================================================================

QString AssemblyInfo::Name()
{
    return "QDriverStation";
}

//=============================================================================
// AssemblyInfo::Major
//=============================================================================

QString AssemblyInfo::Major()
{
    return "0";
}

//=============================================================================
// AssemblyInfo::Minor
//=============================================================================

QString AssemblyInfo::Minor()
{
    return "1";
}

//=============================================================================
// AssemblyInfo::Build
//=============================================================================

QString AssemblyInfo::Build()
{
    return "4";
}

//=============================================================================
// AssemblyInfo::Channel
//=============================================================================

QString AssemblyInfo::Channel()
{
    return "RC";
}

//=============================================================================
// AssemblyInfo::Version
//=============================================================================

QString AssemblyInfo::Version()
{
    return QString ("%1.%2%3 %4").arg (Major(),
                                       Minor(),
                                       Build(),
                                       Channel());
}

//=============================================================================
// AssemblyInfo::Organization
//=============================================================================

QString AssemblyInfo::Organization()
{
    return "WinT 3794";
}

//=============================================================================
// AssemblyInfo::OrganizationDomain
//=============================================================================

QString AssemblyInfo::OrganizationDomain()
{
    return "http://wint3794.org";
}

//=============================================================================
// AssemblyInfo::BuildDateTime
//=============================================================================

QString AssemblyInfo::BuildDateTime()
{
    return QObject::tr ("Built on %1 at %2").arg (__DATE__, __TIME__);
}
