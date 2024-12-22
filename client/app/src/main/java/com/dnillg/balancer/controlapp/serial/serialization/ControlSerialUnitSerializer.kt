package com.dnillg.balancer.controlapp.serial.serialization

import com.dnillg.balancer.controlapp.serial.model.ControlSerialUnit
import com.dnillg.balancer.controlapp.serial.model.SerialUnit

class ControlSerialUnitSerializer : SerialUnitSerializer<ControlSerialUnit> {

    override fun id(): String {
        return "CTRL"
    }

    override fun unitClass(): Class<ControlSerialUnit> {
        return ControlSerialUnit::class.java
    }

    override fun serialize(unit: SerialUnit): String {
        unit as ControlSerialUnit
        return "${shortFloat(unit.speed)},${shortFloat(unit.steer)}"
    }

    override fun deserialize(line: String): ControlSerialUnit {
        val parts = line.split(",")
        return ControlSerialUnit(parts[0].toFloat(), parts[1].toFloat())
    }

}