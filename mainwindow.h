#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_checkBox_stateChanged(int arg1);

    void on_pushButton_11_released();

    void on_pushButton_4_released();

    void on_pushButton_released();

    void on_pushButton_3_released();

    void on_pushButton_9_released();

    void on_pushButton_8_released();

    void on_pushButton_5_released();

    void on_pushButton_7_released();

    void on_pushButton_10_released();

    void on_pushButton_2_released();

    void on_pushButton_6_released();

    void on_pushButton_12_released();

private:
    Ui::MainWindow *ui;
    bool qjyc = false;
    void run();
    void seth(int num);
    bool ready = false;
    char h1num;
    char h2num;
};
#endif // MAINWINDOW_H
