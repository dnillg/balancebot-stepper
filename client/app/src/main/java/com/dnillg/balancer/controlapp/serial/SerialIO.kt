package com.dnillg.balancer.controlapp.serial

import com.dnillg.balancer.controlapp.serial.model.ErrorSerialUnit
import com.dnillg.balancer.controlapp.serial.model.SerialUnit
import com.dnillg.balancer.controlapp.serial.serialization.SerialUnitSerializer

class SerialIO(
  private val serializers: Set<SerialUnitSerializer<*>>
) {

  fun send(serialUnit: SerialUnit) : String {
    for (serializer in serializers) {
      if (serializer.canSerialize(serialUnit)) {
        return serializer.serialize(serialUnit)
      }
    }
    throw IllegalArgumentException("No serializer found for $serialUnit")
  }

  fun receive(line: String): SerialUnit {
    val parts = line.split(">")
    if (parts.size != 2) {
      return ErrorSerialUnit("Invalid line format")
    }
    val id = parts[0]
    val data = parts[1]
    for (serializer in serializers) {
      if (serializer.id() == id) {
        return serializer.deserialize(data)
      }
    }
    return ErrorSerialUnit("No serializer found for $id")
  }

}