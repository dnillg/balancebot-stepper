package com.dnillg.balancer.controlapp.domain.model

data class Setting (
  val id: String,
  val name: String,
  val clazz: Class<*>,
) {
  companion object {
    val settings = listOf(
      Setting("TMC_RSENSE", "TMC5160T: R-Sense", Integer::class.java),
      Setting("TMC_TOFF", "TMC5160T: TOFF", Integer::class.java),
      Setting("TMC_IRUN", "TMC5160T: IRUN", Integer::class.java),
      Setting("TMC_IHOLD", "TMC5160T: IHOLD", Integer::class.java),
      Setting("TMC_IHOLDDELAY", "TMC5160T: IHOLDDELAY", Integer::class.java),
      Setting("MAX_MOT_SPEED", "Max. Motor Speed", Integer::class.java),
      Setting("MAX_MOT_ACC", "Max. Motor Acceleration", Integer::class.java),
      Setting("BALANCE_ROLL", "Balance Target Roll", Double::class.java),
      Setting("MAX_TARGET_ROLL_OFFSET", "Remote Control: Roll Offset", Double::class.java),
      Setting("MAX_TARGET_SPEED", "Remote Control: Max Speed", Integer::class.java),
      Setting("MAX_STEER_OFFSET", "Remote Control: Max. Steer Offset", Integer::class.java),
    )
  }
}

abstract class SettingValue <T>(
  val value: T?,
  val initialized: Boolean
) {

  abstract fun withValue(value: T) : SettingValue<T>
}

class SettingBooleanValue(
  value: Boolean = false,
  initialized: Boolean = false
) : SettingValue<Boolean>(value, initialized)
{
  override fun withValue(value: Boolean) : SettingBooleanValue {
    return SettingBooleanValue(value = value, initialized = true)
  }
}

class SettingIntegerValue(
  value: Int = 0,
  initialized: Boolean = false
) : SettingValue<Int>(value, initialized)
{
  override fun withValue(value: Int) : SettingIntegerValue {
    return SettingIntegerValue(value = value, initialized = true)
  }

  fun withAddedValue(value: Int) : SettingIntegerValue {
    return SettingIntegerValue(value = this.value!! + value, initialized = true)
  }
}

class SettingDoubleValue(
  value: Double = 0.0,
  initialized: Boolean = false
) : SettingValue<Double>(value, initialized)
{
  override fun withValue(value: Double) : SettingDoubleValue {
    return SettingDoubleValue(value = value, initialized = true)
  }

  fun WithAddedValue(value: Double) : SettingDoubleValue {
    return SettingDoubleValue(value = this.value!! + value, initialized = true)
  }

  fun withMultipliedValue(value: Double) : SettingDoubleValue {
    return SettingDoubleValue(value = this.value!! * value, initialized = true)
  }

}

class SettingEnumValue(
  value: Enum<*>? = null,
  initialized: Boolean = false
) : SettingValue<Enum<*>>(value, initialized)
{
  override fun withValue(value: Enum<*>) : SettingEnumValue {
    return SettingEnumValue(value = value, initialized = true)
  }
}


