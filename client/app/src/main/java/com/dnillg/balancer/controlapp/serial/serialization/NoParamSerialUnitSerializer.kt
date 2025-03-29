package com.dnillg.balancer.controlapp.serial.serialization

import com.dnillg.balancer.controlapp.serial.model.SerialUnit
import java.text.DecimalFormat

interface NoParamSerialUnitSerializer<T : SerialUnit> : SerialUnitSerializer<T> {

    override fun serializeParams(unit: SerialUnit): String {
        return ""
    }
    override fun deserializeParams(line: String): T {
        return unitClass().newInstance()
    }

}