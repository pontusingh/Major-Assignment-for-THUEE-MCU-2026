## Network (HTTP)

WiFi connection and HTTP status code Q&A must be completed. For HTTP, you only need to complete **one** of the following three: GET, POST, or MQTT.

### WiFi Part

- The pet can automatically connect to a mobile phone's 2.4 GHz hotspot, and the connection can be seen on the phone. Optionally, you may output the network status on a display device such as an OLED or TFT screen (not mandatory).
- The pet can automatically reconnect after the hotspot is turned off and then back on, and the reconnection can be seen on the phone. Optionally, you may output disconnection and reconnection status on a display device (also not mandatory).

### HTTP Part

- Students must answer the meanings of different HTTP status codes and the corresponding troubleshooting methods.
- The pet can obtain data (e.g., weather, time, etc.) from any service provider (you may use the examples given in the lecture notes or find your own) via HTTP GET, and output the data using a display device.
- The pet can send its own status to any service provider (same requirement as above) via HTTP POST.
- Must properly handle exceptions that occur during GET and POST requests to prevent the program from freezing.

### MQTT Part

- The pet can be controlled via MQTT software on a computer to perform any action (students may decide what actions).
- The pet can send its status to the MQTT software on the computer.
- You may choose any software; MQTTX client is recommended. You may use a public broker or set up your own broker.

---

## Low Power and Flash

### I. Technologies Used

#### 1.1 Deep‑sleep Standby
Normal Active operation (loop() cycling); triggers Deep‑sleep after prolonged inactivity or under low‑light conditions.

**Wake‑up and Recovery:**
- After wake‑up, execution restarts from `setup()`, and the wake‑up source is determined via `esp_sleep_get_wakeup_cause()`
- Before entering Deep‑sleep, call `saveToFlash()` to save the full state; after wake‑up, call `loadFromFlash()` to restore
- Standby wake‑up vs. sleep wake‑up: before entering Deep‑sleep, write a flag to `RTC_DATA_ATTR wasSleeping`; after wake‑up, read it to distinguish (standby wake‑up allows interaction)

#### 1.2 RTC Memory
`RTC_DATA_ATTR` variables retain data during sleep. Three core variables:

| Variable | Purpose |
|----------|---------|
| bootMs | Cumulative running time |
| lastInteract | Timestamp of the last interaction (used for the 30‑second standby timeout) |
| wasSleeping | Mode flag before sleep (distinguishes standby wake‑up from sleep wake‑up) |

#### 1.3 Preferences Flash Memory
Two namespaces:

| Namespace | Purpose | Write Timing |
|-----------|---------|--------------|
| pet_cfg | Mood, feeding status, action state | Before entering Deep‑sleep (power‑off protection) |
| pet_mem | Cumulative counts of pats, feeds, plays | When the corresponding interaction occurs |

**Memory influences behaviour** (you may design the specific rules yourself):
- The number of pats affects mood upper limit, pat cooldown time, and the probability of automatic action triggering
- Read from Flash into RAM cache once at power‑on to avoid frequent Flash reads during runtime

### II. Runtime Behaviour

#### Normal Mode
Three‑line OLED display: `EMOTION:x` / `HUNGRY` or `FULL xm xs` / action name
- Mood decreases by 1 every 30 seconds (no decay when "Full")
- 3.33% probability per second of automatic action
- Serial output every 5 seconds: `[xxxx ms] EMOTION:x HUNGRY/FULL ACTION | Head:x Feed:x`

---

## Elective Reference Design (continued)

**Pat on the head:** Click the button → mood +1 (upper limit affected by pat history) → triggers "Play" → `head_count++` written to Flash → serial output `Head Touch`

- Base cooldown 10 seconds; more pat history can shorten it as part of memory influence

**Feed:** Encoder rotated one full turn → enters "Full" state (120‑second countdown) → `feed_count++` written to Flash → serial output `Feed`

**Standby:** No operation for 30 seconds → Deep‑sleep → wake up via BOOT button → restore from Flash → continue running

**Sleep:** Low light detected for 5 seconds → OLED cleared, servos move to +80° → wake‑up by button only → upon wake‑up, servos do not return to neutral, and no action is triggered