#define BOOST_TEST_MODULE lua-cxx
#include "core.hpp"

#ifdef HAVE_QT_CORE

#include <QCoreApplication>

struct QCoreApplicationFixture
{
    int argc;
    char name[100];
    char* argv[1];

    QCoreApplication* app;
public:
    QCoreApplicationFixture() :
        argc(1)
    {
        strcpy(name, "lua-cxx");
        argv[0] = name;
        app = new QCoreApplication(argc, argv);
    }

    ~QCoreApplicationFixture()
    {
        delete app;
    }
};

BOOST_GLOBAL_FIXTURE(QCoreApplicationFixture);

#endif // HAVE_QT_CORE

// vim: set ts=4 sw=4 :
