#include "urlpicker.h"
#include "ui_urlpicker.h"

UrlPicker::UrlPicker(ETab *tab) : ui(new Ui::UrlPicker)
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->parentTab = tab;
    ui->setupUi(this);

    QString selected = parentTab->getSelection();
    ui->tbTitle->setText(selected);
    this->checked = ui->checkBox->checkState() == Qt::Checked;
}

UrlPicker::~UrlPicker()
{
    delete ui;
}

void UrlPicker::on_buttonBox_accepted()
{
    QString title = ui->tbTitle->text();
    QString link = ui->tbUrl->text();
    parentTab->insertLink(title, link);
}

void UrlPicker::on_checkBox_stateChanged(int checked)
{
    this->checked = (checked == Qt::Checked);
    if(checked) {
        ui->tbUrl->setText(ui->tbTitle->text());
        ui->tbUrl->setEnabled(false);
    } else {
        ui->tbUrl->setEnabled(true);
    }
}

void UrlPicker::on_tbTitle_textChanged(const QString &arg1)
{
    if(checked) {
        ui->tbUrl->setText(arg1);
    }
}
