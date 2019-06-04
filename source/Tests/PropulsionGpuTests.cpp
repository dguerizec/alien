#include "Base/ServiceLocator.h"
#include "ModelBasic/CellComputerCompiler.h"
#include "ModelBasic/QuantityConverter.h"

#include "IntegrationGpuTestFramework.h"

class PropulsionGpuTests
    : public IntegrationGpuTestFramework
{
public:
    PropulsionGpuTests() : IntegrationGpuTestFramework({ 10, 10 })
    {}

    virtual ~PropulsionGpuTests() = default;

protected:
    const float SmallVelocity = 0.005f;
    const float SmallAngularVelocity = 0.05f;
    const float NeglectableVelocity = 0.001f;
    const float NeglectableAngularVelocity = 0.01f;

protected:
    virtual void SetUp();

    DataDescription runPropulsion(Enums::PropIn::Type command, unsigned char propAngle,
        unsigned char propPower, QVector2D const& vel = QVector2D{}, float angle = 0,
        float angularVel = 0.0f) const;

    ClusterDescription createClusterForPropulsionTest(Enums::PropIn::Type command,
        unsigned char propAngle, unsigned char propPower, QVector2D const& vel, 
        float angle, float angularVel) const;

    pair<Physics::Velocities, Enums::PropOut::Type> extractResult(DataDescription const& data);
};


void PropulsionGpuTests::SetUp()
{
    _parameters.radiationProb = 0;    //exclude radiation
    _context->setSimulationParameters(_parameters);
}

ClusterDescription PropulsionGpuTests::createClusterForPropulsionTest(Enums::PropIn::Type command, 
    unsigned char propAngle, unsigned char propPower, QVector2D const& vel, float angle, 
    float angularVel) const
{
    auto result = createLineCluster(2, QVector2D{}, vel, angle, angularVel);
    auto& firstCell = result.cells->at(0);
    firstCell.tokenBranchNumber = 0;
    auto& secondCell = result.cells->at(1);
    secondCell.tokenBranchNumber = 1;
    secondCell.cellFeature = CellFeatureDescription().setType(Enums::CellFunction::PROPULSION);
    auto token = createSimpleToken();
    auto& tokenData = *token.data;
    tokenData[Enums::Prop::IN] = command;
    tokenData[Enums::Prop::IN_ANGLE] = propAngle;
    tokenData[Enums::Prop::IN_POWER] = propPower;
    firstCell.addToken(token);
    return result;
}

DataDescription PropulsionGpuTests::runPropulsion(Enums::PropIn::Type command,
    unsigned char propAngle, unsigned char propPower, QVector2D const& vel,
    float angle, float angularVel) const
{
    DataDescription origData;
    origData.addCluster(createClusterForPropulsionTest(command, propAngle, propPower, vel, angle, angularVel));

    IntegrationTestHelper::updateData(_access, origData);
    IntegrationTestHelper::runSimulation(1, _controller);

    DataDescription newData = IntegrationTestHelper::getContent(_access, { { 0, 0 },{ _universeSize.x, _universeSize.y } });

    checkEnergy(origData, newData);
    return newData;
}

pair<Physics::Velocities, Enums::PropOut::Type> PropulsionGpuTests::extractResult(DataDescription const & data)
{
    pair<Physics::Velocities, Enums::PropOut::Type> result;
    auto const& cluster = data.clusters->at(0);
    CellDescription cell;
    for (auto const& cellToCheck : *cluster.cells) {
        if (cellToCheck.tokens && 1 == cellToCheck.tokens->size()) {
            cell = cellToCheck;
        }
    }
    auto token = cell.tokens->at(0);
    result.first = { *cluster.vel, *cluster.angularVel };
    result.second = static_cast<Enums::PropOut::Type>(token.data->at(Enums::Prop::OUT));
    return result;
}

TEST_F(PropulsionGpuTests, testDoNothing)
{
    auto data = runPropulsion(Enums::PropIn::DO_NOTHING, 0, 100, QVector2D{}, 180);
    auto result = extractResult(data);
    auto const& velocities = result.first;
    auto const& propOut = result.second;

    EXPECT_EQ(Enums::PropOut::SUCCESS, propOut);
    EXPECT_TRUE(isCompatible(QVector2D{}, velocities.linear));
    EXPECT_TRUE(isCompatible(0.0, velocities.angular));
}

TEST_F(PropulsionGpuTests, testThrustControlByAngle1)
{
    auto data = runPropulsion(Enums::PropIn::BY_ANGLE, 0, 100, QVector2D{}, 180);
    auto result = extractResult(data);
    auto const& velocities = result.first;
    auto const& propOut = result.second;

    EXPECT_EQ(Enums::PropOut::SUCCESS, propOut);
    EXPECT_LT(SmallVelocity, velocities.linear.x());
    EXPECT_TRUE(abs(velocities.linear.y()) < NeglectableVelocity);
    EXPECT_TRUE(abs(velocities.angular) < NeglectableAngularVelocity);
}

TEST_F(PropulsionGpuTests, testThrustControlByAngle2)
{
    auto data = runPropulsion(Enums::PropIn::BY_ANGLE, QuantityConverter::convertAngleToData(90), 100, QVector2D{}, 180);
    auto result = extractResult(data);
    auto const& velocities = result.first;
    auto const& propOut = result.second;

    EXPECT_EQ(Enums::PropOut::SUCCESS, propOut);
    EXPECT_TRUE(abs(velocities.linear.x()) < NeglectableVelocity);
    EXPECT_LT(SmallVelocity, velocities.linear.y());
    EXPECT_GT(-SmallAngularVelocity, velocities.angular);
}

TEST_F(PropulsionGpuTests, testThrustControlByAngle3)
{
    auto data = runPropulsion(Enums::PropIn::BY_ANGLE, QuantityConverter::convertAngleToData(180), 100, QVector2D{}, 180);
    auto result = extractResult(data);
    auto const& velocities = result.first;
    auto const& propOut = result.second;

    EXPECT_EQ(Enums::PropOut::SUCCESS, propOut);
    EXPECT_GT(-SmallVelocity, velocities.linear.x());
    EXPECT_TRUE(abs(velocities.linear.y()) < NeglectableVelocity);
    EXPECT_TRUE(abs(velocities.angular) < NeglectableAngularVelocity);
}

TEST_F(PropulsionGpuTests, testThrustControlByAngle4)
{
    auto data = runPropulsion(Enums::PropIn::BY_ANGLE, QuantityConverter::convertAngleToData(270), 100, QVector2D{}, 180);
    auto result = extractResult(data);
    auto const& velocities = result.first;
    auto const& propOut = result.second;

    EXPECT_EQ(Enums::PropOut::SUCCESS, propOut);
    EXPECT_TRUE(abs(velocities.linear.x()) < NeglectableVelocity);
    EXPECT_GT(-SmallVelocity, velocities.linear.y());
    EXPECT_LT(SmallAngularVelocity, velocities.angular);
}

TEST_F(PropulsionGpuTests, testThrustControlFromCenter)
{
    auto data = runPropulsion(Enums::PropIn::FROM_CENTER, 0, 100, QVector2D{}, 180);
    auto result = extractResult(data);
    auto const& velocities = result.first;
    auto const& propOut = result.second;

    EXPECT_EQ(Enums::PropOut::SUCCESS, propOut);
    EXPECT_GT(-SmallVelocity, velocities.linear.x());
    EXPECT_TRUE(abs(velocities.linear.y()) < NeglectableVelocity);
    EXPECT_TRUE(abs(velocities.angular) < NeglectableAngularVelocity);
}

TEST_F(PropulsionGpuTests, testThrustControlTowardCenter)
{
    auto data = runPropulsion(Enums::PropIn::TOWARD_CENTER, 0, 100, QVector2D{}, 180);
    auto result = extractResult(data);
    auto const& velocities = result.first;
    auto const& propOut = result.second;

    EXPECT_EQ(Enums::PropOut::SUCCESS, propOut);
    EXPECT_LT(SmallVelocity, velocities.linear.x());
    EXPECT_TRUE(abs(velocities.linear.y()) < NeglectableVelocity);
    EXPECT_TRUE(abs(velocities.angular) < NeglectableAngularVelocity);
}

TEST_F(PropulsionGpuTests, testThrustControlRotationClockwise)
{
    auto data = runPropulsion(Enums::PropIn::ROTATION_CLOCKWISE, 0, 100, QVector2D{}, 180);
    auto result = extractResult(data);
    auto const& velocities = result.first;
    auto const& propOut = result.second;

    EXPECT_EQ(Enums::PropOut::SUCCESS, propOut);
    EXPECT_TRUE(abs(velocities.linear.x()) < NeglectableVelocity);
    EXPECT_GT(-SmallVelocity, velocities.linear.y());
    EXPECT_LT(SmallAngularVelocity, velocities.angular);
}

TEST_F(PropulsionGpuTests, testThrustControlRotationCounterClockwise)
{
    auto data = runPropulsion(Enums::PropIn::ROTATION_COUNTERCLOCKWISE, 0, 100, QVector2D{}, 180);
    auto result = extractResult(data);
    auto const& velocities = result.first;
    auto const& propOut = result.second;

    EXPECT_EQ(Enums::PropOut::SUCCESS, propOut);
    EXPECT_TRUE(abs(velocities.linear.x()) < NeglectableVelocity);
    EXPECT_LT(SmallVelocity, velocities.linear.y());
    EXPECT_GT(-SmallAngularVelocity, velocities.angular);
}

TEST_F(PropulsionGpuTests, testThrustControlDampRotation1)
{
    auto data = runPropulsion(Enums::PropIn::DAMP_ROTATION, 0, 100, QVector2D{}, 180, 10);
    auto result = extractResult(data);
    auto const& velocities = result.first;
    auto const& propOut = result.second;

    EXPECT_EQ(Enums::PropOut::SUCCESS, propOut);
    EXPECT_GT(10 - SmallAngularVelocity, velocities.angular);
}

TEST_F(PropulsionGpuTests, testThrustControlDampRotation2)
{
    auto data = runPropulsion(Enums::PropIn::DAMP_ROTATION, 0, 100, QVector2D{}, 180, -10);
    auto result = extractResult(data);
    auto const& velocities = result.first;
    auto const& propOut = result.second;

    EXPECT_EQ(Enums::PropOut::SUCCESS, propOut);
    EXPECT_LT(-10 + SmallAngularVelocity, velocities.angular);
}

TEST_F(PropulsionGpuTests, testPowerControl)
{
    DataDescription origData;
    auto&& cluster1 = createClusterForPropulsionTest(Enums::PropIn::FROM_CENTER, 0, 1, QVector2D{}, 0, 0);
    auto&& cluster2 = createClusterForPropulsionTest(Enums::PropIn::FROM_CENTER, 0, 10, QVector2D{}, 0, 0);
    auto&& cluster3 = createClusterForPropulsionTest(Enums::PropIn::FROM_CENTER, 0, 255, QVector2D{}, 0, 0);
    setCenterPos(cluster1, { 0, 0 });
    setCenterPos(cluster2, { 5, 0 });
    setCenterPos(cluster3, { 0, 5 });
    origData.addCluster(cluster1);
    origData.addCluster(cluster2);
    origData.addCluster(cluster3);

    IntegrationTestHelper::updateData(_access, origData);
    IntegrationTestHelper::runSimulation(1, _controller);

    DataDescription newData = IntegrationTestHelper::getContent(_access, { { 0, 0 },{ _universeSize.x, _universeSize.y } });
    checkEnergy(origData, newData);

    auto clusterByClusterId = IntegrationTestHelper::getClusterByClusterId(newData);
    auto const& newCluster1 = clusterByClusterId.at(cluster1.id);
    auto const& newCluster2 = clusterByClusterId.at(cluster2.id);
    auto const& newCluster3 = clusterByClusterId.at(cluster2.id);

    EXPECT_GT(newCluster1.vel->length() * 2, newCluster2.vel->length() );
    EXPECT_GT(newCluster2.vel->length() * 2, newCluster3.vel->length());
}

TEST_F(PropulsionGpuTests, testSlowdown)
{
    auto data = runPropulsion(Enums::PropIn::TOWARD_CENTER, 0, 100, QVector2D{0.3f, 0});
    auto result = extractResult(data);
    auto const& velocities = result.first;
    auto const& propOut = result.second;

    EXPECT_EQ(Enums::PropOut::SUCCESS, propOut);
    EXPECT_GT(0.3f - SmallVelocity, velocities.linear.length());
}
