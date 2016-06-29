/*
 * Copyright (c) 2015-2016 Alex Spataru <alex_spataru@outlook.com>
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

#include "beeper.h"

/* Used for generating the sine wave and various operations */
#include <QtMath>
#include <QQueue>

/* Used for generating the sounds */
#include <SDL.h>
#include <SDL_audio.h>

/**
 * Holds information regarding a beep/wave sound
 */
struct BeepObject {
    qreal frequency;
    int samplesLeft;
};

/* Think of this as the 'volume' of the sound wave */
const int AMPLITUDE = 16000;

/* Corresponds to the freq. used in phones, we do not need more than that */
const int SAMPLING_FREQ = 8000;

/* Holds the beep objects to be processed */
QQueue<BeepObject> BEEPS;

/**
 * Calls the beeper when and generates the audio
 */
void AUDIO_CALLBACK (void* beeper, quint8* stream, int length) {
    Beeper* object = static_cast<Beeper*>(beeper);
    object->generateSamples ((qint16*) stream, length / 2);
}

/**
 * Configures the audio spec
 */
Beeper::Beeper() {
    m_angle = 0;
    m_enabled = false;

    SDL_LockAudio();

    /* Generate the audio configuration */
    SDL_AudioSpec desiredSpec;
    desiredSpec.channels = 1;
    desiredSpec.samples = 1024;
    desiredSpec.userdata = this;
    desiredSpec.freq = SAMPLING_FREQ;
    desiredSpec.format = AUDIO_S16SYS;
    desiredSpec.callback = AUDIO_CALLBACK;

    /* Open the audio device */
    SDL_AudioSpec obtainedSpec;
    SDL_OpenAudio (&desiredSpec, &obtainedSpec);

    /* Forces to initialize the data for the callback function */
    SDL_PauseAudio (0);
}

/**
 * Stop using the SDL audio when destroying this class
 */
Beeper::~Beeper() {
    SDL_CloseAudio();
    SDL_UnlockAudio();
}

void Beeper::generateSamples (qint16* stream, int length) {
    int i = 0;
    while (i < length) {

        /* Beeps object is empty, ensure that stream has neutral values */
        if (BEEPS.empty()) {
            for (; i < length; ++i)
                stream [i] = 0;

            return;
        }

        /* Get the beep object */
        BeepObject& beep = BEEPS.front();
        int samplesToDo = qMin (i + beep.samplesLeft, length);
        beep.samplesLeft -= samplesToDo - i;

        /* Generate the sound */
        for (; i < samplesToDo; ++i) {
            m_angle += beep.frequency;
            stream [i] = AMPLITUDE * qSin ((m_angle * 2 * M_PI) / SAMPLING_FREQ);
        }

        /* Go to next beep object */
        if (beep.samplesLeft == 0)
            BEEPS.pop_front();
    }
}

/**
 * Enables or disables the sound output
 */
void Beeper::setEnabled (bool enabled) {
    m_enabled = enabled;
}

/**
 * Generates a beep of the given \a frequency & \a duration (in milliseconds).
 * \note The request will be ignored if the beeper is disabled
 */
void Beeper::beep (qreal frequency, int duration) {
    if (m_enabled) {
        BeepObject beep_object;
        beep_object.frequency = frequency;
        beep_object.samplesLeft = duration * SAMPLING_FREQ / 1000;

        BEEPS.append (beep_object);
    }
}
