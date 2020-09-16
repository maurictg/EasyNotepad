#ifndef URLPICKER_H
#define URLPICKER_H

#include <QDialog>
#include <etab.h>

namespace Ui {
class UrlPicker;
}

class UrlPicker : public QDialog
{
    Q_OBJECT

public:
    explicit UrlPicker(ETab *tab = nullptr);
    ~UrlPicker();

private slots:
    void on_buttonBox_accepted();
    void on_checkBox_stateChanged(int state);
    void on_tbTitle_textChanged(const QString &arg1);

private:
    Ui::UrlPicker *ui;
    ETab *parentTab;
    bool checked;
};

#endif // URLPICKER_H
