package com.dnillg.balancer.controlapp.serial.model

data class DiagDataSerialUnit (
    val seqNo: Int,
    val roll: Float,
    val targetRoll: Float,
    val speed: Float,
    val targetSpeed: Float,
    val motorLeft: Float,
    val motorRight: Float,
) : SerialUnit