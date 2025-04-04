package com.dnillg.balancer.controlapp.domain.chart

import androidx.compose.ui.graphics.Color


enum class TimeSeriesType(
  val label: String,
  val alias: String,
  val color: Color,
) {
  NOOP("Off", "roll", Color.Red),
  ROLL("Roll", "roll", Color.Red),
  TARGET_ROLL("Target roll", "target-roll", Color.Green),
  SPEED("Speed", "speed", Color.Blue),
  TARGET_SPEED("Target speed", "target-speed", Color.Yellow),
  MOTOR_LEFT_SPEED("Motor left speed", "motor-left-speed", Color.Magenta),
  MOTOR_RIGHT_SPEED("Motor right speed", "motor-right-speed", Color.Cyan),
  MOTOR_SCALED_ROLL_ERROR("Motor Scaled Roll Error", "motor-scaled-roll-error", Color.Red),
}

data class ChartConfiguration(
  val title: String,
  val minimumValue: Float,
  val maximumValue: Float,
  val activeSeries: List<TimeSeriesType>,
  val alias: String
  )

val offChartConfiguration = ChartConfiguration(
  title = "Off",
  minimumValue = -10000f,
  maximumValue = 10000f,
  activeSeries = listOf(TimeSeriesType.NOOP),
  alias = "OFF"
)

val rollChartConfiguration = ChartConfiguration(
  title = "Roll",
  minimumValue = -90f,
  maximumValue = 90f,
  activeSeries = listOf(TimeSeriesType.ROLL, TimeSeriesType.TARGET_ROLL),
  alias = "ROLL"
)

val speedChartConfiguration = ChartConfiguration(
  title = "Speed",
  minimumValue = -10000f,
  maximumValue = 10000f,
  activeSeries = listOf(TimeSeriesType.SPEED, TimeSeriesType.TARGET_SPEED),
  alias = "SPEED"
)

val rollErrorMotorsChartConfiguration = ChartConfiguration(
  title = "Roll Error & Motors",
  minimumValue = -10000f,
  maximumValue = 10000f,
  activeSeries = listOf(TimeSeriesType.MOTOR_SCALED_ROLL_ERROR, TimeSeriesType.MOTOR_RIGHT_SPEED, TimeSeriesType.MOTOR_LEFT_SPEED),
  alias = "ROLL_ERROR_MOTORS"
)

val chartConfigurations: List<ChartConfiguration> = listOf(
  offChartConfiguration,
  rollChartConfiguration,
  speedChartConfiguration,
  rollErrorMotorsChartConfiguration
)