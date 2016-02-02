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

#ifndef _QDS_CONSOLE_WIDGET_H
#define _QDS_CONSOLE_WIDGET_H

#include <QWidget>

class QLineEdit;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QPlainTextEdit;

class ConsoleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConsoleWidget (QWidget* parent);
    ~ConsoleWidget();

public slots:
    void copy();
    void clear();

signals:
    void messageRegistered (QString text);

private slots:
    void createWidgets();
    void createLayouts();
    void connectSlots();

    void sendCommand();
    void onProtocolChanged();
    void registerMessage (QString text);

private:
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_commandLayout;

    QWidget* m_commandWidget;
    QLineEdit* m_commandEdit;
    QPushButton* m_sendButton;

    QPlainTextEdit* m_consoleEdit;
};

#endif
