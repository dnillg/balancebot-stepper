package com.dnillg.balancer.controlapp

import android.Manifest
import android.annotation.SuppressLint
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.content.Context
import android.content.pm.PackageManager
import android.content.res.Configuration
import android.os.Bundle
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.systemBarsPadding
import androidx.compose.foundation.layout.width
import androidx.compose.material3.Button
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.toArgb
import androidx.compose.ui.platform.LocalConfiguration
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.viewinterop.AndroidView
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import com.dnillg.balancer.controlapp.bluetooth.BtConnection
import com.dnillg.balancer.controlapp.sensor.ChartEntryWindow
import com.dnillg.balancer.controlapp.serial.SerialWorker
import com.dnillg.balancer.controlapp.serial.SerialWorkerFactory
import com.dnillg.balancer.controlapp.serial.model.DiagDataSerialUnit
import com.dnillg.balancer.controlapp.ui.theme.ControlAppTheme
import com.github.mikephil.charting.charts.LineChart
import com.github.mikephil.charting.components.Legend
import com.github.mikephil.charting.components.XAxis
import com.github.mikephil.charting.data.Entry
import com.github.mikephil.charting.data.LineData
import com.github.mikephil.charting.data.LineDataSet
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import javax.inject.Inject

class MainActivity @Inject constructor() : ComponentActivity() {

  @Inject
  lateinit var serialWorkerFactory: SerialWorkerFactory

  private var serialWorker: SerialWorker? = null;
  private var btConnection: BtConnection? = null;
  private val chartChartEntryWindow = ChartEntryWindow(seconds = 5.0).apply {
    generateSin("sensor1", 200,0.0)
    generateSin("sensor2", 200, 0.5)
  }

  private var lineChart: LineChart? = null;

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    DaggerAppComponent.create().inject(this)
    enableEdgeToEdge()
    setContent {
      ControlAppTheme {
        Box(
          modifier = Modifier
            .fillMaxSize()
            .systemBarsPadding() // Prevent content from overlapping system UI
        ) {
          DefaultLayout()
        }
      }
    }
  }

  @Composable
  fun DefaultLayout() {
    OrientationBasedLayout()
  }


  @Composable
  @Preview(showBackground = true)
  fun OrientationBasedLayout() {
    val configuration = LocalConfiguration.current
    when (configuration.orientation) {
      Configuration.ORIENTATION_LANDSCAPE -> {
        LandscapeLayout()
      }
    }
  }

  @Composable
  fun LandscapeLayout() {
    Row(
      modifier = Modifier
        .fillMaxSize()
        .padding(16.dp),
      horizontalArrangement = Arrangement.spacedBy(8.dp),
      verticalAlignment = Alignment.CenterVertically
    ) {
      Box(
        modifier = Modifier
          .weight(1f)
          .fillMaxHeight(1.0f)
          .background(Color.Red)
      ) {
        StyledLineChart()
      }
      Column(
        modifier = Modifier
          .width(240.dp)
          .fillMaxHeight()
          .background(Color.Blue),
        verticalArrangement = Arrangement.spacedBy(8.dp),
        horizontalAlignment = Alignment.CenterHorizontally
      ) {
        Button(onClick = { CoroutineScope(Dispatchers.Main).launch { connectBluetooth() } }) {
          Text("Button 1")
        }
        Button(onClick = { /* TODO */ }) {
          Text("Button 2")
        }
        Button(onClick = {
          CoroutineScope(Dispatchers.Main).launch {
            connectBluetooth()
            if (btConnection != null) {
              serialWorker = serialWorkerFactory.create(btConnection!!)
              serialWorker!!.subscribe(DiagDataSerialUnit::class.java) {
                Log.i("SERIAL", "Received: $it")
              }
              serialWorker!!.run();
            }
          }
        }) {
          Text("Button 3")
        }
      }
    }
  }

  @Composable
  private fun StyledLineChart() {
    val context = LocalContext.current
    this.lineChart = remember {
      LineChart(context).apply {
        setBackgroundColor(Color.Black.toArgb()) // Background color
        axisLeft.apply {
          axisLineColor = Color.White.toArgb()
          textColor = Color.White.toArgb()
        }
        axisRight.isEnabled = false
        xAxis.apply {
          position = XAxis.XAxisPosition.BOTTOM // Bottom X-axis
          textColor = Color.White.toArgb()
        }
        legend.apply {
          form = Legend.LegendForm.LINE
          textColor = Color.White.toArgb()
        }
      }
    }

    val dataSet = LineDataSet(chartChartEntryWindow.getPoints("sensor1"), "Sample Data").apply {
      color = Color.Red.toArgb()
      //valueTextColor = Color.Blue.toArgb()
      lineWidth = 1f
      setDrawValues(false)
      setDrawCircles(false)
    }

    val dataSet2 = LineDataSet(chartChartEntryWindow.getPoints("sensor2"), "Sample Data2").apply {
      color = Color.Blue.toArgb()
      //valueTextColor = Color.Blue.toArgb()
      lineWidth = 1f
      setDrawValues(false)
      setDrawCircles(false)
    }

    val lineData = LineData()
    lineData.addDataSet(dataSet)
    lineData.addDataSet(dataSet2)


    lineChart!!.data = lineData
    AndroidView(
      factory = { lineChart!! },
      modifier = Modifier.fillMaxSize()
    )
  }

  @SuppressLint("MissingPermission")
  fun connectBluetooth() {
    if (btConnection != null) {
      try {
        btConnection!!.close();
      } catch (e: Throwable) {
        Log.e("BT", "Could not close BT connection", e)
      }
      btConnection = null;
    }

    if (!hasPermissions()) {
      Log.i("BL", "Request permission");
      requestPermissions();
    }
    if (hasPermissions()) {
      val bluetoothManager = this.getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager
      val bluetoothAdapter: BluetoothAdapter = bluetoothManager.adapter
      val pairedDevices: Set<BluetoothDevice>? = bluetoothAdapter.bondedDevices
      pairedDevices?.forEach { device ->
        if (device.name.contains("balance")) {
          btConnection = BtConnection(device)
        } else {
          Log.i("BL", "Device found: " + device.name)
        }
      }
    } else {
      Log.i("BL", "No permission");
    }
    Log.i("BL", "Connected.")
  }

  private fun hasPermissions(): Boolean {
    return ContextCompat.checkSelfPermission(
      this,
      Manifest.permission.BLUETOOTH_CONNECT
    ) == PackageManager.PERMISSION_GRANTED
  }

  private fun requestPermissions() {
    ActivityCompat.requestPermissions(
      this,
      arrayOf(
        Manifest.permission.BLUETOOTH_CONNECT
      ),
      BLUETOOTH_PERMISSION_REQUEST_CODE
    )
  }

  companion object {
    private const val BLUETOOTH_PERMISSION_REQUEST_CODE = 1001;
  }

}
