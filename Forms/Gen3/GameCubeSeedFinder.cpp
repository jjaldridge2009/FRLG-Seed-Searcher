/*
 * This file is part of PokéFinder
 * Copyright (C) 2017-2019 by Admiral_Fish, bumba, and EzPzStreamz
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <QMessageBox>
#include "GameCubeSeedFinder.hpp"
#include "ui_GameCubeSeedFinder.h"
#include <Core/Gen3/GameCubeSeedSearcher.hpp>

GameCubeSeedFinder::GameCubeSeedFinder(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameCubeSeedFinder)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_DeleteOnClose);

    galesRound = 1;
    coloRound = 1;

    ui->textBoxGalesTopLeft->setValues(1, 800, 10);
    ui->textBoxGalesBottomLeft->setValues(1, 800, 10);
    ui->textBoxGalesTopRight->setValues(1, 800, 10);
    ui->textBoxGalesBottomRight->setValues(1, 800, 10);


    qRegisterMetaType<QVector<u32>>("QVector<u32>");
}

GameCubeSeedFinder::~GameCubeSeedFinder()
{
    delete ui;
}

void GameCubeSeedFinder::on_pushButtonGalesSearch_clicked()
{
    u32 num1, num2;
    if (ui->radioButtonGalesMewtwo->isChecked()) num1 = 0;
    else if (ui->radioButtonGalesMew->isChecked()) num1 = 1;
    else if (ui->radioButtonGalesDeoxys->isChecked()) num1 = 2;
    else if (ui->radioButtonGalesRayquaza->isChecked()) num1 = 3;
    else num1 = 4;

    if (ui->radioButtonGalesArticuno->isChecked()) num2 = 0;
    else if (ui->radioButtonGalesZapdos->isChecked()) num2 = 1;
    else if (ui->radioButtonGalesMoltres->isChecked()) num2 = 2;
    else if (ui->radioButtonGalesKangaskhan->isChecked()) num2 = 3;
    else num2 = 4;

    u16 topLeft = ui->textBoxGalesTopLeft->getUShort();
    u16 bottomLeft = ui->textBoxGalesBottomLeft->getUShort();
    u16 topRight = ui->textBoxGalesTopRight->getUShort();
    u16 bottomRight = ui->textBoxGalesBottomRight->getUShort();

    auto *searcher = new GameCubeSeedSearcher(Game::Gales, { num1, num2, topLeft, bottomLeft, topRight, bottomRight });
    if (galesRound == 1)
    {
        galeSeeds = searcher->getInitialSeeds(num1, num2);

        if (galeSeeds.isEmpty())
        {
            QMessageBox error;
            error.setText("TODO. Download precalc file");
            error.exec();
            return;
        }

        ui->labelGalesRound->setText(tr("Round #") + QString::number(++galesRound));
        ui->labelGalesResults->setText(tr("Possible Results: ") + QString::number(galeSeeds.size()));
    }
    else
    {
        ui->pushButtonGalesSearch->setEnabled(false);
        ui->pushButtonGalesCancel->setEnabled(true);

        connect(searcher, &GameCubeSeedSearcher::finished, this, [ = ] { ui->pushButtonGalesSearch->setEnabled(true); ui->pushButtonGalesCancel->setEnabled(false); });
        connect(searcher, &GameCubeSeedSearcher::updateProgress, this, &GameCubeSeedFinder::updateGalesProgress);
        connect(searcher, &GameCubeSeedSearcher::outputSeeds, this, &GameCubeSeedFinder::updateGales);
        connect(ui->pushButtonGalesCancel, &QPushButton::clicked, searcher, &GameCubeSeedSearcher::cancelSearch);

        ui->progressBarGales->setValue(0);
        ui->progressBarGales->setMaximum(galeSeeds.size());
        searcher->startSearch(galeSeeds);
    }
}

void GameCubeSeedFinder::on_pushButtonGalesReset_clicked()
{
    if (ui->pushButtonGalesSearch->isEnabled())
    {
        galeSeeds.clear();
        galeSeeds.squeeze();
        galesRound = 1;
        ui->labelGalesRound->setText(tr("Round #") + QString::number(galesRound));
    }
}

void GameCubeSeedFinder::on_pushButtonColoSearch_clicked()
{
    u32 num1, num2;
    if (ui->radioButtonColoBlaziken->isChecked()) num1 = 0;
    else if (ui->radioButtonColoEntei->isChecked()) num1 = 1;
    else if (ui->radioButtonColoSwampert->isChecked()) num1 = 2;
    else if (ui->radioButtonColoRaikou->isChecked()) num1 = 3;
    else if (ui->radioButtonColoMeganium->isChecked()) num1 = 4;
    else if (ui->radioButtonColoSuicune->isChecked()) num1 = 5;
    else if (ui->radioButtonColoMetagross->isChecked()) num1 = 6;
    else num1 = 7;

    if (ui->radioButtonColoWes->isChecked()) num2 = 0;
    else if (ui->radioButtonColoSeth->isChecked()) num2 = 1;
    else num2 = 2;

    auto *searcher = new GameCubeSeedSearcher(Game::Colosseum, { num1, num2 });
    if (coloRound == 1)
    {
        coloSeeds = searcher->getInitialSeeds(num1, num2);

        if (coloSeeds.isEmpty())
        {
            QMessageBox error;
            error.setText("TODO. Download precalc file");
            error.exec();
            return;
        }

        ui->labelColoRound->setText(tr("Round #") + QString::number(++coloRound));
        ui->labelColoResults->setText(tr("Possible Results: ") + QString::number(coloSeeds.size()));
    }
    else
    {
        ui->pushButtonColoSearch->setEnabled(false);
        ui->pushButtonColoCancel->setEnabled(true);

        connect(searcher, &GameCubeSeedSearcher::finished, this, [ = ] { ui->pushButtonColoSearch->setEnabled(true); ui->pushButtonColoCancel->setEnabled(false); });
        connect(searcher, &GameCubeSeedSearcher::updateProgress, this, &GameCubeSeedFinder::updateColoProgress);
        connect(searcher, &GameCubeSeedSearcher::outputSeeds, this, &GameCubeSeedFinder::updateColo);
        connect(ui->pushButtonColoCancel, &QPushButton::clicked, searcher, &GameCubeSeedSearcher::cancelSearch);

        ui->progressBarColo->setValue(0);
        ui->progressBarColo->setMaximum(coloSeeds.size());
        searcher->startSearch(coloSeeds);
    }
}

void GameCubeSeedFinder::on_pushButtonColoReset_clicked()
{
    if (ui->pushButtonColoSearch->isEnabled())
    {
        coloSeeds.clear();
        coloSeeds.squeeze();
        coloRound = 1;
        ui->labelColoRound->setText(tr("Round #") + QString::number(coloRound));
    }
}

void GameCubeSeedFinder::updateGales(const QVector<u32> &seeds)
{
    galeSeeds = seeds;
    ui->labelGalesRound->setText(tr("Round #") + QString::number(++galesRound));
    if (galeSeeds.size() == 1)
    {
        ui->labelGalesResults->setText(tr("Seed: ") + QString::number(galeSeeds.at(0), 16).toUpper());
    }
    else
    {
        ui->labelGalesResults->setText(tr("Possible Results: ") + QString::number(galeSeeds.size()));
    }
}

void GameCubeSeedFinder::updateGalesProgress(int progress)
{
    ui->progressBarGales->setValue(progress);
}

void GameCubeSeedFinder::updateColo(const QVector<u32> &seeds)
{
    coloSeeds = seeds;
    ui->labelColoRound->setText(tr("Round #") + QString::number(++coloRound));
    if (coloSeeds.size() == 1)
    {
        ui->labelColoResults->setText(tr("Seed: ") + QString::number(coloSeeds.at(0), 16).toUpper());
    }
    else
    {
        ui->labelColoResults->setText(tr("Possible Results: ") + QString::number(coloSeeds.size()));
    }
}

void GameCubeSeedFinder::updateColoProgress(int progress)
{
    ui->progressBarColo->setValue(progress);
}
