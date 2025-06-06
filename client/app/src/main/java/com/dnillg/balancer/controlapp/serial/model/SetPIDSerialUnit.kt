package com.dnillg.balancer.controlapp.serial.model

import com.dnillg.balancer.controlapp.domain.model.PIDType

class SetPIDSerialUnit (
    val type: PIDType,
    val p: Float,
    val i: Float,
    val d: Float
) : SerialUnit