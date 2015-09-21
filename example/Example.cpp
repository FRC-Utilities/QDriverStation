/**
 * The Falcon header contains many utilities that you may find useful for
 * your application, including launching a QML file easily and automatically
 * scaling your interface to match the screen DPI in Android devices.
 */
#include <Falcon.h>

/**
 * Main entry-point of the application
 */
int main (int argc, char* argv[])
{
    return Falcon::StartApp (argc, argv, "qrc:/Example.qml");
}
