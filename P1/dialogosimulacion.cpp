#include "dialogosimulacion.h"
#include "ui_dialogosimulacion.h"
#include <QList>
DialogoSimulacion::DialogoSimulacion
(std::list<Lote_shrdptr> &lotes, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogoSimulacion),
    tiempoProcesoActual(0),
    finishedSimulation(false)
{
    ui->setupUi(this);
    opcionesOperadores
            = new std::map<Operador, QString>;
    opcionesOperadores->insert({
                                   {Operador::SUMA,tr("+")},
                                   {Operador::RESTA,tr("-")},
                                   {Operador::MULTIPLICACION,tr("*")},
                                   {Operador::DIVISION,tr("/")},
                                   {Operador::RESTO,tr("%")},
                                   {Operador::RAIZ,tr("sqrt ")}
                                });
    ui->clock->start(1000);
    lotesPendientes=lotes;
    lote_en_ejecucion=lotesPendientes.front();
    lotesPendientes.pop_front();
    proceso_en_ejecucion=lote_en_ejecucion->pop();
    updateLabelProcesoActual();
    updateLabelLotesRestantes();
    modeloListaLoteActual=new QStringListModel(this);
    modeloListaProcesoTerminados=new QStringListModel(this);
    ui->listaLoteActual->setModel(modeloListaLoteActual);
    ui->listaLoteActual->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->listaProcesosTerminados->setModel(modeloListaProcesoTerminados);
    ui->listaProcesosTerminados->setEditTriggers(QAbstractItemView::NoEditTriggers);
    updateListaLoteActual();
}

DialogoSimulacion::~DialogoSimulacion()
{
    delete ui;
    delete modeloListaLoteActual;
    delete modeloListaProcesoTerminados;
}

void DialogoSimulacion::updateLabelLotesRestantes()
{
    ui->labelLotesRestantes->setText(QString::number(lotesPendientes.size()));
}

void DialogoSimulacion::updateListaLoteActual()
{
    QList<QString> items;
    for(auto p:*lote_en_ejecucion)
        items<<tr("Programador:")+
               p->getNombreProgramador()+
               tr("\nTiempo max. de ejecución: ")+
               QString::number(p->getMaxTiempo());
    modeloListaLoteActual->setStringList(items);
}
void DialogoSimulacion::updateListaProcesosTerminados()
{
    QList<QString> items;
    for(auto p:procesosTerminados)
        items<<tr("Id: ")+
               QString::number(p->getId())+
               tr("\n")+
               p->getOperando1()+
               opcionesOperadores->at(p->getOperador())+
               p->getOperando2()+p->getResultado()+tr("=")
               +p->getResultado()+
               tr("\n");
    modeloListaProcesoTerminados->setStringList(items);
}

void DialogoSimulacion::updateLabelProcesoActual(int tiempoTranscurrido)
{
    if(!finishedSimulation) ui->label_proceso_actual->setText(
                tr("Programador:")+proceso_en_ejecucion->getNombreProgramador()+"\n"+
                tr("Operación: ")+proceso_en_ejecucion->getOperando1()+
                    opcionesOperadores->at(proceso_en_ejecucion->getOperador())+
                    proceso_en_ejecucion->getOperando2()+"\n"+
                tr("T.M.E: ")+QString::number(proceso_en_ejecucion->getMaxTiempo())+"\n"+
                tr("Id: ")+QString::number(proceso_en_ejecucion->getId())+"\n"+
                tr("Tiempo Transcurrido: ")+QString::number(tiempoTranscurrido)+"\n"+
                tr("Tiempo Restante: ")+QString::number(
                    proceso_en_ejecucion->getMaxTiempo()-tiempoTranscurrido
                    )
                );
    else ui->label_proceso_actual->setText("");
}
#include<QDebug>
void DialogoSimulacion::changeView(unsigned currTime)
{
    if(!finishedSimulation)
    {
        qDebug()<<"Call me";
        updateLabelProcesoActual(tiempoProcesoActual);
        tiempoProcesoActual++;
        if(tiempoProcesoActual>=proceso_en_ejecucion->getMaxTiempo())
        {
            procesosTerminados.push_back(proceso_en_ejecucion->solve());
            updateListaProcesosTerminados();
            if(lote_en_ejecucion->empty())
            {
                if(lotesPendientes.empty())
                {
                    finishedSimulation=true;
                    proceso_en_ejecucion.reset();
                    updateLabelProcesoActual(tiempoProcesoActual);
                    return;
                }
                else
                {
                    lote_en_ejecucion=lotesPendientes.front();
                    lotesPendientes.pop_front();
                    updateLabelLotesRestantes();
                }
            }
            proceso_en_ejecucion=lote_en_ejecucion->pop();
            updateLabelProcesoActual();
            updateListaLoteActual();
        }
    }

}
