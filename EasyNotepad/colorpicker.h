#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <QDialog>
#include <QColor>
#include "etab.h"

//Basic QT header structure
namespace Ui {
class ColorPicker;
}

class ColorPicker : public QDialog {
    Q_OBJECT

public:
    explicit ColorPicker(ETab *tab = nullptr);
    ~ColorPicker();
    void show();

private slots:
    void on_btnForeground_clicked();
    void on_btnBackground_clicked();
    void on_buttonBox_accepted();

private:
    Ui::ColorPicker *ui;
    QColor foreground;
    QColor background;
    ETab *parentTab;
    bool foregroundChanged = false;
    bool backgroundChanged = false;
    void updateColors();
};


#endif // COLORPICKER_H
