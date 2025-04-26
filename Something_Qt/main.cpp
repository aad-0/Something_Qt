#include "mainwindow.h"

#include <QApplication>
#include <QDebug>

#include "comm.h"

//#define TEST_COM

int test_com (int argc, char * argv [])
{
    (void ) argc;
    (void ) argv;
    ComDef_xpu8DeclareBuffer(check, ComDefImu_TypeDef);
    ComDef_xu8GetHeading(&check) = 0xAA;
    ComDef_xu8GetCommand(&check) = ComDef_xu8CommandMask(ComDefCommandMode, ComDefCommandMaskGet); //ComDef_xu8CommandModeGet(ComDefCommandMode);
    ComDef_xu16GetPayloadLength(&check) = (uint16_t) sizeof(ComDefImu_TypeDef);
    ComDef_xu8GetCrc(&check) = 0xFA;
    ComDef_xu8GetEnd(&check) = 0xBB;

    ComDefImu_TypeDef * pPayload = (ComDefImu_TypeDef *)& ComDef_xpu8GetPayload(&check);
    pPayload->fX = 0x10;
    pPayload->fY = 0x20;
    pPayload->fZ = 0x30;


    return 0;
}

int main(int argc, char *argv[])
{
#ifdef TEST_COM
    return test_com (argc, argv);

#endif
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
