/*
 * This code is not mine, it was stealed from:
 * http://stackoverflow.com/questions/10110905/simple-wave-generator-with-sdl-in-c
 */

#ifndef _QDS_BEEPER_H
#define _QDS_BEEPER_H

#include <queue>
#include <QObject>

struct BeepObject
{
    double freq;
    int samplesLeft;
};

class Beeper : public QObject
{
public:
    Beeper();
    ~Beeper();
    void beep (double freq, int duration);
    void generateSamples (qint16* stream, int length);

private:
    double v;
    std::queue<BeepObject> beeps;
};

#endif
