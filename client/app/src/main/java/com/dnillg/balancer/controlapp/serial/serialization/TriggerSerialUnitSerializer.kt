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

    override fun serializeParams(unit: SerialUnit): String {
        unit as TriggerSerialUnit
        return "${unit.triggerType},${unit.userData}"
    }

    override fun deserializeParams(line: String): TriggerSerialUnit {
        val parts = line.split(",")
        return TriggerSerialUnit(TriggerType.valueOf(parts[0]), parts[1]);
    }
}