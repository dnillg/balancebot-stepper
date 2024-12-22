package com.dnillg.balancer.controlapp.serial.model

import com.dnillg.balancer.controlapp.domain.model.PIDType

class GetPIDResponseSerialUnit (
    val type: PIDType,
    val p: Float,
    val i: Float,
    val d: Float
) : SerialUnit
