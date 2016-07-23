# qMDNS

*A simple mDNS responder written in Qt*

qMDNS is a library that allows your Qt applications to interact with a mDNS network group without the need of installing Bonjour or Avahi. qMDNS works on every operating system that supports the Qt networking module.

### Usage

#### How to use qMDNS 

qMDNS is a singleton class, you can call it anywhere from your application using:

    #include <qMDNS.h>
    
    Chat::Chat() {
       connect (qMDNS::getInstance(), &qMDNS::hostFound,
                this,                 &Chat::onHostFound);
                
       findBob();
    }
    
    void Chat::findBob() {        
        qMDNS::getInstance()->lookup ("bob-computer.local");
    }
    
    void Chat::onHostFound (const QHostInfo& info) {
        qDebug() << "Bob is at" << info.addresses().first();
    }

#### Use qMDNS with your project

1. Copy the root folder of this project in your "libs" directory
2. Import qMDNS to your project using qmake's *include()* function
3. Check the example project for more details

### TODO's

- Add support for generating NSEC code blocks in response packets
- Improve the code that interprets the response/query flag

### License

This project is released under the MIT license.