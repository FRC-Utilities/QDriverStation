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

#ifndef FALCON_H
#define FALCON_H

#include <QUrl>
#include <QApplication>

#include <QQmlEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QQmlComponent>

#if defined (Q_OS_ANDROID)
#include <QScreen>
#include <QtAndroidExtras>
#endif

namespace Falcon
{
/**
 * Calculates the DPI in Android devices, so that the interface does not look
 * too small on some phones or tables.
 *
 * Check https://bugreports.qt-project.org/browse/QTBUG-35701 for more info
 *
 * To use it, simply add a context property in your QmlEngine object and change
 * the scale ratio of the application in your main.qml file
 */
extern double GetDpi()
{
#if defined (Q_OS_ANDROID)
    QAndroidJniEnvironment env;
    QAndroidJniObject activity = QtAndroid::androidActivity();
    QAndroidJniObject resources = activity.callObjectMethod ("getResources",
                                  "()Landroid/content/res/Resources;");

    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();

        return 1.0;
    }

    QAndroidJniObject displayMetrics =
        resources.callObjectMethod ("getDisplayMetrics",
                                    "()Landroid/util/DisplayMetrics;");

    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();

        return 1.0;
    }

    return displayMetrics.getField<int> ("densityDpi") / 160.f;
#else
    return 1.0;
#endif
}

/**
 * Returns \c true if the target device is considered a mobile device
 */
extern bool Mobile()
{
#if defined Q_OS_IOS | defined Q_OS_ANDROID
    return true;
#else
    return false;
#endif
}

/**
 * Creates and configures a QML application with the selected QML \a file
 */
extern int StartApp (int argc, char* argv[], QString file)
{
    QApplication app (argc, argv);

    QQmlEngine* engine = new QQmlEngine();
    engine->rootContext()->setContextProperty ("c_dpi", GetDpi());
    engine->rootContext()->setContextProperty ("c_mobile", Mobile());

    QQmlComponent* component = new QQmlComponent (engine);
    component->loadUrl (QUrl (file));

#if defined Q_OS_ANDROID | defined Q_OS_IOS
    qobject_cast<QQuickWindow*> (component->create())->showMaximized();
#else
    qobject_cast<QQuickWindow*> (component->create())->showNormal();
#endif

    return app.exec();
}
}

#endif // FALCON_H

