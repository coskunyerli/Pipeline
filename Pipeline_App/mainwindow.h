#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <actor.h>
class MainWindow : public QMainWindow
{
        Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();
        void start();
    private:
        Pipeline::Thread::Actor* m_actor;
};
#endif // MAINWINDOW_H
