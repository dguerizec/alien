#ifndef MACROEDITOR_H
#define MACROEDITOR_H

#include <QWidget>
#include <QVector3D>
#include <QMatrix>

#include "model/definitions.h"
#include "model/entities/cellto.h"

namespace Ui {
class MacroEditor;
}

class Cell;
class CellCluster;
class EnergyParticle;
class MetadataManager;
class PixelUniverse;
class ShapeUniverse;
class QGraphicsView;
class MacroEditor : public QWidget
{
    Q_OBJECT

public:
    enum ActiveScene {
        PIXEL_SCENE,
        SHAPE_SCENE
    };

    explicit MacroEditor(QWidget *parent = 0);
    ~MacroEditor();

    void reset ();

    void setActiveScene (ActiveScene activeScene);
    QVector3D getViewCenterPosWithInc ();
    void getExtendedSelection (QList< CellCluster* >& clusters, QList< EnergyParticle* >& es);

    void serializeViewMatrix (QDataStream& stream);
    void loadViewMatrix (QDataStream& stream);

    QGraphicsView* getGraphicsView ();

    qreal getZoomFactor ();

signals:
    void requestNewCell (QVector3D pos);    //for simulator
    void requestNewEnergyParticle (QVector3D pos);    //for simulator
    void defocus ();                        //for microeditor
    void focusCell (Cell* cell);       //for microeditor
    void focusEnergyParticle (EnergyParticle* e);       //for microeditor
    void updateCell (QList< Cell* > cells,
                     QList< CellTO > newCellsData,
                     bool clusterDataChanged);      //for simulator
    void energyParticleUpdated (EnergyParticle* e); //for microeditor
    void entitiesSelected (int numCells, int numEnergyParticles);   //for microeditor
    void delSelection (QList< Cell* > cells,
                      QList< EnergyParticle* > es);               //for simulator
    void delExtendedSelection (QList< CellCluster* > clusters,
                         QList< EnergyParticle* > es);               //for simulator


public slots:
    void zoomIn ();
    void zoomOut ();

    void newCellRequested ();
    void newEnergyParticleRequested ();

    void defocused ();
    void delSelection_Slot ();
    void delExtendedSelection_Slot ();
    void cellCreated (Cell* cell);
    void energyParticleCreated (EnergyParticle* e);
    void energyParticleUpdated_Slot (EnergyParticle* e);
    void reclustered (QList< CellCluster* > clusters);
    void universeUpdated (SimulationContext* context, bool force);
    void metadataUpdated ();

private slots:
    void updateTimerTimeout ();

private:
    void centerView (SimulationContext* context);

    Ui::MacroEditor *ui;

    SimulationContext* _context = nullptr;
    ActiveScene _activeScene;
    PixelUniverse* _pixelUniverse;
    ShapeUniverse* _shapeUniverse;

    bool _pixelUniverseInit = false;
    bool _shapeUniverseInit = false;
    QMatrix _pixelUniverseViewMatrix;
    QMatrix _shapeUniverseViewMatrix;
    int _pixelUniversePosX, _pixelUniversePosY;
    int _shapeUniversePosX, _shapeUniversePosY;

    qreal _posIncrement = 0.0;

    //timer for limiting screen updates
    QTimer* _updateTimer = nullptr;
    bool _screenUpdatePossible = true;
};

#endif // MACROEDITOR_H









