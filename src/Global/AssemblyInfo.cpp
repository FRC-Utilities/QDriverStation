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

#define d_Name    "QDriverStation"
#define d_Major   "0"
#define d_Minor   "1"
#define d_Build   "4"
#define d_State   "RC"
#define d_Company "WinT 3794"
#define d_Domain  "wint3794.org"

QString AssemblyInfo::name() {
    return d_Name;
}

QString AssemblyInfo::major() {
    return d_Major;
}

QString AssemblyInfo::minor() {
    return d_Minor;
}

QString AssemblyInfo::build() {
    return d_Build;
}

QString AssemblyInfo::state() {
    return d_State;
}

QString AssemblyInfo::version() {
    return QString ("%1.%2%3 %4").arg (major(),
                                       minor(),
                                       build(),
                                       state());
}

QString AssemblyInfo::organization() {
    return d_Company;
}

QString AssemblyInfo::organizationDomain() {
    return d_Domain;
}

QString AssemblyInfo::buildDateTime() {
    return QObject::tr ("Built on %1 at %2").arg (__DATE__, __TIME__);
}
