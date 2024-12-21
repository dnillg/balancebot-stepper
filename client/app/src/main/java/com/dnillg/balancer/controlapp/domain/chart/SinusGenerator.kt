package com.dnillg.balancer.controlapp.domain.chart

import kotlin.math.sin

class SinusGenerator (
  val samplesPerSecond : Int,
  private var nextSample: Long = 0,
  val amplitude: Double = 100.0,
  val frequency: Double = 0.2,
) {
  // Pre-compute to avoid repeated calculation in the loop
  private val radiansPerSample = 2 * Math.PI * frequency / samplesPerSecond

  fun generate(samplesToGenerate: Int, consumer: (Float) -> Unit) {
    for (i in 0 until samplesToGenerate) {
      consumer(next())
      nextSample++
    }
  }

  fun next(): Float {
    val radians = nextSample * radiansPerSample
    val value = (sin(radians) * amplitude).toFloat()
    nextSample++
    return value
  }
}