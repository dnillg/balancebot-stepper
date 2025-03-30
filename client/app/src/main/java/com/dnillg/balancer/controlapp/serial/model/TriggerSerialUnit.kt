package com.dnillg.balancer.controlapp.serial.model

enum class TriggerType {
  SLAV_CAT,
  CHARLIE,
  CAT_KISS,
  PEDRO_1X,
  PEDRO_2X,
  PEDRO_4X,
  RICK_ROLL,
  ROBOT_FACE_STANDARD,
  OIIUU_CAT,
  ZLAD,
  COCONUT,
  NYAN,
  TROLOLO,
  MINECRAFT,
  CSIPKES,
  TARKOV
}

class TriggerSerialUnit (
  val triggerType: TriggerType,
  val userData: String = ""
) : SerialUnit