#ifndef SPACEMETRICIMPL_H
#define SPACEMETRICIMPL_H

#include "model/context/SpaceMetric.h"

class SpaceMetricImpl
	: public SpaceMetric
{
public:
	SpaceMetricImpl(QObject* parent = nullptr);
	virtual ~SpaceMetricImpl() {}

	virtual void init(IntVector2D size);
	virtual SpaceMetric* clone(QObject* parent = nullptr) const override;

	virtual IntVector2D getSize() const override;

	virtual void correctPosition(QVector2D& pos) const override;
	virtual IntVector2D correctPositionWithIntPrecision(QVector2D const& pos) const override;
	virtual IntVector2D shiftPosition(IntVector2D const& pos, IntVector2D const && shift) const override;
	virtual void correctDisplacement(QVector2D& displacement) const override;
	virtual QVector2D displacement(QVector2D fromPoint, QVector2D toPoint) const override;
	virtual qreal distance(QVector2D fromPoint, QVector2D toPoint) const override;
	virtual QVector2D correctionIncrement(QVector2D pos1, QVector2D pos2) const override;

	virtual void serializePrimitives(QDataStream& stream) const override;
	virtual void deserializePrimitives(QDataStream& stream) override;

private:
	inline void correctPosition(IntVector2D & pos) const;

	IntVector2D _size{ 0, 0 };
};

void SpaceMetricImpl::correctPosition(IntVector2D & pos) const
{
	pos.x = ((pos.x % _size.x) + _size.x) % _size.x;
	pos.y = ((pos.y % _size.y) + _size.y) % _size.y;
}

#endif // SPACEMETRICIMPL_H
