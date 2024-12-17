package com.dnillg.balancer.controlapp.chart

import kotlin.math.sin

class SinusGenerator (
  val samplesPerSecond : Int,
  private var nextSample: Long = 0,
  val amplitude: Double = 100.0,
  val frequency: Double = 0.2,
  val consumer: (Float) -> Unit
) {
  // Pre-compute to avoid repeated calculation in the loop
  private val radiansPerSample = 2 * Math.PI * frequency / samplesPerSecond

  fun generate(samplesToGenerate: Int) {
    for (i in 0 until samplesToGenerate) {
      val radians = nextSample * radiansPerSample
      val value = (sin(radians) * amplitude).toFloat()
      consumer(value)
      nextSample++
    }
  }
}