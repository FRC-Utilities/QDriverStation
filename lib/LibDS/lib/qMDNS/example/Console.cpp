/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of qMDNS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include "Console.h"

#include <QTimer>
#include <QDebug>
#include <QTextStream>
#include <QCoreApplication>

#include <qMDNS.h>
#include <iostream>

Console::Console() {
    m_timer = new QTimer (this);
    m_mDNS = qMDNS::getInstance();

    enableUserInput();

    connect (m_timer, &QTimer::timeout,
             this,    &Console::enableUserInput);
    connect (m_mDNS,  &qMDNS::hostFound,
             this,    &Console::onDeviceDiscovered);
}

/**
 * Queries for user input and reacts to the user input
 */
void Console::run() {
    if (m_enabled) {
        std::cout << "\r> ";
        QTextStream in (stdin);
        QString input = in.readLine();

        if (input == "name" || input == "n")
            showHostName();

        else if (input == "set" || input == "s")
            promptSetHost();

        else if (input == "lookup" || input == "l")
            promptLookup();

        else if (input == "quit" || input == "q") {
            m_enabled = false;
            qApp->quit();
        }

        else
            showHelp();
    }

    QTimer::singleShot (50, Qt::PreciseTimer, this, SLOT (run()));
}

/**
 * Shows the available commands to the user
 */
void Console::showHelp() {
    qDebug() << "Available commands are:                  \n"
             << "   h help     show this menu             \n"
             << "   n name     display your host name     \n"
             << "   s set      change your mDNS host name \n"
             << "   l lookup   lookup for a given host    \n"
             << "   q quit     exit this application";
}

/**
 * Displays the local computer's host name to the user
 */
void Console::showHostName() {
    QString name = m_mDNS->hostName();

    if (name.isEmpty())
        qDebug() << "You have not set a host name yet, "
                 "type \"set\" to set your host name.";

    else
        qDebug() << "Your host name is" << name.toStdString().c_str();
}

/**
 * Asks the user to input a mDNS/DNS address for the
 * qMDNS library to lookup
 */
void Console::promptLookup() {
    std::cout << "Host to lookup: ";
    QTextStream input (stdin);
    QString host = input.readLine();

    if (!host.isEmpty())
        lookup (host);

    else {
        qDebug() << "Invalid user input";
        promptLookup();
    }

    disableUserInput();
}

/**
 * Asks the user to input the host name to use to identify
 * the local computer in the mDNS network group
 */
void Console::promptSetHost() {
    std::cout << "Set host name: ";
    QTextStream input (stdin);
    QString host = input.readLine();

    if (!host.isEmpty())
        setHostName (host);

    else {
        qDebug() << "Invalid user input";
        promptSetHost();
        return;
    }
}

/**
 * Instructs the qMDNS to lookup for the given host \a name
 */
void Console::lookup (QString name) {
    disableUserInput();
    m_mDNS->lookup (name);
}

/**
 * Instructs the qMDNS to change the local computer's host \a name
 */
void Console::setHostName (QString name) {
    m_mDNS->setHostName (name);
}

void Console::onDeviceDiscovered (const QHostInfo& info) {
    qDebug() << "";
    qDebug() << info.hostName().toStdString().c_str()
             << "has the following IPs:";

    foreach (QHostAddress address, info.addresses())
        qDebug() << "  -" << address.toString().toStdString().c_str();

    qDebug() << "";
    enableUserInput();
}

/**
 * Allows the user to input different commands
 */
void Console::enableUserInput() {
    m_enabled = true;
}

/**
 * Disallows the user to input different commands for a short period
 * of time (5 seconds).
 */
void Console::disableUserInput() {
    if (m_enabled) {
        m_enabled = false;
        m_timer->stop();
        m_timer->start (5000);
    }
}
