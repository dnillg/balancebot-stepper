package com.dnillg.balancer.controlapp.serial.serialization

import com.dnillg.balancer.controlapp.serial.model.SerialUnit
import com.dnillg.balancer.controlapp.serial.model.SetConfigSerialUnit

class SetConfigSerialUnitSerializer : SerialUnitSerializer<SetConfigSerialUnit> {

    override fun id(): String {
        return "SETCONF"
    }

    override fun unitClass(): Class<SetConfigSerialUnit> {
        return SetConfigSerialUnit::class.java
    }

    override fun serializeParams(unit: SerialUnit): String {
        unit as SetConfigSerialUnit
        return "${unit.name},${unit.value}"
    }

    override fun deserializeParams(line: String): SetConfigSerialUnit {
        val parts = line.split(",")
        return SetConfigSerialUnit(parts[0], parts[1])
    }

}