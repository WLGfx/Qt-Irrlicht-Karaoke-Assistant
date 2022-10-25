#ifndef IRRLICHTWINDOW_H
#define IRRLICHTWINDOW_H

#include <QMainWindow>
#include "irrlicht.h"
#include "QFileInfo"
#include <QList>
#include <vector>
#include <list>

#define PI 3.1415926589f
#define PI2 6.2831853f

#define singerIWidth 2048
#define singerIHeight 256

using namespace irr;
using namespace std;

namespace Ui {
class IrrlichtWindow;
}

class IrrlichtWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit IrrlichtWindow(QWidget *parent = 0);
    ~IrrlichtWindow();

    // -----
    // Main vars
    // -----

    IrrlichtDevice          *device;
    video::IVideoDriver     *driver;
    gui::IGUIEnvironment    *gui;
    scene::ISceneManager    *smgr;

    scene::ICameraSceneNode *camera;
    scene::ILightSceneNode  *light;
    scene::IMeshSceneNode   *singer;
    video::ITexture         *singerTexture;

    float   camXRadius, camXSpeed, camXAngle;
    float   camYRadius, camYSpeed, camYAngle;
    float   camZRadius, camZSpeed, camZAngle;
    float   lightAngle;

    video::ITexture         *rainParticle;
    video::ITexture         *snowParticle;

    vector<scene::IBillboardSceneNode*>   rainParticles;
    vector<scene::IBillboardSceneNode*>   snowParticles;

    float                   windSpeed;
    int                     rainDensity, snowDensity;
    float                   rainSpeed, snowSpeed;
    bool                    rainActive, snowActive;

    // -----
    // Main functions
    // -----

    bool    startIrrlicht();
    void    update();
    void    startUp();
    void    closeDown();
    void    pictureOnOff(bool checked);
    void    setPicture(QFileInfo fi);
    void    setCameraSettings(float xr, float xs,
                              float yr, float ys,
                              float zr, float zs);
    void    setSinger(bool checked, QImage *img);

    void    activateSnow(bool checked,int density,float speed);
    void    activateRain(bool checked,int density,float speed);
    void    setWindSpeed(float speed);

    void    updateRain();
    void    updateSnow();

    // -----
    // Temporary stuff while testing
    // -----

    scene::IMeshSceneNode   *obj;
    float                   angle;
    scene::IMeshSceneNode   *sky;
    void                    setSkyBoxTexture(QFileInfo fi);
    scene::IMeshSceneNode   *picture;

private:
    Ui::IrrlichtWindow *ui;
};

#endif // IRRLICHTWINDOW_H
