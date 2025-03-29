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

    override fun serializeParams(unit: SerialUnit): String {
      unit as DiagDataSerialUnit
      return "${unit.seqNo},${shortFloat(unit.roll)},${shortFloat(unit.targetRoll)},${shortFloat(unit.speed)},${shortFloat(unit.targetSpeed)},${shortFloat(unit.motorLeft)},${shortFloat(unit.motorRight)}"
    }

    override fun deserializeParams(line: String): DiagDataSerialUnit {
      val parts = line.split(SEPARATOR)
      var i = 0;
      return DiagDataSerialUnit(
        parts[i++].toInt(),
        parts[i++].toFloat(),
        parts[i++].toFloat(),
        parts[i++].toFloat(),
        parts[i++].toFloat(),
        parts[i++].toFloat(),
        parts[i++].toFloat()
      )
    }

}