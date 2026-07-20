#include <WiFi.h>
#include <WebServer.h>
#include <esp_wifi.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include <PacketSerial.h>
#include <DHT.h> 

// Nanopb Generated Headers
#include "pb_encode.h"
#include "pb_decode.h"
#include "robot_state.pb.h"

// --- Configuration ---
#define WIFI_SSID           "MattMatt"
#define WIFI_PASS           "lolmattmatt"
#define WIFI_TIMEOUT_MS     10000u

// --- Pin Allocations ---
#define PIN_L298N_ENA       1
#define PIN_L298N_IN1       2
#define PIN_L298N_IN2       42
#define PIN_L298N_IN3       41
#define PIN_L298N_IN4       40
#define PIN_L298N_ENB       39

#define PIN_DHT22_DATA      18
#define PIN_MQ2_AOUT        4

#define PIN_SONAR_L_TRIG    5
#define PIN_SONAR_L_ECHO    6
#define PIN_SONAR_RR_TRIG   7
#define PIN_SONAR_RR_ECHO   15
#define PIN_SONAR_R_TRIG    16
#define PIN_SONAR_R_ECHO    17

// --- Hardware Settings ---
#define DHTTYPE             DHT22
#define CMD_QUEUE_DEPTH     16
#define DEBUG_STR_LEN       128

// PWM Setup Constants (Updated for Core v3.x API)
#define PWM_FREQ            5000
#define PWM_RES             8    // 8-bit resolution (0-255)

typedef struct {
    int      temp;   
    int      hum;    
    int      rear;   
    int      left;   
    int      right;  
    int      gas;    
    uint32_t ok_count;
    uint32_t err_count;
    char     last_err[DEBUG_STR_LEN];
    char     debug[DEBUG_STR_LEN];
} SensorSnapshot_t;

// --- Globals ---
static SensorSnapshot_t  g_snap  = { 0, 0, 999, 999, 999, 0, 0, 0, "none", "System Running..." };
static SemaphoreHandle_t g_mutex = NULL;
static QueueHandle_t     g_cmdQ  = NULL;

static WebServer server(80);
static PacketSerial piSerial;
static DHT dht(PIN_DHT22_DATA, DHTTYPE);

// Global speed memory (0 to 255 mapping)
static uint8_t g_current_speed_raw = 255; 

// --- HTML Dashboard ---
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
  <title>Robot Command</title>
  <style>
    *{box-sizing:border-box;}
    body{font-family:'Segoe UI',Tahoma,sans-serif;text-align:center;background:#1a1a1a;color:#fff;margin:0;padding:20px;user-select:none;-webkit-user-select:none;}
    h2{margin:0 0 10px;}
    .container{max-width:860px;margin:auto;}
    .slider-container{margin: 20px auto; width: 260px; text-align: left; background:#2d2d2d; padding:15px; border-radius:8px;}
    input[type=range] {width: 100%; margin-top: 10px;}
    .controls{display:grid;grid-template-columns:repeat(3,80px);gap:12px;justify-content:center;margin:20px 0;touch-action:manipulation;}
    button{padding:22px;font-size:22px;font-weight:bold;border-radius:12px;border:none;background:#007bff;color:#fff;cursor:pointer;outline:none;transition:background 0.1s,transform 0.1s;}
    button:active{background:#0056b3;transform:scale(0.93);}
    .empty{background:transparent!important;border:none!important;cursor:default!important;}
    .data-grid{display:flex;flex-wrap:wrap;justify-content:center;gap:10px;margin-top:10px;}
    .data-box{background:#2d2d2d;padding:14px 16px;border-radius:8px;width:30%;min-width:130px;font-size:17px;border-left:4px solid #007bff;text-align:left;}
    .dist-ok    {border-left-color:#00cc66!important;}
    .dist-warn {border-left-color:#ffaa00!important;color:#ffaa00;}
    .dist-crit {border-left-color:#ff4d4d!important;color:#ff4d4d;font-weight:bold;}
    .log-box{width:96%;background:#111;color:#00ff00;font-family:monospace;font-size:13px;border-left:4px solid #00ff00;margin-top:10px;text-align:left;padding:10px;}
    .log-box.warn{border-left:4px solid #ffaa00;color:#ffaa00;}
    .log-box.err {border-left:4px solid #ff4d4d;color:#ff4d4d;}
    .danger{border-left-color:#ff4d4d!important;color:#ff4d4d;font-weight:bold;}
    .status-text{color:#aaa;font-size:13px;margin-top:12px;}
    .settings-panel {display:none; background:#2a2a2a; padding:15px; border-radius:8px; margin-top:20px; border:1px solid #444;}
    table {width:100%; border-collapse:collapse; margin-bottom:10px;}
    th, td {padding:8px; text-align:center; border-bottom:1px solid #444;}
    .map-btn {padding:10px 15px; font-size:14px; background:#28a745;}
    .gear-btn {padding:10px; font-size:16px; background:#444; border-radius:8px; margin-top:10px;}
  </style>
</head>
<body oncontextmenu="return false;">
<div class="container">
  <h2>Robot Command</h2>
  
  <div class="slider-container">
    <label>Speed: <span id="speedVal">100</span>%</label>
    <input type="range" min="0" max="9" value="9" oninput="setSpeed(this.value)">
  </div>

  <div class="controls">
    <div class="empty"></div>
    <button onpointerdown="cmd('F')" onpointerup="cmd('S')">▲</button>
    <div class="empty"></div>
    <button onpointerdown="cmd('L')" onpointerup="cmd('S')">◀</button>
    <button onpointerdown="cmd('B')" onpointerup="cmd('S')">▼</button>
    <button onpointerdown="cmd('R')" onpointerup="cmd('S')">▶</button>
  </div>
  
  <div class="data-grid">
    <div class="data-box">Temp<br><span id="temp">--</span> °C</div>
    <div class="data-box">Humidity<br><span id="hum">--</span> %</div>
    <div class="data-box" id="gas-box"><span id="gas-label">Gas</span><br><span id="gas">--</span></div>
    <div class="data-box" id="dist-left-box">Left<br><span id="dist-left">--</span> cm</div>
    <div class="data-box" id="dist-rear-box">Rear<br><span id="dist-rear">--</span> cm</div>
    <div class="data-box" id="dist-right-box">Right<br><span id="dist-right">--</span> cm</div>
  </div>
  
  <div class="data-box log-box" id="log-box" style="margin-top:12px;">
    <span id="log-status">--</span><br>
    Last Err: <span id="log-lasterr">none</span>
  </div>
  <div class="status-text" id="status">UI Active.</div>

  <button class="gear-btn" onclick="toggleSettings()">⚙️ Motor Mapping</button>
  <div class="settings-panel" id="settings">
    <h3 style="margin-top:0;">Map Logical Directions to Pins</h3>
    <table id="mapTable"></table>
    <button class="map-btn" onclick="saveMapping()">Save Mapping</button>
  </div>
</div>

<script>
  const DIRS = ['F', 'B', 'L', 'R'];
  let mapping = {'F':[1,0,1,0], 'B':[0,1,0,1], 'L':[0,1,1,0], 'R':[1,0,0,1], 'S':[0,0,0,0]};
  try { let saved = localStorage.getItem('motorMap'); if (saved) mapping = JSON.parse(saved); } catch(e) {}

  function renderSettings() {
      let html = `<tr><th>Dir</th><th>IN1</th><th>IN2</th><th>IN3</th><th>IN4</th></tr>`;
      DIRS.forEach(d => {
          html += `<tr><td><b>${d}</b></td>`;
          for(let i=0; i<4; i++) {
              let checked = mapping[d][i] ? 'checked' : '';
              html += `<td><input type="checkbox" id="map_${d}_${i}" ${checked} style="width:22px;height:22px;"></td>`;
          }
          html += `</tr>`;
      });
      document.getElementById('mapTable').innerHTML = html;
  }
  
  function toggleSettings() {
      let s = document.getElementById('settings');
      if(s.style.display === 'none' || s.style.display === '') { renderSettings(); s.style.display = 'block'; } 
      else { s.style.display = 'none'; }
  }
  
  function saveMapping() {
      DIRS.forEach(d => { for(let i=0; i<4; i++) mapping[d][i] = document.getElementById(`map_${d}_${i}`).checked ? 1 : 0; });
      localStorage.setItem('motorMap', JSON.stringify(mapping));
      toggleSettings();
  }

  function setSpeed(val) {
      document.getElementById('speedVal').textContent = Math.round((val/9)*100);
      fetch('/cmd?spd=' + val).catch(() => {});
  }

  let lastCmd = '';
  function cmd(dir) {
    if (dir === lastCmd) return;
    lastCmd = dir;
    let arr = mapping[dir] || [0,0,0,0];
    let val = (arr[0]<<3) | (arr[1]<<2) | (arr[2]<<1) | arr[3];
    let charCode = String.fromCharCode(97 + val); // maps 0-15 to a-p
    fetch('/cmd?raw=' + charCode).catch(() => {});
  }

  function distClass(cm) { return cm>=999 ? '' : (cm<=20 ? 'dist-crit' : (cm<=50 ? 'dist-warn' : 'dist-ok')); }
  function distText(cm) { return cm >= 999 ? 'OOR' : cm; }
  
  setInterval(() => {
    fetch('/data').then(r => r.json()).then(d => {
      document.getElementById('temp').textContent = (d.temp / 10).toFixed(1);
      document.getElementById('hum').textContent  = (d.hum  / 10).toFixed(1);
      document.getElementById('gas').textContent  = d.gas;
      const gasBox = document.getElementById('gas-box');
      if (d.gas > 400) { gasBox.classList.add('danger'); document.getElementById('gas-label').textContent = 'GAS LEAK'; } 
      else { gasBox.classList.remove('danger'); document.getElementById('gas-label').textContent = 'Gas'; }
      
      ['left','rear','right'].forEach(side => {
        const val = d['dist_' + side];
        document.getElementById('dist-' + side + '-box').className = 'data-box ' + distClass(val);
        document.getElementById('dist-' + side).textContent = distText(val);
      });
      
      const logBox = document.getElementById('log-box');
      const okRate = (d.ok+d.err) > 0 ? Math.round(d.ok*100/(d.ok+d.err)) : 0;
      document.getElementById('log-status').textContent  = d.debug + ' | OK:' + d.ok + ' ERR:' + d.err + ' (' + okRate + '%)';
      document.getElementById('log-lasterr').textContent = d.last_err;
      logBox.className = (d.err > 0 && okRate < 80) ? 'data-box log-box err' : (d.err > 0 ? 'data-box log-box warn' : 'data-box log-box');
      document.getElementById('status').textContent = 'Last sync: ' + new Date().toLocaleTimeString();
    }).catch(() => { document.getElementById('status').textContent = 'Connection lost — retrying...'; });
  }, 250);
</script>
</body></html>
)rawliteral";

// --- Helper Functions ---
static String json_escape(const char *s) {
    String out; out.reserve(strlen(s) + 8);
    for (; *s; s++) {
        if (*s == '"') out += "\\\""; else if (*s == '\\') out += "\\\\"; else out += *s;
    }
    return out;
}

static void write_motor_pins(char code) {
    if (code < 'a' || code > 'p') return;
    uint8_t val = code - 'a'; 
    
    uint8_t in1 = (val >> 3) & 0x01;
    uint8_t in2 = (val >> 2) & 0x01;
    uint8_t in3 = (val >> 1) & 0x01;
    uint8_t in4 = val & 0x01;

    digitalWrite(PIN_L298N_IN1, in1);
    digitalWrite(PIN_L298N_IN2, in2);
    digitalWrite(PIN_L298N_IN3, in3);
    digitalWrite(PIN_L298N_IN4, in4);

    // Updated for Core v3.x API: ledcWrite uses the direct pin number now
    ledcWrite(PIN_L298N_ENA, g_current_speed_raw);
    ledcWrite(PIN_L298N_ENB, g_current_speed_raw);
}

// --- Web Handlers ---
static void handleRoot() {
    server.sendHeader("Connection", "close");
    server.send_P(200, "text/html", INDEX_HTML);
}

static void handleCmd() {
    server.sendHeader("Connection", "close");
    
    if (server.hasArg("raw")) {
        char c = server.arg("raw")[0];
        if (c >= 'a' && c <= 'p') xQueueSend(g_cmdQ, &c, 0);
    }
    
    if (server.hasArg("spd")) {
        char s = server.arg("spd")[0];
        if (s >= '0' && s <= '9') xQueueSend(g_cmdQ, &s, 0);
    }
    
    server.send(200, "text/plain", "OK");
}

static void handleData() {
    server.sendHeader("Connection", "close");
    SensorSnapshot_t snap;
    if (xSemaphoreTake(g_mutex, pdMS_TO_TICKS(20)) != pdTRUE) { 
        server.send(503, "text/plain", "busy"); 
        return; 
    }
    snap = g_snap; 
    xSemaphoreGive(g_mutex);
    
    String json = "{\"temp\":" + String(snap.temp) + ",\"hum\":" + String(snap.hum) + ",\"dist_rear\":" + String(snap.rear) +
                  ",\"dist_left\":" + String(snap.left) + ",\"dist_right\":" + String(snap.right) + ",\"gas\":" + String(snap.gas) +
                  ",\"ok\":" + String(snap.ok_count) + ",\"err\":" + String(snap.err_count) + 
                  ",\"debug\":\"" + json_escape(snap.debug) + "\",\"last_err\":\"" + json_escape(snap.last_err) + "\"}";
    server.send(200, "application/json", json);
}

// --- TASK: Protobuf callback (Pi to ESP32) ---
void onPacketReceived(const uint8_t* buffer, size_t size) {
    MotorCommand cmd = MotorCommand_init_zero;
    pb_istream_t stream = pb_istream_from_buffer(buffer, size);
    
    if (pb_decode(&stream, MotorCommand_fields, &cmd)) {
        char pic_cmd = 'a'; 
        
        if (cmd.linear_vel > 0.1) pic_cmd = 'k';      
        else if (cmd.linear_vel < -0.1) pic_cmd = 'f'; 
        else if (cmd.angular_vel > 0.2) pic_cmd = 'g';  
        else if (cmd.angular_vel < -0.2) pic_cmd = 'j'; 

        xQueueSend(g_cmdQ, &pic_cmd, 0);
    }
}

// --- TASK: Protobuf Comms (ESP32 to Pi) ---
static void vTaskPiComm(void *pvParameters) {
    uint8_t tx_buffer[128];
    SensorSnapshot shared_sensors = SensorSnapshot_init_zero;

    for (;;) {
        piSerial.update(); 

        if (xSemaphoreTake(g_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
            shared_sensors.temp = g_snap.temp / 10.0f;
            shared_sensors.hum = g_snap.hum / 10.0f;
            shared_sensors.dist_rear = g_snap.rear;
            shared_sensors.dist_left = g_snap.left;
            shared_sensors.dist_right = g_snap.right;
            shared_sensors.gas = g_snap.gas;
            xSemaphoreGive(g_mutex);

            pb_ostream_t stream = pb_ostream_from_buffer(tx_buffer, sizeof(tx_buffer));
            if (pb_encode(&stream, SensorSnapshot_fields, &shared_sensors)) {
                piSerial.send(tx_buffer, stream.bytes_written);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100)); 
    }
}

// --- TASK: Local Sensor Acquisition Engine ---
static int read_sonar_cm(uint8_t trig, uint8_t echo) {
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
    
    long duration = pulseIn(echo, HIGH, 25000); 
    if (duration == 0) return 999; 
    return duration * 0.0343 / 2;
}

static void vTaskSensorReader(void *pv) {
    dht.begin();
    TickType_t xLastWakeTime = xTaskGetTickCount();
    uint32_t environmental_counter = 0;

    for (;;) {
        int l_dist  = read_sonar_cm(PIN_SONAR_L_TRIG, PIN_SONAR_L_ECHO);
        vTaskDelay(pdMS_TO_TICKS(15));
        int rr_dist = read_sonar_cm(PIN_SONAR_RR_TRIG, PIN_SONAR_RR_ECHO);
        vTaskDelay(pdMS_TO_TICKS(15));
        int r_dist  = read_sonar_cm(PIN_SONAR_R_TRIG, PIN_SONAR_R_ECHO);
        
        float t_val = NAN;
        float h_val = NAN;
        int gas_val = 0;
        
        if (environmental_counter % 4 == 0) {
            t_val = dht.readTemperature();
            h_val = dht.readHumidity();
            gas_val = analogRead(PIN_MQ2_AOUT);
        }
        environmental_counter++;

        if (xSemaphoreTake(g_mutex, pdMS_TO_TICKS(20)) == pdTRUE) {
            g_snap.left  = l_dist;
            g_snap.rear  = rr_dist;
            g_snap.right = r_dist;
            
            if (!isnan(t_val)) g_snap.temp = (int)(t_val * 10.0f);
            if (!isnan(h_val)) g_snap.hum  = (int)(h_val * 10.0f);
            if (environmental_counter % 4 == 1) g_snap.gas = gas_val;
            
            g_snap.ok_count++;
            snprintf(g_snap.debug, DEBUG_STR_LEN, "L:%d Rr:%d R:%d G:%d", g_snap.left, g_snap.rear, g_snap.right, g_snap.gas);
            xSemaphoreGive(g_mutex);
        } else {
            if (xSemaphoreTake(g_mutex, pdMS_TO_TICKS(5)) == pdTRUE) {
                g_snap.err_count++;
                strncpy(g_snap.last_err, "Mutex timeout on data capture writing", DEBUG_STR_LEN);
                xSemaphoreGive(g_mutex);
            }
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(500)); 
    }
}

// --- TASK: WebServer execution loop ---
static void vTaskWebServer(void *pv) {
    for (;;) { 
        server.handleClient(); 
        vTaskDelay(pdMS_TO_TICKS(2)); 
    }
}

// --- TASK: Direct Motor Command Processor ---
static void vTaskMotorCmd(void *pv) {
    char c;
    for (;;) {
        if (xQueueReceive(g_cmdQ, &c, portMAX_DELAY) == pdTRUE) {
            if (c >= '0' && c <= '9') {
                g_current_speed_raw = (uint8_t)((c - '0') * 28.33f);
                // Updated for Core v3.x API: passing the direct pins
                ledcWrite(PIN_L298N_ENA, g_current_speed_raw);
                ledcWrite(PIN_L298N_ENB, g_current_speed_raw);
            } 
            else if (c >= 'a' && c <= 'p') {
                write_motor_pins(c);
            }
        }
    }
}

// --- TASK: WiFi Keep-Alive ---
static void vTaskWiFiGuard(void *pv) {
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(5000));
        if (WiFi.status() != WL_CONNECTED) {
            WiFi.disconnect(); 
            vTaskDelay(pdMS_TO_TICKS(100)); 
            WiFi.begin(WIFI_SSID, WIFI_PASS);
            uint32_t start = millis();
            while (WiFi.status() != WL_CONNECTED && (millis() - start) < WIFI_TIMEOUT_MS) {
                vTaskDelay(pdMS_TO_TICKS(500));
            }
        }
    }
}

void setup() {
    Serial.begin(115200);
    piSerial.setStream(&Serial);
    piSerial.setPacketHandler(&onPacketReceived);

    pinMode(PIN_L298N_IN1, OUTPUT);
    pinMode(PIN_L298N_IN2, OUTPUT);
    pinMode(PIN_L298N_IN3, OUTPUT);
    pinMode(PIN_L298N_IN4, OUTPUT);

    pinMode(PIN_SONAR_L_TRIG, OUTPUT);
    pinMode(PIN_SONAR_L_ECHO, INPUT);
    pinMode(PIN_SONAR_RR_TRIG, OUTPUT);
    pinMode(PIN_SONAR_RR_ECHO, INPUT);
    pinMode(PIN_SONAR_R_TRIG, OUTPUT);
    pinMode(PIN_SONAR_R_ECHO, INPUT);

    // New Core v3.x API: Single initialization command per PWM pin.
    // ledcAttach(pin, frequency, resolution_bits) automatically configures everything under the hood.
    ledcAttach(PIN_L298N_ENA, PWM_FREQ, PWM_RES);
    ledcAttach(PIN_L298N_ENB, PWM_FREQ, PWM_RES);

    // Default configuration: Safe Stop
    write_motor_pins('a');

    g_mutex = xSemaphoreCreateMutex();
    g_cmdQ  = xQueueCreate(CMD_QUEUE_DEPTH, sizeof(char));
    
    WiFi.mode(WIFI_STA);
    WiFi.setTxPower(WIFI_POWER_8_5dBm);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    
    server.on("/", handleRoot);
    server.on("/cmd", handleCmd);
    server.on("/data", handleData);
    server.begin();
    
    xTaskCreatePinnedToCore(vTaskPiComm,       "PiComm",       4096, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(vTaskSensorReader,  "SensorReader",  4096, NULL, 3, NULL, 0); 
    xTaskCreatePinnedToCore(vTaskMotorCmd,     "MotorCmd",     2048, NULL, 4, NULL, 0); 
    xTaskCreatePinnedToCore(vTaskWebServer,    "WebServer",    8192, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(vTaskWiFiGuard,    "WiFiGuard",    3072, NULL, 1, NULL, 1);
}

void loop() { 
    vTaskDelete(NULL); 
}