/*
 * This code is based on the answer provided at:
 * http://stackoverflow.com/questions/10110905/simple-wave-generator-with-sdl-in-c
 */

#ifndef _QDS_BEEPER_H
#define _QDS_BEEPER_H

#include <QObject>

/**
 * \brief Uses SDL to generate telephone-like sound tones on the fly
 */
class Beeper : public QObject
{
    Q_OBJECT

public:
    explicit Beeper();
    ~Beeper();

    void generateSamples (qint16* stream, int length);

public slots:
    void setEnabled (bool enabled);
    void beep (qreal frequency, int duration);

private:
    bool m_enabled;
    double m_angle;
};

#endif
