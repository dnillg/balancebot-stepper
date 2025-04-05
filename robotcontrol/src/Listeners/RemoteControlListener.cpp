#include "RemoteControlListener.hpp"

void RemoteControlListener::consume(ISerialUnit *unit)
{
	if (unit->getAlias() == SerialUnitAlias::CTRL)
	{
		auto ctrlSerialUnit = static_cast<ControlSerialUnit *>(unit);
		this->control->setRemoteControlInput(ctrlSerialUnit->getSpeed(), ctrlSerialUnit->getSteer());
	}
}