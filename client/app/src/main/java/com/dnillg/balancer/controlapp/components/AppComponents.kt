package com.dnillg.balancer.controlapp.components

import android.util.Log
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.painter.Painter
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

private val doNothing = {}

@Composable
fun LargeButton(
  text: String,
  buttonColor: Color,
  textColor: Color,
  enabled: Boolean = true,
  onClick: () -> Unit,
) {
  Button(
    onClick = (if (enabled) onClick else doNothing),
    colors = ButtonDefaults.buttonColors(containerColor = (if (enabled) buttonColor else Color.Gray)),
    shape = RoundedCornerShape(8.dp),
    modifier = Modifier
      .height(48.dp)
  ) {
    Text(
      text = text,
      fontSize = 16.sp,
      color = (if (enabled) textColor else Color.Black),
      fontWeight = FontWeight.Bold
    )
  }
}

@Composable
fun NumericValueBox(number: Double) {
  val format = if (number < 1) "%.10f" else if (number < 10) "%.6f" else if (number < 1000) "%.4f" else "%.2f"
  val formattedNumber = String.format(format, number)
  ValueBox(modifier = Modifier.width(240.dp), text = formattedNumber)
}

@Composable
fun IntegerValueBox(number: Int) {
  ValueBox(modifier = Modifier.width(240.dp), text = number.toString())
}

@Composable
fun ValueBox(modifier: Modifier = Modifier, text: String) {
  Box(
    modifier = modifier
      .background(
        color = Color.LightGray,
        shape = RoundedCornerShape(8.dp)
      ) // Rounded background
      .padding(6.dp),
    contentAlignment = Alignment.Center
  ) {
    Text(
      text = text,
      fontSize = 18.sp,
      color = Color.Black,
      fontWeight = FontWeight.Bold,
      textAlign = TextAlign.Center
    )
  }
}

@Composable
fun SimpleButton(
  onClick: () -> Unit,
  imageVector: ImageVector,
  toggled: Boolean = false,
) {
  IconButton(
    onClick = onClick,
    modifier = Modifier
      .background(if (toggled) Color.White else Color.LightGray, shape = CircleShape)
      .size(42.dp)
      .padding(4.dp)
  ) {
    Icon(
      imageVector = imageVector,
      contentDescription = "Button",
      modifier = Modifier.size(32.dp)
    )
  }
}

@Composable
fun GenericButton(action: () -> Unit, text: String, backgroundColor: Color) {

}

@Composable
fun SimpleImageButton(
  onClick: () -> Unit,
  painter: Painter
) {
  Box(
    contentAlignment = Alignment.Center,
    modifier = Modifier
      .size(42.dp)
      .background(Color.Transparent, shape = CircleShape)
  ) {
    IconButton(onClick = onClick) {
      Icon(
        painter = painter,
        contentDescription = "Button",
        modifier = Modifier.size(42.dp).fillMaxSize(),
        tint = Color.Unspecified, // Ensures the image is displayed correctly
      )
    }
  }
}