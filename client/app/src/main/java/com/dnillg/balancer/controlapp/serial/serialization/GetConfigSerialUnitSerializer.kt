package com.dnillg.balancer.controlapp.serial.serialization

import com.dnillg.balancer.controlapp.serial.model.GetConfigSerialUnit
import com.dnillg.balancer.controlapp.serial.model.SerialUnit

class GetConfigSerialUnitSerializer : SerialUnitSerializer<GetConfigSerialUnit> {

    override fun id(): String {
        return "GETCONF"
    }

    override fun unitClass(): Class<GetConfigSerialUnit> {
        return GetConfigSerialUnit::class.java
    }

    override fun serializeParams(unit: SerialUnit): String {
        unit as GetConfigSerialUnit
        return unit.name
    }

    override fun deserializeParams(line: String): GetConfigSerialUnit {
        val parts = line.split(",")
        return GetConfigSerialUnit(parts[0])
    }

}