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

    override fun serialize(unit: SerialUnit): String {
        unit as SetPIDSerialUnit
        return "${shortFloat(unit.p)},${shortFloat(unit.i)},${shortFloat(unit.d)}"
    }

    override fun deserialize(line: String): SetPIDSerialUnit {
        val parts = line.split(",")
        return SetPIDSerialUnit(PIDType.valueOf(parts[0]), parts[1].toFloat(), parts[2].toFloat(), parts[3].toFloat())
    }

}