package com.dnillg.balancer.controlapp.domain.model

data class Setting (
  val id: String,
  val name: String,
  val stateClazz: (Setting) -> SettingState<*>,
) {

  fun createState() : SettingState<*> {
    return stateClazz.invoke(this);
  }

  companion object {
    val settings = listOf(
      Setting("TMC_RSENSE", "TMC5160T: R-Sense") { setting -> SettingIntegerState(setting) },
      Setting("TMC_TOFF", "TMC5160T: TOFF") { setting -> SettingIntegerState(setting) },
      Setting("TMC_IRUN", "TMC5160T: IRUN") { setting -> SettingIntegerState(setting) },
      Setting("TMC_IHOLD", "TMC5160T: IHOLD", { setting -> SettingIntegerState(setting) }),
      Setting("TMC_IHOLDDELAY", "TMC5160T: IHOLDDELAY") { setting -> SettingIntegerState(setting) },
      Setting("MAX_MOT_SPEED", "Max. Motor Speed") { setting -> SettingIntegerState(setting) },
      Setting("MAX_MOT_ACC", "Max. Motor Acceleration") { setting -> SettingIntegerState(setting) },
      Setting("BALANCE_ROLL", "Balance Target Roll") { setting -> SettingDoubleState(setting) },
      Setting("MAX_TARGET_SPEED", "Remote Control: Max Speed") { setting -> SettingIntegerState(setting) },
      Setting("MAX_STEER_OFFSET", "Remote Control: Max. Steer Offset") { setting -> SettingDoubleState(setting) },
      Setting("ROLL_PID_OUTPUT_LIMIT", "ROLL PID: Limit") { setting -> SettingDoubleState(setting) },
      Setting("ROLL_PID_OUTPUT_ACC", "ROLL PID: Acceleration") { setting -> SettingDoubleState(setting) },
      Setting("SPEED_PID_OUTPUT_LIMIT", "SPEED PID: Limit") { setting -> SettingDoubleState(setting) },
      Setting("SPEED_PID_OUTPUT_ACC", "SPEED PID: Acceleration") { setting -> SettingDoubleState(setting) },
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
  val clazz: Class<Enum<*>>,
  value: String? = null,
  val enumValue: Enum<*>? = null,
  initialized: Boolean = false
) : SettingState<String>(setting, value, initialized)
{
  override fun withValue(value: String) : SettingEnumValue {
    val enumValue = clazz.enumConstants!!.first { it.name == value }
    return SettingEnumValue(setting, clazz = clazz, value = enumValue.name, enumValue, initialized = true)
  }

}


