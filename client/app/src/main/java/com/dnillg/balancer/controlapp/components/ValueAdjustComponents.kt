package com.dnillg.balancer.controlapp.components

import android.util.Log
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.dnillg.balancer.controlapp.domain.model.SettingEnumValue
import com.dnillg.balancer.controlapp.domain.model.SettingState

@Composable
fun AdjustNumericValueRow(
  enabled: Boolean,
  value: Double,
  incFunc: (Double) -> Unit,
  inc1: Double = 0.01,
  inc2: Double = 0.1,
) {
  Row(
    verticalAlignment = Alignment.CenterVertically,
    horizontalArrangement = Arrangement.spacedBy(8.dp)
  ) {
    if (inc2 != 0.0) {
      AdjustValueButton("--", Color.Black, {
        incFunc.invoke(-inc2)
      }, enabled = enabled)
    }
    AdjustValueButton("-", Color.Black, {
      incFunc.invoke(-inc1)
    }, enabled = enabled)
    NumericValueBox(number = value)
    AdjustValueButton("+", Color.Black, {
      incFunc.invoke(inc1)
    }, enabled = enabled)
    if (inc2 != 0.0) {
      AdjustValueButton("++", Color.Black, {
        incFunc.invoke(inc2)
      }, enabled = enabled)
    }
  }
}

@Composable
fun AdjustIntegerValueRow(
  enabled: Boolean,
  value: Int,
  incFunc: (Int) -> Unit,
  inc1: Int = 1,
  inc2 : Int = 0,
) {
  Row(
    verticalAlignment = Alignment.CenterVertically,
    horizontalArrangement = Arrangement.spacedBy(8.dp)
  ) {
    if (inc2 != 0) {
      AdjustValueButton("--", Color.Black, {
        incFunc.invoke(-inc2)
      }, enabled = enabled)
    }
    AdjustValueButton("-", Color.Black, {
      incFunc.invoke(-inc1)
    }, enabled = enabled)
    IntegerValueBox(value)
    AdjustValueButton("+", Color.Black, {
      incFunc.invoke(inc1)
    }, enabled = enabled)
    if (inc2 != 0) {
      AdjustValueButton("++", Color.Black, {
        incFunc.invoke(inc2)
      }, enabled = enabled)
    }
  }
}

@Composable
fun AdjustEnumValueRow(
  enabled: Boolean,
  value: Enum<*>,
  setValue: (Enum<*>) -> Unit,
) {
  val ordinal = value.ordinal
  val values = value.javaClass.enumConstants!!
  Row(
    verticalAlignment = Alignment.CenterVertically,
    horizontalArrangement = Arrangement.spacedBy(8.dp)
  ) {
    AdjustValueButton("-", Color.Black, {
      setValue(values[(ordinal - 1 + values.size) % values.size])
    }, enabled = enabled)
    ValueBox(text = value.name)
    AdjustValueButton("+", Color.Black, {
      setValue(values[(ordinal + 1) % values.size])
    }, enabled = enabled)
  }
}

@Composable
fun AdjustValueButton(
  text: String,
  textColor: Color,
  onClick: () -> Unit,
  enabled: Boolean = true
) {
  val bgColor = if (enabled) Color.LightGray else Color.Gray
  val effectiveOnClick: () -> Unit =
    if (enabled) onClick else { -> Log.d("AdjustValueButton", "Disabled") }

  Button(
    onClick = effectiveOnClick,
    colors = ButtonDefaults.buttonColors(containerColor = bgColor),
    shape = RoundedCornerShape(8.dp),
    contentPadding = PaddingValues(0.dp),
    modifier = Modifier
      .height(36.dp)
      .width(36.dp)
  ) {
    Text(
      text = text,
      fontSize = 24.sp,
      color = textColor,
      fontWeight = FontWeight.Bold
    )
  }
}