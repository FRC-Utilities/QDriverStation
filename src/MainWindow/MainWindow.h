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

#ifndef _QDS_MAIN_WINDOW_H
#define _QDS_MAIN_WINDOW_H

#include <QMainWindow>

class Status;
class Buttons;
class LeftTab;
class RightTab;
class QHBoxLayout;
class QVBoxLayout;

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow();
    ~MainWindow();

  protected:
    void moveEvent (QMoveEvent* event);
    void closeEvent (QCloseEvent* event);

  private slots:
    void showDocked();
    void showUnDocked();
    void startUpSound();

    void updateSize();
    void displayWindow();

  private:
    bool m_docked;
    bool m_closing;

    QWidget* m_central;
    Status* m_status;
    Buttons* m_buttons;
    LeftTab* m_leftTab;
    RightTab* m_rightTab;
    QHBoxLayout* m_layout;
};

#endif
