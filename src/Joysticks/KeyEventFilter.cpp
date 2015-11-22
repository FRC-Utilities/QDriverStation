#include "KeyEventFilter.h"

bool KeyEventFilter::eventFilter (QObject* object, QEvent* event) {
    Q_UNUSED (object);

    switch (event->type()) {
    case QEvent::KeyPress:
        emit keyPress (static_cast <QKeyEvent*> (event), true);
        break;
    case QEvent::KeyRelease:
        emit keyPress (static_cast <QKeyEvent*> (event), false);
        break;
    default:
        break;
    }

    return false;
}
