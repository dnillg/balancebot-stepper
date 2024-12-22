package com.dnillg.balancer.controlapp.serial.serialization

import com.dnillg.balancer.controlapp.serial.model.SerialUnit
import java.text.DecimalFormat

interface NoParamSerialUnitSerializer<T : SerialUnit> : SerialUnitSerializer<T> {

    override fun serialize(unit: SerialUnit): String {
        return ""
    }
    override fun deserialize(line: String): T {
        return unitClass().newInstance()
    }

}