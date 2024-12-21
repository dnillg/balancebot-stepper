package com.dnillg.balancer.controlapp.domain.model

import kotlin.math.abs

data class PIDValues(
  val pidType: PIDType? = null,
  val p: Float = 0.0f,
  val i: Float = 0.0f,
  val d: Float = 0.0f,
) {
  fun incP(prop: Float) = this.copy(p = calcNewValue(this.p, prop))
  fun incI(prop: Float) = this.copy(i = calcNewValue(this.i, prop))
  fun incD(prop: Float) = this.copy(d = calcNewValue(this.d, prop))

  private fun calcNewValue(prevValue: Float, absProp: Float): Float {
    var prop = absProp;
    if (prevValue < 0f) {
      prop = -absProp
    }
    val newValue = prevValue * (1.0f + absProp)
    if (abs(newValue) < 0.001f && newValue > 0f && prop < 0.0f) {
      return 0f
    }
    if (abs(newValue) < 0.001f && newValue < 0f && prop > 0.0f) {
      return 0f
    }

    if (abs(newValue) < 0.001f && prop < 0f) {
      return -0.001f
    }
    if (abs(newValue) < 0.001f && prop > 0f) {
      return 0.001f
    }

    return newValue
  }

  fun initialized(): Boolean {
    return p != 0.0f || i != 0.0f || d != 0.0f
  }
}