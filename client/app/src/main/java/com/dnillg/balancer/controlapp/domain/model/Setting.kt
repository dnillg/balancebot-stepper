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