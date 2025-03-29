package com.dnillg.balancer.controlapp.serial.serialization

import com.dnillg.balancer.controlapp.domain.model.PIDType
import com.dnillg.balancer.controlapp.serial.model.GetPIDSerialUnit
import com.dnillg.balancer.controlapp.serial.model.SerialUnit
import com.dnillg.balancer.controlapp.serial.model.SetPIDSerialUnit

class GetPIDSerialUnitSerializer : SerialUnitSerializer<GetPIDSerialUnit> {

    override fun id(): String {
        return "GETPID"
    }

    override fun unitClass(): Class<GetPIDSerialUnit> {
        return GetPIDSerialUnit::class.java
    }

    override fun serializeParams(unit: SerialUnit): String {
        unit as GetPIDSerialUnit
        return unit.type.name
    }

    override fun deserializeParams(line: String): GetPIDSerialUnit {
        val parts = line.split(",")
        return GetPIDSerialUnit(PIDType.valueOf(parts[0]))
    }

}