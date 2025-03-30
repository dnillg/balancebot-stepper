package com.dnillg.balancer.controlapp.serial.model

data class DG1SerialUnit (
    val seqNo: Int,
    val roll: Float,
    val targetRoll: Float,
) : SerialUnit