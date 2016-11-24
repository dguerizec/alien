#ifndef CELLFUNCTIONWEAPON_H
#define CELLFUNCTIONWEAPON_H

#include "model/features/cellfunction.h"

class CellFunctionWeapon : public CellFunction
{
public:
    CellFunctionWeapon (Grid* grid);

    CellFunctionType getType () const { return CellFunctionType::WEAPON; }

protected:
    ProcessingResult processImpl (Token* token, Cell* cell, Cell* previousCell);
};

#endif // CELLFUNCTIONWEAPON_H
