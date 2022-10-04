#include "mainwindow.h"
#include "CMD.h"
#include <QApplication>
#include "ShaderParameters.h"
#include "GlobalTypeDefs.h"

/**
 * \brief 
 * \param argc 
 * \param argv 
 * \return 
 */
int main(int argc, char *argv[])
{
	
#define DEBUG   //Leaving this on for now
#ifdef DEBUG
    RedirectIOToConsole();
#endif // DEBUG
	
	//These macros will write to shaders at compile time, do not insert anything in between :)
    BEGIN_SHADER_WRITES()
	END_SHADER_WRITES()
	
    //Forces the usage of desktop OpenGL - Qt uses OpenGL ES as default
    //Attribute must be set before Q(Gui)Application is constructed:
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);

    //Makes a Qt application
    QApplication a(argc, argv);

    World* world = new World();
	
    //Makes the Qt MainWindow and shows it.
    MainWindow w(nullptr, world);
    w.show();

    return a.exec();
}
