package com.dnillg.balancer.controlapp.domain.model

data class Setting (
  val id: String,
  val name: String,
  val stateClazz: Class<out SettingState<*>>,
) {

  fun createState() : SettingState<*> {
    return when (stateClazz) {
      SettingBooleanState::class.java -> SettingBooleanState(this)
      SettingIntegerState::class.java -> SettingIntegerState(this)
      SettingDoubleState::class.java -> SettingDoubleState(this)
      SettingEnumValue::class.java -> SettingEnumValue(this)
      else -> throw IllegalArgumentException("Unknown state class: $stateClazz")
    }
  }

  companion object {
    val settings = listOf(
      Setting("TMC_RSENSE", "TMC5160T: R-Sense", SettingIntegerState::class.java),
      Setting("TMC_TOFF", "TMC5160T: TOFF", SettingIntegerState::class.java),
      Setting("TMC_IRUN", "TMC5160T: IRUN", SettingIntegerState::class.java),
      Setting("TMC_IHOLD", "TMC5160T: IHOLD", SettingIntegerState::class.java),
      Setting("TMC_IHOLDDELAY", "TMC5160T: IHOLDDELAY", SettingIntegerState::class.java),
      Setting("MAX_MOT_SPEED", "Max. Motor Speed", SettingIntegerState::class.java),
      Setting("MAX_MOT_ACC", "Max. Motor Acceleration", SettingIntegerState::class.java),
      Setting("BALANCE_ROLL", "Balance Target Roll", SettingDoubleState::class.java),
      Setting("MAX_TARGET_ROLL_OFFSET", "Remote Control: Roll Offset", SettingIntegerState::class.java),
      Setting("MAX_TARGET_SPEED", "Remote Control: Max Speed", SettingIntegerState::class.java),
      Setting("MAX_STEER_OFFSET", "Remote Control: Max. Steer Offset", SettingIntegerState::class.java),
    )
  }
}

abstract class SettingState <T>(
  val setting: Setting,
  val value: T?,
  val initialized: Boolean
) {

  abstract fun withValue(value: T) : SettingState<T>
}

class SettingBooleanState(
  setting: Setting,
  value: Boolean = false,
  initialized: Boolean = false
) : SettingState<Boolean>(setting, value, initialized)
{
  override fun withValue(value: Boolean) : SettingBooleanState {
    return SettingBooleanState(setting, value = value, initialized = true)
  }
}

class SettingIntegerState(
  setting: Setting,
  value: Int = 0,
  initialized: Boolean = false
) : SettingState<Int>(setting, value, initialized)
{
  override fun withValue(value: Int) : SettingIntegerState {
    return SettingIntegerState(setting, value = value, initialized = true)
  }

  fun withAddedValue(value: Int) : SettingIntegerState {
    return SettingIntegerState(setting, value = this.value!! + value, initialized = true)
  }
}

class SettingDoubleState(
  setting: Setting,
  value: Double = 0.0,
  initialized: Boolean = false
) : SettingState<Double>(setting, value, initialized)
{
  override fun withValue(value: Double) : SettingDoubleState {
    return SettingDoubleState(setting, value = value, initialized = true)
  }

  fun WithAddedValue(value: Double) : SettingDoubleState {
    return SettingDoubleState(setting, value = this.value!! + value, initialized = true)
  }

  fun withMultipliedValue(value: Double) : SettingDoubleState {
    return SettingDoubleState(setting, value = this.value!! * value, initialized = true)
  }

}

class SettingEnumValue(
  setting: Setting,
  value: Enum<*>? = null,
  initialized: Boolean = false
) : SettingState<Enum<*>>(setting, value, initialized)
{
  override fun withValue(value: Enum<*>) : SettingEnumValue {
    return SettingEnumValue(setting, value = value, initialized = true)
  }
}


