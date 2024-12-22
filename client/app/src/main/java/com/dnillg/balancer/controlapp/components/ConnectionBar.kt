package com.dnillg.balancer.controlapp.components

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.MutableState
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.dnillg.balancer.controlapp.domain.model.ConnectionStatus
import com.dnillg.balancer.controlapp.domain.model.ConnectionStatusEnum

@Composable
fun ConnectionBar(mutableConnectionStatus: MutableState<ConnectionStatus>) {
  val statusObj = mutableConnectionStatus.value
  val bgColor = when(statusObj.status) {
    ConnectionStatusEnum.CONNECTED -> Color(0.0f, 0.66f, 0.0f, 1.0f)
    ConnectionStatusEnum.NOT_CONNECTED -> Color.DarkGray
    ConnectionStatusEnum.UNHEALTHY -> Color(0.66f, 0.0f, 0.0f, 1.0f)
  }
  Box(
    modifier = Modifier
      .background(
        color = bgColor,
        shape = RoundedCornerShape(8.dp)
      )
      .fillMaxWidth()
      .padding(0.dp),
    contentAlignment = Alignment.Center,
  ) {
    Text(
      text = statusObj.status.displayName,
      fontSize = 12.sp,
      color = Color.White,
      fontWeight = FontWeight.Medium,
      textAlign = TextAlign.Center
    )
  }

}