#include "SaveDialog.h"
#include "ui_SaveDialog.h"

SaveDialog::SaveDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveDialog)
{
    ui->setupUi(this);
    ui->checkBoxPacifist->setCheckState(Qt::PartiallyChecked);
}

SaveDialog::~SaveDialog()
{
    delete ui;
}

GameMapChanger::Options SaveDialog::options()
{
    GameMapChanger::Options r;
    r.removeFoW = ui->checkBoxRemoveFoW->isChecked();
    r.removeBuildingSites = ui->checkBoxRemoveBuildingSites->isChecked();
    r.doubleMinerals = ui->checkBoxDoubleMinerals->isChecked();
    r.pacifist = ui->checkBoxPacifist->checkState();
    if (ui->checkBoxRename) {
        r.name = ui->lineEditRename->text().toUtf8();
    }
    return r;
}

void SaveDialog::addInfo(const QString& text)
{
    QString newText = ui->labelInfo->text();
    if (!newText.isEmpty()) {
        newText.append('\n');
    }
    newText.append(text);
    ui->labelInfo->setText(text);
}
