#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include "QRect"
#include "QFileDialog"
#include "QFileInfo"
#include "QPainter"
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    iWind = 0;  // 3d window not opened yet
    timer = 0;

    QDir dir;

    ui->statusBar->showMessage(dir.currentPath(),0);

    imageSelected = -1;     // no image selected in the list
}

MainWindow::~MainWindow()
{
    if ( iWind )
    {
        iWind->close();
        delete iWind;
        iWind = 0;
        delete timer;
        timer = 0;
    }

    delete ui;
}

void MainWindow::on_scrShow_clicked()
{
    if ( !iWind )   // Let's activate the window then
    {
        // create a new instance of the class

        iWind = new IrrlichtWindow(0);

        // show, resize and move the window

        iWind->show();
        iWind->resize(ui->scrWidth->value(),ui->scrHeight->value());
        iWind->move(ui->scrX->value(),ui->scrY->value());

        // activate the main window again

        activateWindow();

        // check if irrlicht starts up ok

        if ( !iWind->startIrrlicht() )
        {
            delete iWind;
            ui->scrShow->setChecked(false);
            iWind = 0;
        }
        else        // start the timer as well
        {
            timer = new QTimer();

            connect(timer, SIGNAL(timeout()), this, SLOT(on_timer()));
            timer->setInterval(ui->scrFPS->value());
            timer->start(ui->scrFPS->value());
            changeCameraSettings();
        }
    }
    else            // Time to close it then
    {
        delete iWind;
        iWind = 0;
        delete timer;
        timer = 0;
    }
}

void MainWindow::on_timer()
{
    if ( iWind )
        iWind->update();    // update the 3d window
}

void MainWindow::on_scrFPS_valueChanged(int arg1)
{
    if ( iWind && timer )
    {
        timer->setInterval(arg1);   // milliseconds update rate
    }
}

void MainWindow::on_imageListAdd_clicked()
{
    // Use the fileselector to add multiple directories

    QStringList fn = QFileDialog::getOpenFileNames(this,
           tr("Open Images..."),QString(),
           tr("Image Files (*.jpg *.bmp *.png);;All Files(*)"));

    int count = fn.size();

    while ( count )
    {
        count--;

        QFileInfo fi( fn[ count ] );

        if (fi.exists() && (
                    (fi.suffix().toUpper() == "JPG") |
                    (fi.suffix().toUpper() == "BMP") |
                    (fi.suffix().toUpper() == "PNG") |
                    (fi.suffix().toUpper() == "JPEG" )))
        {
            ui->imageList->addItem( fi.fileName() );
            imagePaths.append( fi );
        }
    }
}

void MainWindow::on_imageListRemove_clicked()
{
    if ( imageSelected >= 0 )
    {
        ui->imageList->takeItem( imageSelected );
        imagePaths.erase( imagePaths.begin() + imageSelected );
    }
}

void MainWindow::on_imageList_currentRowChanged(int currentRow)
{
    imageSelected = currentRow;
    imagePreview.load( imagePaths[imageSelected].absoluteFilePath() );
    ui->imageListPreview->setPixmap(QPixmap::fromImage(imagePreview));
}

void MainWindow::on_imageListSkySphere_clicked()
{
    // I'm probably better off just loading the texture manually

    if (iWind)
        iWind->setSkyBoxTexture( imagePaths[imageSelected] );
}

void MainWindow::on_pictureOnOff_clicked(bool checked)
{
    if (iWind)
        iWind->pictureOnOff(checked);
}

void MainWindow::on_imageListSkySphere_2_clicked()
{
    if (iWind)
        iWind->setPicture(imagePaths[imageSelected]);
}

void MainWindow::changeCameraSettings()
{
    if (iWind)
    {
        float xr = ui->xRadius->value();
        float xs = ui->xSpeed->value();
        float yr = ui->yRadius->value();
        float ys = ui->ySpeed->value();
        float zr = ui->zRadius->value();
        float zs = ui->zSpeed->value();
        iWind->setCameraSettings(xr,xs,yr,ys,zr,zs);
    }
}

void MainWindow::on_xRadius_valueChanged(double arg1)
{
    (void)(arg1);
    changeCameraSettings();
}

void MainWindow::on_xSpeed_valueChanged(double arg1)
{
    (void)(arg1);
    changeCameraSettings();
}

void MainWindow::on_yRadius_valueChanged(double arg1)
{
    (void)(arg1);
    changeCameraSettings();
}

void MainWindow::on_ySpeed_valueChanged(double arg1)
{
    (void)(arg1);
    changeCameraSettings();
}

void MainWindow::on_zRadius_valueChanged(double arg1)
{
    (void)(arg1);
    changeCameraSettings();
}

void MainWindow::on_zSpeed_valueChanged(double arg1)
{
    (void)(arg1);
    changeCameraSettings();
}

void MainWindow::on_singerEdit_returnPressed()
{
    QString text = ui->singerEdit->text();
    if ( text.length() > 0 )
        ui->singerList->addItem( text );
}

void MainWindow::on_singerAdd_clicked()
{
    QString text = ui->singerEdit->text();
    if ( text.length() > 0 )
        ui->singerList->addItem( text );
}

void MainWindow::on_singerRemove_clicked()
{
    int currentItem = ui->singerList->currentRow();
    if ( currentItem >= 0 )
        ui->singerList->takeItem( currentItem );
}

void MainWindow::on_singerShow_clicked(bool checked)
{
    int currentItem = ui->singerList->currentRow();

    if ( currentItem >= 0 && iWind )
    {
        if ( checked )
        {
            QString text = ui->singerList->item(currentItem)->text();

            QImage temp(singerIWidth,singerIHeight,QImage::Format_ARGB32);

            QPainter paint;
            paint.begin(&temp);

            paint.setCompositionMode(QPainter::CompositionMode_Source);
            paint.fillRect(0,0,singerIWidth,singerIHeight,Qt::transparent);

            QFont font = (ui->singerFont->font());
            font.setPointSize( ui->singerSize->value() );
            font.setBold( ui->singerBold->isChecked() );

            QString fStr = ui->singerFrontColour->currentText();
            QString sStr = ui->singerShadowColour->currentText();

            paint.setFont( font );

            paint.setPen( sStr );
            paint.drawText(QRect(6,6,singerIWidth,singerIHeight),
                           Qt::AlignCenter, text);

            paint.setPen( fStr );
            paint.drawText(QRect(0,0,singerIWidth,singerIHeight),
                           Qt::AlignCenter, text);

            paint.end();

            iWind->setSinger(checked, &temp);
        }
        else
        {
            iWind->setSinger(checked, 0);
        }
    }
}

void MainWindow::on_rainShow_clicked(bool checked)
{
    if ( iWind )
        iWind->activateRain( checked,
                             ui->rainDensity->value(),
                             ui->rainSpeed->value());
}

void MainWindow::on_snowShow_clicked(bool checked)
{
    if ( iWind )
        iWind->activateSnow(checked,
                            ui->snowDensity->value(),
                            ui->snowSpeed->value());
}

void MainWindow::on_windSpeed_valueChanged(double arg1)
{
    if ( iWind )
        iWind->setWindSpeed(arg1);
}

void MainWindow::on_savePrefs_clicked()
{
    QFile savefile( "imagePrefs.txt" );

    // if file already exists then delete it

    if ( savefile.exists() ) savefile.remove();

    // if unable to create the file then just exit

    if ( !savefile.open(QIODevice::WriteOnly | QIODevice::Text) )
        return;

    // Setup the text stream

    QTextStream out( &savefile );

    // first line is the array size

    out << imagePaths.size() << "\n";

    int c;

    // now output all the paths in the array

    for ( c=0; c<imagePaths.size(); c++ )
    {
        out << imagePaths[c].absoluteFilePath() << "\n";
    }

    // close the file

    savefile.close();
}

void MainWindow::on_loadPrefs_clicked()
{
    QFile loadfile( "imageprefs.txt" );

    if ( !loadfile.open(QIODevice::ReadOnly | QIODevice::Text) )
        return;

    QTextStream in( &loadfile );

    // read the number in first

    QString text;
    text = in.readLine();
    int count = text.toInt();

    int c;

    // clear the current array and read all files

    imagePaths.clear();
    ui->imageList->clear();
    QFileInfo fi;

    for ( c=0; c<count; c++ )
    {
        text = in.readLine();
        fi.setFile( text );
        ui->imageList->addItem( fi.fileName() );
        imagePaths.append( fi );
    }

    // close the file

    loadfile.close();
}
