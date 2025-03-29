package com.dnillg.balancer.controlapp.serial.serialization

import com.dnillg.balancer.controlapp.serial.model.MotorToggleSerialUnit
import com.dnillg.balancer.controlapp.serial.model.SerialUnit

class MotorToggleSerialUnitSerializer : SerialUnitSerializer<MotorToggleSerialUnit> {

    override fun id(): String {
        return "MOTTOG"
    }

    override fun deserializeParams(line: String): MotorToggleSerialUnit {
        return MotorToggleSerialUnit(line == "1")
    }

    override fun serializeParams(unit: SerialUnit): String {
        unit as MotorToggleSerialUnit
        return if (unit.enabled) "1" else "0"
    }

    override fun unitClass(): Class<MotorToggleSerialUnit> {
        return MotorToggleSerialUnit::class.java
    }

}