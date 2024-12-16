package com.dnillg.balancer.controlapp.serial.model

enum class TriggerType {
  HORN,
  FEELING_LOVE,
  FEELING_FREEZING,
  FEELING_SAD,
  FEELING_ANGRY,
  FEELING_HAPPY,
  BEER
}

class TriggerSerialUnit (
  val triggerType: TriggerType
) : SerialUnit