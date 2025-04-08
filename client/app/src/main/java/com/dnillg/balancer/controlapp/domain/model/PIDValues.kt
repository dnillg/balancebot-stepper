package com.dnillg.balancer.controlapp.domain.model

import kotlin.math.abs

private const val MIN_ABS_VALUE = 0.000001f

data class PIDValues(
  val pidType: PIDType? = null,
  val p: Float = 0.0f,
  val i: Float = 0.0f,
  val d: Float = 0.0f,
) {
  fun incP(prop: Double) = this.copy(p = calcNewValue(this.p, prop))
  fun incI(prop: Double) = this.copy(i = calcNewValue(this.i, prop))
  fun incD(prop: Double) = this.copy(d = calcNewValue(this.d, prop))

  private fun calcNewValue(prevValue: Float, absIncProp: Double): Float {
    var prop = absIncProp;
    if (prevValue < 0f) {
      prop = -absIncProp
    }
    val newValue = prevValue * (1.0f + absIncProp)
    if (abs(newValue) < MIN_ABS_VALUE && newValue > 0f && prop < 0.0f) {
      return 0f
    }
    if (abs(newValue) < MIN_ABS_VALUE && newValue < 0f && prop > 0.0f) {
      return 0f
    }

    if (abs(newValue) < MIN_ABS_VALUE && prop < 0f) {
      return -MIN_ABS_VALUE
    }
    if (abs(newValue) < MIN_ABS_VALUE && prop > 0f) {
      return MIN_ABS_VALUE
    }

    return newValue.toFloat()
  }

  fun initialized(): Boolean {
    return (p != 0.0f || i != 0.0f || d != 0.0f) && pidType != null
  }

}