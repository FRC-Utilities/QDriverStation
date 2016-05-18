/*
 * Copyright (c) 2015-2016 WinT 3794 <http://wint3794.org>
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

//==============================================================================
// Application includes
//==============================================================================

#include "beeper.h"

//==============================================================================
// System includes
//==============================================================================

#include <cmath>
#include <SDL.h>
#include <SDL_audio.h>

//==============================================================================
// Global variables
//==============================================================================

const int AMPLITUDE = 28000;
const int FREQUENCY = 44100;
float V             = 0;
bool ENABLED        = false;

std::queue<BeepObject> BEEPS;

//==============================================================================
// audio_callback
//==============================================================================

void audio_callback (void* _beeper, Uint8* _stream, int _length) {
    int length = _length / 2;
    Sint16* stream = (Sint16*) _stream;
    Beeper* beeper = (Beeper*) _beeper;

    beeper->generateSamples (stream, length);
}

//==============================================================================
// Beeper::Beeper
//==============================================================================

Beeper::Beeper() {
    SDL_AudioSpec desiredSpec;
    desiredSpec.freq = FREQUENCY;
    desiredSpec.format = AUDIO_S16SYS;
    desiredSpec.channels = 1;
    desiredSpec.samples = 2048;
    desiredSpec.callback = audio_callback;
    desiredSpec.userdata = this;

    SDL_AudioSpec obtainedSpec;
    SDL_OpenAudio (&desiredSpec, &obtainedSpec);
    SDL_PauseAudio (0);
}

//==============================================================================
// Beeper::~Beeper
//==============================================================================

Beeper::~Beeper() {
    SDL_CloseAudio();
}

//==============================================================================
// Beeper::generateSamples
//==============================================================================

void Beeper::generateSamples (qint16* stream, int length) {
    int i = 0;
    while (i < length) {

        if (BEEPS.empty()) {
            while (i < length) {
                stream[i] = 0;
                i++;
            }
            return;
        }

        BeepObject& bo = BEEPS.front();

        int samplesToDo = std::min (i + bo.samplesLeft, length);
        bo.samplesLeft -= samplesToDo - i;

        while (i < samplesToDo) {
            stream[i] = AMPLITUDE * std::sin (V * 2 * M_PI / FREQUENCY);
            i++;
            V += bo.freq;
        }

        if (bo.samplesLeft == 0)
            BEEPS.pop();
    }
}

//==============================================================================
// Beeper::setEnabled
//==============================================================================

void Beeper::setEnabled (bool enabled) {
    ENABLED = enabled;
}

//==============================================================================
// Beeper::beep
//==============================================================================

void Beeper::beep (float freq, int duration) {
    if (ENABLED) {
        BeepObject beep_object;
        beep_object.freq = freq;
        beep_object.samplesLeft = duration * FREQUENCY / 1000;

        BEEPS.push (beep_object);
    }
}
