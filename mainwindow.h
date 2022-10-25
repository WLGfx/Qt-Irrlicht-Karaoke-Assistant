#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "irrlichtwindow.h"
#include <QTimer>
#include "QFileInfo"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    // -----
    // Main variables
    // -----

    IrrlichtWindow      *iWind;
    QTimer              *timer;

    QList<QFileInfo>    imagePaths;
    int                 imageSelected;
    QImage              imagePreview;
    video::ITexture     *skyTexture;

    QImage              *singerImage;

    void                changeCameraSettings();

private slots:
    void on_scrShow_clicked();

    void on_timer();

    void on_scrFPS_valueChanged(int arg1);

    void on_imageListAdd_clicked();

    void on_imageListRemove_clicked();

    void on_imageList_currentRowChanged(int currentRow);

    void on_imageListSkySphere_clicked();

    void on_pictureOnOff_clicked(bool checked);

    void on_imageListSkySphere_2_clicked();

    void on_xRadius_valueChanged(double arg1);

    void on_xSpeed_valueChanged(double arg1);

    void on_yRadius_valueChanged(double arg1);

    void on_ySpeed_valueChanged(double arg1);

    void on_zRadius_valueChanged(double arg1);

    void on_zSpeed_valueChanged(double arg1);

    void on_singerEdit_returnPressed();

    void on_singerAdd_clicked();

    void on_singerRemove_clicked();

    void on_singerShow_clicked(bool checked);

    void on_rainShow_clicked(bool checked);

    void on_snowShow_clicked(bool checked);

    void on_windSpeed_valueChanged(double arg1);

    void on_savePrefs_clicked();

    void on_loadPrefs_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
