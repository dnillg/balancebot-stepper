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
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
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
import com.dnillg.balancer.controlapp.bt.BtConnection
import com.dnillg.balancer.controlapp.serial.model.ControlSerialUnit
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
import java.util.LinkedList
import javax.inject.Inject
import kotlin.math.sin

class MainActivity @Inject constructor() : ComponentActivity() {

    @Inject
    lateinit var serialUnitThreadFactory: SerialUnitThreadFactory

    private var serialUnitThread: SerialUnitThread? = null;
    private var btConnection: BtConnection? = null;
    private var readerThread: Thread? = null;
    private var lineChart: LineChart? = null;
    private var data: LinkedList<Entry> = LinkedList<Entry>().let {
      it.add(Entry(0f, 180.0f));
      it.add(Entry(1f, -180.0f));
      it;
    };

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
            //BtDataCollector()
          }
        }
      }
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

    @Composable
    fun DefaultLayout() {
      OrientationBasedLayout()
    }

    private fun reader(consumer: (Float) -> Unit) {
//      for (i in 0..10000000000) {
//        if (btConnection == null) {
//          Thread.sleep(100);
//          continue;
//        }
//        val conn = btConnection!!;
//        if (conn.isAlive()) {
//          val line: String = conn.readLine();
//          Log.i("INPUT", line);
//          if (line.startsWith("TLM>")) {
//            val parts = line.substring(4).split(",");
//            if (parts.size > 1) {
//              try {
//                consumer.invoke(parts[0].toFloat());
//              } catch (e: Throwable) {
//                Log.e("BT", "Error parsing number", e);
//              }
//            }
//          }
//        }
//      }
    }


    @Composable
    @Preview(showBackground = true)
    fun OrientationBasedLayout() {
      val configuration = LocalConfiguration.current
      LaunchedEffect(Unit) {
        readerThread = Thread {
          reader(
            {
              runOnUiThread {
                //androidx.compose.runtime.snapshots.Snapshot.withMutableSnapshot {
                data.removeLast()
                data.add(Entry(1f, it))
                //}
                lineChart?.invalidate()
              }
            }
          )
        }
        readerThread!!.start()
      }

      when (configuration.orientation) {
        Configuration.ORIENTATION_PORTRAIT -> {
          PortraitLayout()
        }

        Configuration.ORIENTATION_LANDSCAPE -> {
          LandscapeLayout()
        }
      }
    }

    @Composable
    fun PortraitLayout() {
      Column(
        modifier = Modifier
          .fillMaxSize()
          .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(8.dp),
        horizontalAlignment = Alignment.CenterHorizontally
      ) {
        Text("Portrait Mode", style = MaterialTheme.typography.headlineMedium)
        Button(onClick = {
          CoroutineScope(Dispatchers.Main).launch {
            connectBluetooth()
            if (btConnection != null) {
              serialUnitThread = serialUnitThreadFactory.create(btConnection!!)
              serialUnitThread!!.subscribe(DiagDataSerialUnit::class.java) {
                Log.i("SERIAL", "Received: $it")
              }
              serialUnitThread!!.run();
            }
          }
        }) {
          Text("Button 1")
        }
        Button(onClick = {
          serialUnitThread!!.send(ControlSerialUnit(0.3f, 0.2f));
        }) {
          Text("Button 2")
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
      val dataSet = LineDataSet(data, "Sample Data").apply {
        color = Color.Red.toArgb()
        //valueTextColor = Color.Blue.toArgb()
        lineWidth = 2f
        setDrawValues(false)
        setDrawCircles(false)
      }
      val lineData = LineData(dataSet)
      lineChart!!.data = lineData
      AndroidView(
        factory = { lineChart!! },
        modifier = Modifier.fillMaxSize()
      )
    }

    @Composable
    fun VerticalLayout() {
      Column(
        modifier = Modifier
          .fillMaxSize()
          .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(8.dp), // Space between items
        horizontalAlignment = Alignment.CenterHorizontally // Align items horizontally
      ) {
        Text("Item 1", style = MaterialTheme.typography.headlineSmall)
        Text("Item 2", style = MaterialTheme.typography.headlineSmall)
        Text("Item 3", style = MaterialTheme.typography.headlineSmall)
      }
    }

    fun generateSineWave(
      samples: Int,
      amplitude: Float = 1f,
      frequency: Float = 1f,
      phaseShift: Float = 0f // Phase shift in radians
    ): List<Float> {
      return List(samples) { index ->
        val x = 2 * 3.1414 * frequency * index / samples + phaseShift // Add phase shift
        (amplitude * sin(x)).toFloat()
      }
    }

    companion object {
      private const val BLUETOOTH_PERMISSION_REQUEST_CODE = 1001;
    }

  }
