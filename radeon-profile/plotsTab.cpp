
// copyright marazmista @ 13.06.2017

#include "radeon_profile.h"
#include "ui_radeon_profile.h"
#include "dialog_defineplot.h"

void radeon_profile::createPlots() {
    for (const QString &name : plotManager.plots.keys())
        ui->pagePlots->layout()->addWidget(plotManager.plots.value(name));
}

void radeon_profile::on_btn_configurePlots_clicked()
{
    ui->stack_plots->setCurrentIndex(1);
}

void radeon_profile::on_btn_applySavePlotsDefinitons_clicked()
{
    plotManager.setRightGap(ui->cb_plotsRightGap->isChecked());

    plotManager.recreatePlotsFromSchemas();
    createPlots();

    ui->stack_plots->setCurrentIndex(0);
    ui->stack_plots->layout()->invalidate();
}

void radeon_profile::on_btn_addPlotDefinition_clicked()
{
    Dialog_definePlot *d = new Dialog_definePlot(this);
    d->setAvailableGPUData(device.gpuData.keys());

    if (d->exec() == QDialog::Accepted) {
        PlotDefinitionSchema pds = d->getCreatedSchema();

        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, pds.name);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(0,Qt::Checked);

        if (plotManager.schemas.contains(pds.name)) {
            if (!askConfirmation(tr("Question"), tr("Plot definition with that name already exists. Replace?"))) {
                delete d;
                delete item;
                return;
            }
        } else
            ui->list_plotDefinitions->addTopLevelItem(item);


        plotManager.addSchema(pds);
    }

    delete d;
}

void radeon_profile::on_btn_removePlotDefinition_clicked()
{
    if (!ui->list_plotDefinitions->currentItem())
        return;

    if (!askConfirmation("",tr("Remove selected plot definition?")))
        return;

    plotManager.removeSchema(ui->list_plotDefinitions->currentItem()->text(0));
    delete ui->list_plotDefinitions->currentItem();
}

void radeon_profile::modifyPlotSchema(const QString &name) {
    PlotDefinitionSchema pds = plotManager.schemas[name];

    Dialog_definePlot *d = new Dialog_definePlot(this);
    d->setAvailableGPUData(device.gpuData.keys());
    d->setEditedPlotSchema(pds);

    if (d->exec() == QDialog::Accepted) {
        PlotDefinitionSchema pds = d->getCreatedSchema();
        plotManager.addSchema(pds);

        if (pds.name != name) {
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, pds.name);
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            item->setCheckState(0,Qt::Checked);
        }
    }

    delete d;
}

void radeon_profile::on_btn_modifyPlotDefinition_clicked()
{
    if (ui->list_plotDefinitions->currentItem() == 0)
        return;

    modifyPlotSchema(ui->list_plotDefinitions->currentItem()->text(0));
}

void radeon_profile::on_btn_cancelEditPlots_clicked()
{
    ui->stack_plots->setCurrentIndex(0);
}

void radeon_profile::on_list_plotDefinitions_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    modifyPlotSchema(item->text(0));
}

void radeon_profile::on_list_plotDefinitions_itemChanged(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);

    switch (item->checkState(0)) {
        case Qt::Unchecked:
            plotManager.modifySchemaState(item->text(0), false);
            return;
        case Qt::Checked:
            plotManager.modifySchemaState(item->text(0), true);
            return;
        default:
            return;
    }
}