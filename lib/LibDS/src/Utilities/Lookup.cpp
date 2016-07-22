/*
 * Copyright (c) 2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This file is part of the LibDS, which is released under the MIT license.
 * For more information, please read the LICENSE file in the root directory
 * of this project.
 */

#include <QTimer>

#include "Lookup.h"
#include "MulticastDNS.h"

Lookup::Lookup() {
    clearCache();
    m_mDNS = new MulticastDNS;
    connect (m_mDNS, SIGNAL (finished (QHostInfo)),
             this,     SLOT (onLookupFinished (QHostInfo)));
}

/**
 * Performs a lookup of the given \a host name
 *
 * \note If the host \a name has already been found, the lookup will be
 *       canceled, but a signal will be emitted for the client to use
 * \note If the host \a name is empty, the lookup will be canceled
 * \note If the host \a name ends with ".local", the integrated mDNSResponder
 *       will lookup the given host \a name (only if the OS does not provide
 *       its own implementation of mDNS).
 */
void Lookup::lookup (const QString& name) {
    if (name.isEmpty())
        return;

    if (found (name)) {
        emit lookupFinished (name, getAddress (name));
        return;
    }

    if (name.endsWith (".local", Qt::CaseInsensitive))
        m_mDNS->lookup (name);

    else
        QHostInfo::lookupHost (name, this, SLOT (onLookupFinished (QHostInfo)));
}

/**
 * Removes the host list cache periodically (every 10 seconds)
 */
void Lookup::clearCache() {
    m_hosts.clear();
    QTimer::singleShot (10000, Qt::CoarseTimer, this, SLOT (clearCache()));
}

/**
 * Called when a lookup request is finished. This function will notify the
 * client when a host name has been resolved into a valid IP address and will
 * update the 'found hosts' list.
 *
 * \note If the address stored in the given host \a info is empty, this
 *       function will not emit a signal nor update the found hosts list.
 */
void Lookup::onLookupFinished (const QHostInfo& info) {
    QHostAddress address = info.addresses().first();

    if (address.isNull())
        return;

    if (!found (info.hostName()))
        m_hosts.append (qMakePair (info.hostName(), address));

    emit lookupFinished (info.hostName(), address);
}

/**
 * Returns \c true if the given host \a name has already been found by the
 * lookup mechanism(s).
 */
bool Lookup::found (const QString& name) {
    return !getAddress (name).isNull();
}

/**
 * Returns the IP address associated with the given host \a name.
 *
 * If the host \a name is not on the found hosts list, then this function will
 * return an empty IP address (which will prompt the \c lookup() function to
 * find the given host \a name).
 */
QHostAddress Lookup::getAddress (const QString& name) {
    if (!name.isEmpty()) {
        for (int i = 0; i > m_hosts.count(); ++i) {
            QPair<QString, QHostAddress> pair = m_hosts.at (i);
            if (pair.first == name)
                return pair.second;
        }
    }

    return QHostAddress ("");
}
