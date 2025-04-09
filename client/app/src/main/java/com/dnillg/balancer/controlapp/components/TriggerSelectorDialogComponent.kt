package com.dnillg.balancer.controlapp.components

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.unit.dp
import androidx.compose.ui.window.Dialog
import com.dnillg.balancer.controlapp.R
import com.dnillg.balancer.controlapp.serial.model.TriggerSerialUnit
import com.dnillg.balancer.controlapp.serial.model.TriggerType


@Composable
fun TriggerSelectorDialog(onDismiss: () -> Unit, sendUnit: (TriggerSerialUnit) -> Unit) {
  Dialog(onDismissRequest = onDismiss) {
    Box(
      modifier = Modifier
        .background(Color.White, shape = RoundedCornerShape(16.dp))
        .padding(16.dp)
        .fillMaxWidth()
    ) {
      Column(
        verticalArrangement = Arrangement.spacedBy(16.dp),
        horizontalAlignment = Alignment.CenterHorizontally
      ) {
        Row(
          verticalAlignment = Alignment.CenterVertically,
          horizontalArrangement = Arrangement.spacedBy(16.dp)
        ) {
          SimpleImageButton(
            { onDismiss(); sendUnit(TriggerSerialUnit(TriggerType.SLAV_CAT)) },
            painterResource(R.drawable.cccp)
          )
          SimpleImageButton(
            { onDismiss(); sendUnit(TriggerSerialUnit(TriggerType.ZLAD)) },
            painterResource(R.drawable.zlad)
          )
          SimpleImageButton(
            { onDismiss(); sendUnit(TriggerSerialUnit(TriggerType.CHARLIE)) },
            painterResource(R.drawable.charlie)
          )
          SimpleImageButton(
            { onDismiss(); sendUnit(TriggerSerialUnit(TriggerType.CAT_KISS)) },
            painterResource(R.drawable.catkiss)
          )
          SimpleImageButton(
            { onDismiss(); sendUnit(TriggerSerialUnit(TriggerType.OIIUU_CAT)) },
            painterResource(R.drawable.oiia)
          )

        }
        Row(
          verticalAlignment = Alignment.CenterVertically,
          horizontalArrangement = Arrangement.spacedBy(16.dp)
        ) {
          SimpleImageButton(
            { onDismiss(); sendUnit(TriggerSerialUnit(TriggerType.PEDRO_4X)) },
            painterResource(R.drawable.pedro)
          )
          SimpleImageButton(
            { onDismiss(); sendUnit(TriggerSerialUnit(TriggerType.RICK_ROLL)) },
            painterResource(R.drawable.rickrolld)
          )
          SimpleImageButton(
            { onDismiss(); sendUnit(TriggerSerialUnit(TriggerType.ROBOT_FACE_STANDARD)) },
            painterResource(R.drawable.robot)
          )
          SimpleImageButton(
            { onDismiss(); sendUnit(TriggerSerialUnit(TriggerType.NYAN)) },
            painterResource(R.drawable.nyan)
          )
          SimpleImageButton(
            { onDismiss(); sendUnit(TriggerSerialUnit(TriggerType.MINECRAFT)) },
            painterResource(R.drawable.minecraft)
          )
        }

        Row(
          verticalAlignment = Alignment.CenterVertically,
          horizontalArrangement = Arrangement.spacedBy(16.dp)
        ) {
          SimpleImageButton(
            { onDismiss(); sendUnit(TriggerSerialUnit(TriggerType.TARKOV)) },
            painterResource(R.drawable.tarkov)
          )
          SimpleImageButton(
            { onDismiss(); sendUnit(TriggerSerialUnit(TriggerType.CSIPKES)) },
            painterResource(R.drawable.csipkes)
          )
          SimpleImageButton(
            { onDismiss(); sendUnit(TriggerSerialUnit(TriggerType.TROLOLO)) },
            painterResource(R.drawable.trololo)
          )
          SimpleImageButton(
            { onDismiss(); sendUnit(TriggerSerialUnit(TriggerType.AXA)) },
            painterResource(R.drawable.axa)
          )
          SimpleImageButton(
            { onDismiss(); sendUnit(TriggerSerialUnit(TriggerType.SWEET_BROWN)) },
            painterResource(R.drawable.sweetbrown)
          )
        }

        Row(
          verticalAlignment = Alignment.CenterVertically,
          horizontalArrangement = Arrangement.spacedBy(16.dp)
        ) {
          SimpleImageButton(
            { onDismiss(); sendUnit(TriggerSerialUnit(TriggerType.EMOTION_SAD)) },
            painterResource(R.drawable.robotsad)
          )
          SimpleImageButton(
            { onDismiss(); sendUnit(TriggerSerialUnit(TriggerType.EMOTION_LOVE)) },
            painterResource(R.drawable.robotlove)
          )
        }

      }
    }
  }
}