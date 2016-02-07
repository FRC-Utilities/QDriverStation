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

#ifndef _QDS_MW_JOYSTICKS_H
#define _QDS_MW_JOYSTICKS_H

#include <QWidget>
#include <QSpinBox>
#include <QPushButton>
#include <QProgressBar>

#include "Joysticks/JoysticksCommon.h"

class QGroupBox;
class QListWidget;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;

class Joysticks : public QWidget
{
    Q_OBJECT

public:
    explicit Joysticks (QWidget* parent);

protected:
    void showEvent (QShowEvent* event);
    void hideEvent (QHideEvent* event);

private slots:
    void onCountChanged();
    void setupIndicators (int row);
    void setTipsVisible (bool visible);
    void onPOVEvent (QDS_POVEvent event);
    void onAxisEvent (QDS_AxisEvent event);
    void onButtonEvent (QDS_ButtonEvent event);

private:
    QHBoxLayout* m_mainLayout;
    QListWidget* m_joystickNames;

    QWidget* m_axesContainer;
    QWidget* m_povsContainer;
    QWidget* m_buttonsContainer;

    QGroupBox* m_JSIndicators;
    QGroupBox* m_POVIndicators;
    QGroupBox* m_axisIndicators;
    QGroupBox* m_buttonIndicators;

    QVBoxLayout* m_joystickBox;
    QVBoxLayout* m_povGroupBox;
    QVBoxLayout* m_axisGroupBox;
    QVBoxLayout* m_buttonGroupBox;

    QVBoxLayout* m_axesBox;
    QVBoxLayout* m_povsBox;
    QGridLayout* m_buttonsBox;

    QList<QSpinBox*> m_povs;
    QList<QProgressBar*> m_axes;
    QList<QPushButton*> m_buttons;
};

#endif
