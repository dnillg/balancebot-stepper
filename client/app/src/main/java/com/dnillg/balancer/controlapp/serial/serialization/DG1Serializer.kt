package com.dnillg.balancer.controlapp.serial.serialization

import com.dnillg.balancer.controlapp.serial.model.DG1SerialUnit
import com.dnillg.balancer.controlapp.serial.model.DiagDataSerialUnit
import com.dnillg.balancer.controlapp.serial.model.SerialUnit
import com.dnillg.balancer.controlapp.serial.serialization.SerialUnitSerializer.Companion.SEPARATOR

class DG1Serializer : SerialUnitSerializer<DG1SerialUnit> {

    override fun id(): String {
      return "DG1"
    }

    override fun unitClass(): Class<DG1SerialUnit> {
      return DG1SerialUnit::class.java
    }

    override fun serializeParams(unit: SerialUnit): String {
      unit as DiagDataSerialUnit
      return "${unit.seqNo},${shortFloat(unit.roll)},${shortFloat(unit.targetRoll)}"
    }

    override fun deserializeParams(line: String): DG1SerialUnit {
      val parts = line.split(SEPARATOR)
      var i = 0;
      return DG1SerialUnit(
        parts[i++].toInt(),
        parts[i++].toFloat(),
        parts[i++].toFloat()
      )
    }

}