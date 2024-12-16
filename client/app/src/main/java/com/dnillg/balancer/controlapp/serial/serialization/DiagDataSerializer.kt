package com.dnillg.balancer.controlapp.serial.serialization

import com.dnillg.balancer.controlapp.serial.model.DiagDataSerialUnit
import com.dnillg.balancer.controlapp.serial.model.SerialUnit
import com.dnillg.balancer.controlapp.serial.serialization.SerialUnitSerializer.Companion.SEPARATOR

class DiagDataSerializer : SerialUnitSerializer<DiagDataSerialUnit> {

    override fun id(): String {
      return "DIAG"
    }

    override fun unitClass(): Class<DiagDataSerialUnit> {
      return DiagDataSerialUnit::class.java
    }

    override fun serialize(unit: SerialUnit): String {
      unit as DiagDataSerialUnit
      return "${shortFloat(unit.roll)},${shortFloat(unit.targetRoll)},${shortFloat(unit.speed)},${shortFloat(unit.targetSpeed)},${shortFloat(unit.motorLeft)},${shortFloat(unit.motorRight)}"
    }

    override fun deserialize(line: String): DiagDataSerialUnit {
      val parts = line.split(SEPARATOR)
      return DiagDataSerialUnit(
        parts[0].toFloat(),
        parts[1].toFloat(),
        parts[2].toFloat(),
        parts[3].toFloat(),
        parts[4].toFloat(),
        parts[5].toFloat()
      )
    }

}