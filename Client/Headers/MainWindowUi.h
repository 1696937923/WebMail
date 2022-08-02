#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void ReciveMainUiShow();

    void on_LoginBtn_clicked();

    void on_RegisterBtn_clicked();

private:
    Ui::MainWindow *ui;
    QTimer * timer;

signals:
    void ShowUserUi(int, const char *, const char *, int);
    void ShowRegisterUi();
    void NewEmail();
};

#endif // MAINWINDOW_H
