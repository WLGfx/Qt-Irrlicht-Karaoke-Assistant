#include "irrlichtwindow.h"
#include "ui_irrlichtwindow.h"
#include "math.h"
#include "stdlib.h"

IrrlichtWindow::IrrlichtWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::IrrlichtWindow)
{
    ui->setupUi(this);

    // make the display borderless

    setWindowFlags(Qt::FramelessWindowHint);

    // set default values on init

    device = 0;
    picture = 0;

    camXRadius = camXSpeed = camXAngle = 0.0f;
    camYRadius = camYSpeed = camYAngle = 0.0f;
    camZRadius = camZSpeed = camZAngle = 0.0f;
}

IrrlichtWindow::~IrrlichtWindow()
{
    closeDown();

    // irrlicht will drop all loaded textures, objects, etc

    if ( device ) device->drop();

    delete ui;
}

bool IrrlichtWindow::startIrrlicht()
{
    // get irrlicht attached to the window

    static SIrrlichtCreationParameters params;

    int w = ui->centralwidget->width();
    int h = ui->centralwidget->height();

    params.WindowId = (void*)ui->centralwidget->winId();
    params.IgnoreInput = true;
    params.WindowSize.Width = w;
    params.WindowSize.Height = h;
    params.Bits = 32;
    params.Vsync = true;
    params.DeviceType = EIDT_BEST;
    params.DriverType = video::EDT_OPENGL;
    params.Doublebuffer = true;
    params.HandleSRGB = true;
    params.UsePerformanceTimer = false;

    // now attempt to activate irrlicht

    device = createDeviceEx( params );

    if ( device )
    {
        // get some more useful class pointers

        driver = device->getVideoDriver();
        gui = device->getGUIEnvironment();
        smgr = device->getSceneManager();

        // carry on with some more startup stuff

        startUp();

        return true;
    }
    else
    {
        // failed...

        device = 0;
        return false;
    }
}

void IrrlichtWindow::update()
{
    // rotate the test cube

    angle = fmod(angle+0.1f, 360.0f);
    obj->setRotation(core::vector3df(angle, angle, angle));

    // and the sky sphere for that matter

    sky->setRotation(core::vector3df(180,angle,0));

    // move the camera

    core::vector3df camPosition, lightPosition;

    camXAngle = fmod( camXAngle + camXSpeed, PI2 );
    camYAngle = fmod( camYAngle + camYSpeed, PI2 );
    camZAngle = fmod( camZAngle + camZSpeed, PI2 );

    camPosition.X = 0.0f + sin( camXAngle ) * camXRadius;
    camPosition.Y = 0.0f + sin( camYAngle ) * camYRadius;
    camPosition.Z = -57.0f + sin( camZAngle ) * camZRadius;

    camera->setPosition( camPosition );
    camera->setTarget(core::vector3df(0.0f,0.0f,0.0f));

    // sping the light around

    lightAngle = fmod( lightAngle + 0.01f, PI2 );

    lightPosition.X = sin( lightAngle ) * 60.0f;
    lightPosition.Y = cos( lightAngle ) * 60.0f;
    lightPosition.Z = -140.0f;

    light->setPosition( lightPosition );

    // run the rain and snow functions

    updateRain();
    updateSnow();

    // update the engine and redraw the scene

    driver->beginScene(true,true,0x00ffffff);
    smgr->drawAll();
    driver->endScene();
}

void IrrlichtWindow::startUp()
{
    // add main camera node

    camera = smgr->addCameraSceneNode(0, core::vector3df(0,0,-50),
                             core::vector3df(0,0,0));

    // add a dummy cube that will be hidden by the picture

    obj = smgr->addCubeSceneNode(10);

    // setup the main light

    light = smgr->addLightSceneNode(0,core::vector3df(50,50,-50));
    light->setRadius(200.0f);

    // add the sky sphere

    sky = smgr->addMeshSceneNode(smgr->getMesh("sphere.x"));
    sky->getMaterial(0).Lighting = false;
    sky->getMaterial(0).BackfaceCulling = true;

    // setup the picture box to display images

    picture = smgr->addCubeSceneNode(48);
    picture->setScale(core::vector3df(1.4f, 1.0f, 1.0f));
    picture->setVisible(false);

    // add the singers display box and the texture

    singer = smgr->addMeshSceneNode(smgr->getMesh("singer.x"));
    singerTexture = driver->addTexture(core::dimension2du(singerIWidth,singerIHeight),
                                       "singer",
                                       video::ECF_A8R8G8B8);
    singer->setMaterialTexture( 0, singerTexture );
    singer->setPosition(core::vector3df(0,-8,-40));
    singer->setScale(core::vector3df(0.4f, 0.2f, 1.0f));
    singer->getMaterial(0).BackfaceCulling = true;
    singer->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
    singer->setVisible( false );

    // load up the rain and snow particles

    rainParticle = driver->getTexture("raindrop.png");
    snowParticle = driver->getTexture("snowflake.png");

    // set the default wind speed and make rain and snow disabled

    windSpeed = 0.0f;
    rainActive = false;
    snowActive = false;
}

void IrrlichtWindow::closeDown()
{
    // clean up the rain and snow data

    int c;
    scene::IBillboardSceneNode *node;

    if ( rainActive )
    {
        for ( c=0; c<rainParticles.size(); c++ )
        {
            node = rainParticles[c];
            node->remove();
        }
        rainParticles.clear();
    }

    if ( snowActive )
    {
        for ( c=0; c<snowParticles.size(); c++ )
        {
            node = snowParticles[c];
            node->remove();
        }
        snowParticles.clear();
    }
}

void IrrlichtWindow::setSkyBoxTexture(QFileInfo fi)
{
    QString str = fi.absoluteFilePath();
    sky->setMaterialTexture(0,
        driver->getTexture((char*)str.toStdString().c_str()));
}

void IrrlichtWindow::pictureOnOff(bool checked)
{
    if ( checked )
    {
        picture->setVisible( true );
    }
    else
    {
        picture->setVisible( false );
    }
}

void IrrlichtWindow::setPicture(QFileInfo fi)
{
    if (picture)
    {
        QString str = fi.absoluteFilePath();
        picture->setMaterialTexture(0,
            driver->getTexture((char*)str.toStdString().c_str()));
    }
}

void IrrlichtWindow::setCameraSettings(float xr, float xs, float yr, float ys, float zr, float zs)
{
    camXRadius = xr;
    camXSpeed = xs;
    camYRadius = yr;
    camYSpeed = ys;
    camZRadius = zr;
    camZSpeed = zs;
}

void IrrlichtWindow::setSinger(bool checked, QImage *img)
{
    // a straight forward memcpy because both QImage and ITexture
    // are exactly the same size and format

    if ( checked )
    {
        void *src = (void*)img->bits();
        void *dst = singerTexture->lock();
        memcpy( dst, src, singerIWidth * singerIHeight * 4 );
        singerTexture->unlock();
        singer->setMaterialTexture(0, singerTexture);
        singer->setVisible(true);
        singer->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
    }
    else
    {
        singer->setVisible(false);
    }
}

void IrrlichtWindow::activateSnow(bool checked,int density,float speed)
{
    snowDensity = density;
    snowSpeed = speed;
    snowActive = checked;

    int c;

    core::vector3df pos;
    scene::IBillboardSceneNode *node;

    if ( checked )
    {
        for ( c = 0; c < snowDensity; c++ )
        {
            pos.X = rand() % 200 - 100;
            pos.Y = rand() % 200 - 50;
            pos.Z = rand() % 100 - 100;

            node = smgr->addBillboardSceneNode(0, core::dimension2df(1,1),
                                               pos );
            node->setMaterialTexture(0, snowParticle);
            node->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);

            snowParticles.push_back( node );
        }
    }
    else
    {
        for ( c = 0; c < snowDensity; c++ )
        {
            node = snowParticles[c];
            node->remove();
        }
        snowParticles.clear();
    }
}

void IrrlichtWindow::activateRain(bool checked,int density,float speed)
{
    rainDensity = density;
    rainSpeed = speed;
    rainActive = checked;

    int c;

    core::vector3df pos;
    scene::IBillboardSceneNode *node;

    if ( checked )
    {
        //rainParticles.reserve( rainDensity );

        for ( c = 0; c < rainDensity; c++ )
        {
            pos.X = rand() % 200 - 100;
            pos.Y = rand() % 200 - 50;
            pos.Z = rand() % 100 - 100;

            node = smgr->addBillboardSceneNode(0, core::dimension2df(1,1),
                                               pos );
            node->setMaterialTexture(0, rainParticle);
            node->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);

            rainParticles.push_back( node );
        }
    }
    else
    {
        for ( c = 0; c < rainDensity; c++ )
        {
            node = rainParticles[c];
            node->remove();
        }
        rainParticles.clear();
    }
}

void IrrlichtWindow::setWindSpeed(float speed)
{
    windSpeed = speed;
}

void IrrlichtWindow::updateRain()
{
    core::vector3df pos;
    scene::IBillboardSceneNode *node;
    int c;

    if ( rainActive )
    {
        for ( c=0; c<rainDensity; c++ )
        {
            node=rainParticles[c];
            pos=node->getPosition();
            pos.Y -= rainSpeed;
            pos.X += windSpeed;
            if (pos.Y < -50)
                pos.Y = 100;
            if (pos.X < -100)
                pos.X = 100;
            if (pos.X > 100)
                pos.X = -100;
            node->setPosition(pos);
        }
    }
}

void IrrlichtWindow::updateSnow()
{
    core::vector3df pos;
    scene::IBillboardSceneNode *node;
    int c;

    if ( snowActive )
    {
        for ( c=0; c<snowDensity; c++ )
        {
            node=snowParticles[c];
            pos=node->getPosition();
            pos.Y -= snowSpeed;
            pos.X += windSpeed + float(( rand() % 32 ) - 16) / 32.0f;
            if (pos.Y < -50)
                pos.Y = 100;
            if (pos.X < -100)
                pos.X = 100;
            if (pos.X > 100)
                pos.X = -100;
            node->setPosition(pos);
        }
    }
}
