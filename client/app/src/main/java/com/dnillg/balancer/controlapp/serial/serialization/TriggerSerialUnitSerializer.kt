package com.dnillg.balancer.controlapp.serial.serialization

import com.dnillg.balancer.controlapp.serial.model.SerialUnit
import com.dnillg.balancer.controlapp.serial.model.TriggerSerialUnit
import com.dnillg.balancer.controlapp.serial.model.TriggerType

class TriggerSerialUnitSerializer : SerialUnitSerializer<TriggerSerialUnit> {

    override fun id(): String {
        return "TRIG"
    }

    override fun unitClass(): Class<TriggerSerialUnit> {
        return TriggerSerialUnit::class.java
    }

    override fun serialize(unit: SerialUnit): String {
        unit as TriggerSerialUnit
        return "${unit.triggerType}"
    }

    override fun deserialize(line: String): TriggerSerialUnit {
        return TriggerSerialUnit(TriggerType.valueOf(line));
    }
}