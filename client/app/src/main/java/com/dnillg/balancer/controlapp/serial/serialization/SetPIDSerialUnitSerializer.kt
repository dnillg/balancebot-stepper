package com.dnillg.balancer.controlapp.serial.serialization

import com.dnillg.balancer.controlapp.domain.model.PIDType
import com.dnillg.balancer.controlapp.serial.model.SerialUnit
import com.dnillg.balancer.controlapp.serial.model.SetPIDSerialUnit

class SetPIDSerialUnitSerializer : SerialUnitSerializer<SetPIDSerialUnit> {

    override fun id(): String {
        return "SETPID"
    }

    override fun unitClass(): Class<SetPIDSerialUnit> {
        return SetPIDSerialUnit::class.java
    }

    override fun serializeParams(unit: SerialUnit): String {
        unit as SetPIDSerialUnit
        return "${unit.type},${longFloat(unit.p)},${longFloat(unit.i)},${longFloat(unit.d)}"
    }

    override fun deserializeParams(line: String): SetPIDSerialUnit {
        val parts = line.split(",")
        return SetPIDSerialUnit(PIDType.valueOf(parts[0]), parts[1].toFloat(), parts[2].toFloat(), parts[3].toFloat())
    }

}