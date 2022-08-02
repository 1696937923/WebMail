#ifndef EMAIL_H
#define EMAIL_H

#include <QWidget>
#include <QTextBrowser>
#include <QListView>

namespace Ui {
class Email;
}

class Email : public QWidget
{
    Q_OBJECT

public:
    explicit Email(QWidget *parent = nullptr);
    ~Email();

private:
    Ui::Email *ui;
    char CurrentUser[48];
    int count = 0;

private slots:
    void ReciveEmailUiShow(const char *);
    void on_BackBtn_clicked();

    void on_DelEmailBtn_clicked();

signals:
    void BackUserUi();

};

#endif // EMAIL_H
