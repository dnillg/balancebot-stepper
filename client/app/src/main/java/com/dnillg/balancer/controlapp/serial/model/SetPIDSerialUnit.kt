package com.dnillg.balancer.controlapp.serial.model

enum class PIDType {
    ROLL,
    SPEED
}

class SetPIDSerialUnit (
    val type: PIDType,
    val p: Float,
    val i: Float,
    val d: Float
) : SerialUnit