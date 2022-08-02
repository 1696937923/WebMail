#ifndef REGISTERUI_H
#define REGISTERUI_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class RegisterUi;
}

class RegisterUi : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterUi(QWidget *parent = nullptr);
    ~RegisterUi();

private:
    Ui::RegisterUi *ui;
    QTimer *timer;

private slots:
    void ReciveRegisterUiShow();
    void on_BackBtn_clicked();

    void on_RegisterBtn_clicked();

signals:
    void ShowMainUi();
};

#endif // REGISTERUI_H
