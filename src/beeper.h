/*
 * This code is not mine, it was stealed from:
 * http://stackoverflow.com/questions/10110905/simple-wave-generator-with-sdl-in-c
 */

#ifndef _QDS_BEEPER_H
#define _QDS_BEEPER_H

#include <queue>
#include <QObject>

///
/// Holds information regarding the current sound
///
struct BeepObject {
    float freq;
    int samplesLeft;
};

///
/// The \c Beeper class allows us to use SDL to generate custom sounds
/// and "beeps" on the fly, which eliminates the need to generate some
/// sound files and use \c QMediaPlayer to reproduce them.
///
/// In other words, the solution is lighter and more efficient.
///
class Beeper : public QObject {
    ///
    /// This macro is needed in order to interact with the SIGNAL/SLOT
    /// system and make the public functions accessible from QML
    ///
    Q_OBJECT

  public:
    ///
    /// Initializes the SDL sound system
    ///
    Beeper();

    ///
    /// Stops the SDL sound system to avoid bugs and weird issuess
    ///
    ~Beeper();

    ///
    /// Allows or inhibits the sound generation, based on the value given
    /// to the \a enabled argument.
    ///
    Q_INVOKABLE void setEnabled (bool enabled);

    ///
    /// Provided that the program did not disable the \c Beeper with the
    /// \c setEnabled() function, the \c beep() function will generate
    /// a tone with the given \a frequency (in Hertz) and
    /// specified duration (in milliseconds).
    ///
    Q_INVOKABLE void beep (float freq, int duration);

    ///
    /// This function is called when the class is initialized and has no
    /// use for the rest of the program. However, this function must
    /// remain public in order to let SDL do its magic when it is initialized.
    ///
    void generateSamples (qint16* stream, int length);
};

#endif
