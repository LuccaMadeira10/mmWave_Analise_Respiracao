#include <Arduino.h>
#include "Seeed_Arduino_mmWave.h"

#ifdef ESP32
  #include <HardwareSerial.h>
  HardwareSerial mmwaveSerial(0);   // caso sua case use outra UART, mude para (1) e set RX/TX
#else
  #define mmwaveSerial Serial1
#endif

/****** configurações de serial ******/
const uint32_t BAUD = 115200;          // pode mudar para 300 se quiser MUITO lento

/****** instancia ******/
SEEED_MR60FDA2 mmWave;

/****** parametros ajustados para bancada ******/
uint32_t sensitivity = 20;  // mais sensível para teste de mesa
float height = 3.0;         // altura do sensor ao PISO (m) — mesa ~0.75–0.90
float threshold = 0.9;      // limiar um pouco menor p/ facilitar detecção

/****** params lidos do radar ******/
float rect_XL, rect_XR, rect_ZF, rect_ZB;

/****** estados ******/
typedef enum {
  EXIST_PEOPLE,
  NO_PEOPLE,
  PEOPLE_FALL,
} MMWAVE_STATUS;

MMWAVE_STATUS status = NO_PEOPLE, last_status = NO_PEOPLE;

/****** filtro de estabilidade (frames consecutivos) ******/
int human_cnt = 0, nohuman_cnt = 0;
const int N_FRAMES = 3;  // ~3*100ms = 300ms para firmar estado

/****** controle de velocidade de impressão ******/
const uint32_t RAW_INTERVAL_MS = 1200; // <<< intervalo p/ imprimir "raw:" (1,2 s)
uint32_t last_raw_ms = 0;

void setup() {
  Serial.begin(115200);

  // Se precisar usar outra UART no ESP32, faça:
  // HardwareSerial mmwaveSerial(1);
  // mmwaveSerial.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
  // mmWave.begin(&mmwaveSerial);
  mmWave.begin(&mmwaveSerial);

  mmWave.setUserLog(0);

  if (mmWave.setInstallationHeight(height)) {
    Serial.printf("setInstallationHeight success: %.2f\n", height);
  } else {
    Serial.println("setInstallationHeight failed");
  }

  if (mmWave.setThreshold(threshold)) {
    Serial.printf("setThreshold success: %.2f\n", threshold);
  } else {
    Serial.println("setThreshold failed");
  }

  if (mmWave.setSensitivity(sensitivity)) {
    Serial.printf("setSensitivity success %u\n", sensitivity);
  } else {
    Serial.println("setSensitivity failed");
  }

  if (mmWave.getRadarParameters(height, threshold, sensitivity,
                                rect_XL, rect_XR, rect_ZF, rect_ZB)) {
    Serial.printf("height: %.2f\tthreshold: %.2f\tsensitivity: %u\n",
                  height, threshold, sensitivity);
    Serial.printf("rect_XL: %.2f\trect_XR: %.2f\trect_ZF: %.2f\trect_ZB: %.2f\n",
                  rect_XL, rect_XR, rect_ZF, rect_ZB);
  } else {
    Serial.println("getRadarParameters failed");
  }

  Serial.println("\n--- Bench test: presence + fall (with hysteresis) ---");
}

void loop() {
  if (mmWave.update(100)) {  // processa frame (~100 ms timeout)
    bool is_human = false, is_fall = false;

    // leituras “seguras” (se falhar, fica false)
    if (!mmWave.getHuman(is_human)) is_human = false;
    if (!mmWave.getFall(is_fall))   is_fall  = false;

    // imprime "raw:" devagar (a cada RAW_INTERVAL_MS)
    uint32_t now = millis();
    if (now - last_raw_ms >= RAW_INTERVAL_MS) {
      Serial.printf("raw: human=%d fall=%d hc=%d nhc=%d\n",
                    is_human ? 1 : 0, is_fall ? 1 : 0, human_cnt, nohuman_cnt);
      last_raw_ms = now;
    }

    // filtro de estabilidade + estados
    if (is_fall) {
      status = PEOPLE_FALL;
      human_cnt = 0;
      nohuman_cnt = 0;
    } else if (is_human) {
      human_cnt++;
      nohuman_cnt = 0;
      if (human_cnt >= N_FRAMES) status = EXIST_PEOPLE;
    } else {
      nohuman_cnt++;
      human_cnt = 0;
      if (nohuman_cnt >= N_FRAMES) status = NO_PEOPLE;
    }
  }

  // prints amigáveis (idênticos aos seus), só quando muda de estado
  if (status != last_status) {
    switch (status) {
      case NO_PEOPLE:    Serial.println("Waiting for people"); break;
      case EXIST_PEOPLE: Serial.println("PEOPLE !!!");         break;
      case PEOPLE_FALL:  Serial.println("FALL !!!");           break;
    }
    last_status = status;
  }
}
