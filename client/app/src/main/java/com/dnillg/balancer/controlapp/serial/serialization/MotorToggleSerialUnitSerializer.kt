package com.dnillg.balancer.controlapp.serial.serialization

import com.dnillg.balancer.controlapp.serial.model.MotorToggleSerialUnit

class MotorToggleSerialUnitSerializer : NoParamSerialUnitSerializer<MotorToggleSerialUnit> {

    override fun id(): String {
        return "MOTTOG"
    }

    override fun unitClass(): Class<MotorToggleSerialUnit> {
        return MotorToggleSerialUnit::class.java
    }

}