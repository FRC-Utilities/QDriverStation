/*
 * Copyright (c) 2015-2017 Alex Spataru <alex_spataru@outlook.com>
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

#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QApplication>
#include <QJoysticks/SDL_Joysticks.h>

/**
 * Holds a generic mapping to be applied to joysticks that have not been mapped
 * by the SDL project or by the database.
 *
 * This mapping is different on each supported operating system.
 */
static QString GENERIC_MAPPINGS;

/**
 * Load a different generic/backup mapping for each operating system.
 */
#ifdef SDL_SUPPORTED
    #if defined Q_OS_WIN
        #define GENERIC_MAPPINGS_PATH ":/QJoysticks/SDL/GenericMappings/Windows.txt"
    #elif defined Q_OS_MAC
        #define GENERIC_MAPPINGS_PATH ":/QJoysticks/SDL/GenericMappings/OSX.txt"
    #elif defined Q_OS_LINUX && !defined Q_OS_ANDROID
        #define GENERIC_MAPPINGS_PATH ":/QJoysticks/SDL/GenericMappings/Linux.txt"
    #endif
#endif

SDL_Joysticks::SDL_Joysticks (QObject* parent) : QObject (parent)
{
    m_tracker = -1;

#ifdef SDL_SUPPORTED
    if (SDL_Init (SDL_INIT_HAPTIC | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER)) {
        qDebug() << "Cannot initialize SDL:" << SDL_GetError();
        qApp->quit();
    }

    QFile database (":/QJoysticks/SDL/Database.txt");
    if (database.open (QFile::ReadOnly)) {
        while (!database.atEnd()) {
            QString line = QString::fromUtf8 (database.readLine());
            SDL_GameControllerAddMapping (line.toStdString().c_str());
        }

        database.close();
    }

    QFile genericMappings (GENERIC_MAPPINGS_PATH);
    if (genericMappings.open (QFile::ReadOnly)) {
        GENERIC_MAPPINGS = QString::fromUtf8 (genericMappings.readAll());
        genericMappings.close();
    }

    QTimer::singleShot (100, Qt::PreciseTimer, this, &SDL_Joysticks::update);
#endif
}

SDL_Joysticks::~SDL_Joysticks()
{
#ifdef SDL_SUPPORTED
    SDL_Quit();
#endif
}

/**
 * Returns a list with all the registered joystick devices
 */
QList<QJoystickDevice*> SDL_Joysticks::joysticks()
{
    QList<QJoystickDevice*> list;

#ifdef SDL_SUPPORTED
    for (int i = 0; i < SDL_NumJoysticks(); ++i)
        list.append (getJoystick (i));
#endif

    return list;
}

/**
 * Based on the data contained in the \a request, this function will instruct
 * the appropriate joystick to rumble for
 */
void SDL_Joysticks::rumble (const QJoystickRumble& request)
{
#ifdef SDL_SUPPORTED
    SDL_Haptic* haptic = SDL_HapticOpen (request.joystick->id);

    if (haptic) {
        SDL_HapticRumbleInit (haptic);
        SDL_HapticRumblePlay (haptic, request.strength, request.length);
    }
#else
    Q_UNUSED (request);
#endif
}

/**
 * Polls for new SDL events and reacts to each event accordingly.
 */
void SDL_Joysticks::update()
{
#ifdef SDL_SUPPORTED
    SDL_Event event;

    while (SDL_PollEvent (&event)) {
        switch (event.type) {
        case SDL_JOYDEVICEADDED:
            configureJoystick (&event);
            break;
        case SDL_JOYDEVICEREMOVED:
            SDL_JoystickClose (SDL_JoystickOpen (event.jdevice.which));
            SDL_GameControllerClose (SDL_GameControllerOpen (event.cdevice.which));
            emit countChanged();
            break;
        case SDL_CONTROLLERAXISMOTION:
            emit axisEvent (getAxisEvent (&event));
            break;
        case SDL_JOYBUTTONUP:
            emit buttonEvent (getButtonEvent (&event));
            break;
        case SDL_JOYBUTTONDOWN:
            emit buttonEvent (getButtonEvent (&event));
            break;
        case SDL_JOYHATMOTION:
            emit POVEvent (getPOVEvent (&event));
            break;
        }
    }

    QTimer::singleShot (10, Qt::PreciseTimer, this, &SDL_Joysticks::update);
#endif
}

/**
 * Checks if the joystick referenced by the \a event can be initialized.
 * If not, the function will apply a generic mapping to the joystick and
 * attempt to initialize the joystick again.
 */
void SDL_Joysticks::configureJoystick (const SDL_Event* event)
{
#ifdef SDL_SUPPORTED
    if (!SDL_IsGameController (event->cdevice.which)) {
        SDL_Joystick* js = SDL_JoystickOpen (event->jdevice.which);

        if (js) {
            char guid [1024];
            SDL_JoystickGetGUIDString (SDL_JoystickGetGUID (js), guid, sizeof (guid));

            QString mapping = QString ("%1,%2,%3")
                              .arg (guid)
                              .arg (SDL_JoystickName (js))
                              .arg (GENERIC_MAPPINGS);

            SDL_GameControllerAddMapping (mapping.toStdString().c_str());
            SDL_JoystickClose (js);
        }
    }

    SDL_GameControllerOpen (event->cdevice.which);

    ++m_tracker;
    emit countChanged();
#else
    Q_UNUSED (event);
#endif
}

/**
 * Returns a joystick ID compatible with the \c QJoysticks system.
 * SDL assigns an ID to each joystick based on the order that they are attached,
 * but it does not decrease the ID counter when a joystick is removed.
 *
 * As noted earlier, the \c QJoysticks maintains an ID system similar to the
 * one used by a \c QList, since it eases the operation with most Qt classes
 * and widgets.
 */
int SDL_Joysticks::getDynamicID (int id)
{
#ifdef SDL_SUPPORTED
    id = abs (m_tracker - (id + 1));

    if (id >= SDL_NumJoysticks())
        id -= 1;

#endif

    return id;
}

/**
 * Returns the josytick device registered with the given \a id.
 * If no joystick with the given \a id is found, then the function will warn
 * the user through the console.
 */
QJoystickDevice* SDL_Joysticks::getJoystick (int id)
{
#ifdef SDL_SUPPORTED
    QJoystickDevice* joystick = new QJoystickDevice;
    SDL_Joystick* sdl_joystick = SDL_JoystickOpen (id);
    joystick->id = getDynamicID (id);

    if (sdl_joystick) {
        joystick->blacklisted = false;
        joystick->name = SDL_JoystickName (sdl_joystick);

        /* Get joystick properties */
        int povs = SDL_JoystickNumHats (sdl_joystick);
        int axes = SDL_JoystickNumAxes (sdl_joystick);
        int buttons = SDL_JoystickNumButtons (sdl_joystick);

        /* Initialize POVs */
        for (int i = 0; i < povs; ++i)
            joystick->povs.append (0);

        /* Initialize axes */
        for (int i = 0; i < axes; ++i)
            joystick->axes.append (0);

        /* Initialize buttons */
        for (int i = 0; i < buttons; ++i)
            joystick->buttons.append (false);
    }

    else
        qWarning() << Q_FUNC_INFO << "Cannot find joystick with id:" << id;

    return joystick;
#else
    Q_UNUSED (id);
    return NULL;
#endif
}

/**
 * Reads the contents of the given \a event and constructs a new
 * \c QJoystickPOVEvent to be used with the \c QJoysticks system.
 */
QJoystickPOVEvent SDL_Joysticks::getPOVEvent (const SDL_Event* sdl_event)
{
    QJoystickPOVEvent event;

#ifdef SDL_SUPPORTED
    event.pov = sdl_event->jhat.hat;
    event.joystick = getJoystick (sdl_event->jdevice.which);

    switch (sdl_event->jhat.value) {
    case SDL_HAT_RIGHTUP:
        event.angle = 45;
        break;
    case SDL_HAT_RIGHTDOWN:
        event.angle = 135;
        break;
    case SDL_HAT_LEFTDOWN:
        event.angle = 225;
        break;
    case SDL_HAT_LEFTUP:
        event.angle = 315;
        break;
    case SDL_HAT_UP:
        event.angle = 0;
        break;
    case SDL_HAT_RIGHT:
        event.angle = 90;
        break;
    case SDL_HAT_DOWN:
        event.angle = 180;
        break;
    case SDL_HAT_LEFT:
        event.angle = 270;
        break;
    default:
        event.angle = -1;
        break;
    }
#else
    Q_UNUSED (sdl_event);
#endif

    return event;
}

/**
 * Reads the contents of the given \a event and constructs a new
 * \c QJoystickAxisEvent to be used with the \c QJoysticks system.
 */
QJoystickAxisEvent SDL_Joysticks::getAxisEvent (const SDL_Event* sdl_event)
{
    QJoystickAxisEvent event;

#ifdef SDL_SUPPORTED
    event.axis = sdl_event->caxis.axis;
    event.value = static_cast<qreal> (sdl_event->caxis.value) / 32767;
    event.joystick = getJoystick (sdl_event->cdevice.which);
#else
    Q_UNUSED (sdl_event);
#endif

    return event;
}

/**
 * Reads the contents of the given \a event and constructs a new
 * \c QJoystickButtonEvent to be used with the \c QJoysticks system.
 */
QJoystickButtonEvent SDL_Joysticks::getButtonEvent (const SDL_Event*
        sdl_event)
{
    QJoystickButtonEvent event;

#ifdef SDL_SUPPORTED
    event.button = sdl_event->jbutton.button;
    event.pressed = sdl_event->jbutton.state == SDL_PRESSED;
    event.joystick = getJoystick (sdl_event->jdevice.which);
#else
    Q_UNUSED (sdl_event);
#endif

    return event;
}
