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

interface SettingValue <T> {
  val value: T

  fun adjust(amount: Double)
}

class SettingBooleanValue(
  override var value: Boolean,
) : SettingValue<Boolean>
{
  override fun adjust(amount: Double) {
    value = if (amount > 0) true else false
  }
}

class SettingIntValue(
  override var value: Int,
) : SettingValue<Int>
{
  override fun adjust(amount: Double) {
    value = (value + amount).toInt()
  }
}

class SettingDoubleValue(
  override var value: Double,
) : SettingValue<Double>
{
  override fun adjust(amount: Double) {
    value += amount
  }
}

class SettingEnumValue<T : Enum<T>>(
  value: T,
  private val enumClass: Class<T>,
) : SettingValue<T> {
  override var value: T = value
    private set

  override fun adjust(amount: Double) {
    val enumSize = enumClass.enumConstants!!.size;
    val offset = if (amount > 0) 1 else if (amount < 0) -1 else 0
    val newOrdinal = (value.ordinal + offset) % enumSize
    value = enumClass.enumConstants!![newOrdinal];
  }
}