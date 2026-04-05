// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied.  See the License for the specific language governing permissions and limitations
// under the License.

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
    r.cureVillagers = ui->checkBoxCureVillagers->isChecked();
    r.doubleMinerals = ui->checkBoxDoubleMinerals->isChecked();
    r.pacifist = ui->checkBoxPacifist->checkState();
    if (ui->checkBoxRename->isChecked()) {
        r.name = ui->lineEditRename->text().toUtf8();
    }
    if (ui->checkBoxDifficulty->isChecked()) {
        r.difficulty = ui->comboBoxDifficulty->currentIndex();
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
