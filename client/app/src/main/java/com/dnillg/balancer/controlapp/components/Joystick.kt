package com.dnillg.balancer.controlapp.components

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.gestures.detectDragGestures
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.size
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.drawscope.DrawScope
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.platform.LocalDensity
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.dp
import kotlin.math.atan2
import kotlin.math.cos
import kotlin.math.sin
import kotlin.math.sqrt

@Composable
inline fun Joystick(
  modifier: Modifier = Modifier,
  baseRadius: Dp = 60.dp,
  thumbRadius: Dp = 20.dp,
  crossinline onMove: (xPercent: Float, yPercent: Float) -> Unit = { _, _ -> }
) {
  var thumbPosition by remember { mutableStateOf(Offset.Zero) }

  val baseRadiusPx = with(LocalDensity.current) { baseRadius.toPx() }
  val thumbRadiusPx = with(LocalDensity.current) { thumbRadius.toPx() }

  Box(
    modifier = modifier
      .size(baseRadius * 2)
      .pointerInput(Unit) {
        detectDragGestures(
          onDrag = { change, dragAmount ->
            val newPosition = thumbPosition + Offset(dragAmount.x, dragAmount.y)
            val distance = sqrt(newPosition.x * newPosition.x + newPosition.y * newPosition.y)
            if (distance <= baseRadiusPx) {
              thumbPosition = newPosition
            } else {
              val angle = atan2(newPosition.y, newPosition.x)
              thumbPosition = Offset(
                x = cos(angle) * baseRadiusPx,
                y = sin(angle) * baseRadiusPx
              )
            }
            val xPercent = thumbPosition.x / baseRadiusPx
            val yPercent = thumbPosition.y / baseRadiusPx
            onMove(xPercent, yPercent)
          },
          onDragEnd = {
            // Reset thumb to the center when the user releases the drag
            thumbPosition = Offset.Zero
            onMove(0f, 0f) // Send "centered" state to the callback
          }
        )
      }
  ) {
    Canvas(modifier = Modifier.fillMaxSize()) {
      drawJoystick(baseRadiusPx, thumbRadiusPx, thumbPosition)
    }
  }
}

fun DrawScope.drawJoystick(baseRadiusPx: Float, thumbRadiusPx: Float, thumbPosition: Offset) {
  drawCircle(
    color = Color.LightGray,
    radius = baseRadiusPx,
    center = Offset(size.width / 2, size.height / 2)
  )
  drawCircle(
    color = Color.DarkGray,
    radius = thumbRadiusPx,
    center = Offset(
      x = size.width / 2 + thumbPosition.x,
      y = size.height / 2 + thumbPosition.y
    )
  )
}