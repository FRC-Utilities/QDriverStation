/*
 * Copyright (c) 2015 Alex Spataru <alex_spataru@outlook.com>
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

import QtQuick 2.0
import QtQuick.Window 2.0

Window {

    //--------------------------------------------------------------------------
    // Custom properties
    //--------------------------------------------------------------------------

    property alias q_app: _app
    property alias q_global: _global
    property alias q_toolbar: _toolbar

    property double scaleRatio: {
        if (typeof c_dpi == 'undefined')
            return 1.0

        return c_dpi
    }

    //--------------------------------------------------------------------------
    // Functions
    //--------------------------------------------------------------------------

    function setToolbarText (text) {
        _toolbar.text = text
    }

    function topMargin() {
        return _toolbar.height + scale (8)
    }

    function scale (input) {
        return input * scaleRatio
    }

    function getSize (string) {
        switch (string) {
        case "small":
            return scale (12)
        case "medium":
            return scale (14)
        case "large":
            return scale (18)
        }

        return scale (12)
    }

    //--------------------------------------------------------------------------
    // Pproperties
    //--------------------------------------------------------------------------

    id: _app
    color: "#fff"

    //--------------------------------------------------------------------------
    // Objects
    //--------------------------------------------------------------------------

    QtObject {
        id: _global

        property string font: "OpenSans"

        property var icon: FontLoader {
            source: "qrc:/falcon/fonts/FontAwesome.ttf"
        }

        property var bold: FontLoader {
            source: "qrc:/falcon/fonts/OpenSans-Bold.ttf"
        }

        property var regular: FontLoader {
            source: "qrc:/falcon/fonts/OpenSans-Regular.ttf"
        }
    }

    Toolbar {
        id: _toolbar
    }
}
