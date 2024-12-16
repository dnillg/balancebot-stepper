package com.dnillg.balancer.controlapp.serial.serialization

import com.dnillg.balancer.controlapp.serial.model.SerialUnit
import java.text.DecimalFormat

interface SerialUnitSerializer<T : SerialUnit> {

    fun id(): String
    fun unitClass(): Class<T>
    fun canSerialize(unit: SerialUnit): Boolean {
        return unit::class.java == unitClass()
    }

    fun serialize(unit: SerialUnit): String
    fun deserialize(line: String): T

    fun shortFloat(value: Float): String {
        return SHORT_FLOAT.format(value)
    }

    fun longFloat(value: Float): String {
        return LONG_FLOAT.format(value)
    }

    companion object {
        val SHORT_FLOAT = DecimalFormat("#.###")
        val LONG_FLOAT = DecimalFormat("#.######")
        const val SEPARATOR = ",";
    }

}