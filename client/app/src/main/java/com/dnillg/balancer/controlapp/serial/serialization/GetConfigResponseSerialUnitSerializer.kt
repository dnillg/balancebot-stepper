package com.dnillg.balancer.controlapp.serial.serialization

import com.dnillg.balancer.controlapp.serial.model.GetConfigResponseSerialUnit
import com.dnillg.balancer.controlapp.serial.model.GetConfigSerialUnit
import com.dnillg.balancer.controlapp.serial.model.GetPIDSerialUnit
import com.dnillg.balancer.controlapp.serial.model.SerialUnit

class GetConfigResponseSerialUnitSerializer : SerialUnitSerializer<GetConfigResponseSerialUnit> {

    override fun id(): String {
        return "GETCONFRSP"
    }

    override fun unitClass(): Class<GetConfigResponseSerialUnit> {
        return GetConfigResponseSerialUnit::class.java
    }

    override fun serializeParams(unit: SerialUnit): String {
        unit as GetConfigResponseSerialUnit
        return unit.name + "," + unit.value
    }

    override fun deserializeParams(line: String): GetConfigResponseSerialUnit {
        val parts = line.split(",")
        return GetConfigResponseSerialUnit(parts[0], parts[1])
    }

}