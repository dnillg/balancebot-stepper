package com.dnillg.balancer.controlapp.serial

import com.dnillg.balancer.controlapp.serial.model.ErrorSerialUnit
import com.dnillg.balancer.controlapp.serial.model.SerialUnit
import com.dnillg.balancer.controlapp.serial.serialization.SerialUnitSerializer
import android.util.Log

class SerialIO(
  private val serializers: Set<SerialUnitSerializer<*>>
) {

  fun send(serialUnit: SerialUnit) : String {
    for (serializer in serializers) {
      if (serializer.canSerialize(serialUnit)) {
        return serializer.id() + ">" + serializer.serializeParams(serialUnit)
      }
    }
    throw IllegalArgumentException("No serializer found for $serialUnit")
  }

  fun receive(line: String): SerialUnit {
    try {
      val parts = line.split(">")
      if (parts.size != 2) {
        return ErrorSerialUnit("Invalid line format")
      }
      val id = parts[0]
      val data = parts[1]
      try {
        for (serializer in serializers) {
          if (serializer.id() == id) {
            return serializer.deserializeParams(data)
          }
        }
      } catch (e: Exception) {
        Log.e(this.javaClass.simpleName, "Error while deserializing message: " + line, e)
      }
      return ErrorSerialUnit("No serializer found for $id")
    } catch (e: Exception) {
      Log.e(this.javaClass.simpleName, "Error while reading message: " + line, e)
      return ErrorSerialUnit(e.message?:"");
    }
  }

}