#include "colorpicker.h"
#include "ui_colorpicker.h"

#include <iostream>
#include <QColorDialog>
#include <QTextCharFormat>

//Make sure the ui file <class></class> and name= are the same as Ui::*
ColorPicker::ColorPicker(ETab *tab) : ui(new Ui::ColorPicker)
{
    this->setAttribute(Qt::WA_DeleteOnClose); //Make sure ~ is called
    this->foreground = tab->foreground();
    this->background = tab->background();
    this->parentTab = tab;
    ui->setupUi(this);
    this->updateColors();
}

ColorPicker::~ColorPicker() {
    delete ui;
}

void ColorPicker::updateColors() {
    ui->foreground->setStyleSheet("background-color: " + this->foreground.name());
    ui->background->setStyleSheet("background-color: " + this->background.name());
}

void ColorPicker::on_btnForeground_clicked()
{
    QColor color = QColorDialog::getColor(this->foreground, this);
    if(color.isValid()){
        this->foreground = color;
        updateColors();
        foregroundChanged = true;
    }
}

void ColorPicker::on_btnBackground_clicked()
{
    QColor color = QColorDialog::getColor(this->background, this);
    if(color.isValid()){
        this->background = color;
        updateColors();
        backgroundChanged = true;
    }
}

void ColorPicker::on_buttonBox_accepted()
{
    QTextCharFormat format;
    if(foregroundChanged) {
        format.setForeground(foreground);
    }

    if(backgroundChanged) {
        format.setBackground(background);
    }

    parentTab->mergeFormat(format);
}
