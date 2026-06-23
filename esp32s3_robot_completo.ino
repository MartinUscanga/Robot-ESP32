/**
 * ============================================================================
 * ESP32-S3 Robot Asistente con Audio Bidireccional + Pantalla ST7789
 * ============================================================================
 * 
 * Hardware:
 * - ESP32-S3
 * - Micrófono INMP441 (I2S)
 * - Altavoz MAX98357A (I2S DAC)
 * - Pantalla ST7789 240x320 (SPI)
 * 
 * Autor: Robot ESP32 Project
 * Versión: 2.0
 * ============================================================================
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <driver/i2s.h>
#include "esp_task_wdt.h"

// ============================================================================
// CONFIGURACIÓN DE PINES
// ============================================================================

// --- Pantalla ST7789 (SPI) ---
#define TFT_CS    10
#define TFT_DC    11
#define TFT_RST   12
#define TFT_MOSI  13
#define TFT_SCK   14

// --- Audio I2S ---
#define I2S_NUM_TX    (i2s_port_t)0  // Puerto I2S para Altavoz
#define I2S_NUM_RX    (i2s_port_t)1  // Puerto I2S para Micrófono

// Pines Altavoz MAX98357A (I2S TX)
#define I2S_SPEAKER_BCLK  16
#define I2S_SPEAKER_LRCK  17
#define I2S_SPEAKER_DOUT  15

// Pines Micrófono INMP441 (I2S RX)
#define I2S_MIC_BCLK      4
#define I2S_MIC_LRCK      5
#define I2S_MIC_DIN       6

// ============================================================================
// CONFIGURACIÓN RED Y API
// ============================================================================



#define WIFI_SSID         "Tenda_88ADC0"
#define WIFI_PASSWORD     "924113587"
#define API_URL           "http://18.221.246.87:8000"
#define DEVICE_ID         "esp32s3-display"

// ============================================================================
// CONFIGURACIÓN DE AUDIO
// ============================================================================

#define SAMPLE_RATE       16000          // Hz (16kHz para compatibilidad con API)
#define BITS_PER_SAMPLE   16             // Bits
#define RECORD_TIME       5              // Segundos de grabación
#define BUFFER_SIZE       512            // Tamaño del buffer I2S
#define RECORD_BUFFER_SIZE (SAMPLE_RATE * RECORD_TIME * sizeof(int16_t))

// ============================================================================
// CONFIGURACIÓN DE PANTALLA
// ============================================================================

#define SCREEN_WIDTH      240
#define SCREEN_HEIGHT     320
#define CENTER_X          (SCREEN_WIDTH / 2)
#define CENTER_Y          (SCREEN_HEIGHT / 2)

// Colores
#define COLOR_FONDO       0x001F    // Azul oscuro
#define COLOR_CARA        0xFFFF    // Blanco
#define COLOR_OJO         0x0000    // Negro
#define COLOR_BOCA        0xF800    // Rojo
#define COLOR_MEJILLA     0xFBEA    // Rosa claro
#define COLOR_TEXTO       0xFFFF    // Blanco
#define COLOR_VERDE       0x07E0    // Verde
#define COLOR_AMARILLO    0xFFE0    // Amarillo

// ============================================================================
// OBJETOS GLOBALES
// ============================================================================

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// ============================================================================
// ENUMERACIONES
// ============================================================================

enum Estado {
  INICIALIZANDO,
  CONECTANDO_WIFI,
  ESPERANDO_COMANDO,
  GRABANDO_AUDIO,
  ENVIANDO_API,
  REPRODUCIENDO_RESPUESTA,
  ERROR_SISTEMA
};

enum Emocion {
  FELIZ,
  TRISTE,
  PENSANDO,

  SORPRENDIDO,
  ENOJADO,
  HABLANDO
};

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================

Estado estadoActual = INICIALIZANDO;
Emocion emocionActual = FELIZ;

uint8_t* audioBuffer = nullptr;
size_t audioBufferSize = 0;

unsigned long ultimoFrame = 0;
int animacionFrame = 0;
bool botonPresionado = false;

// ============================================================================
// FUNCIONES DE INICIALIZACIÓN
// ============================================================================

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n");
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║  ESP32-S3 Robot Asistente v2.0        ║");
  Serial.println("║  Con Audio Bidireccional + Display    ║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.println();
  
  // Desactivar watchdog durante inicialización
  esp_task_wdt_init(30, false);
  
  // 1. Inicializar Display
  if (!inicializarDisplay()) {
    Serial.println("❌ Error crítico: No se pudo inicializar display");
    estadoActual = ERROR_SISTEMA;
    while(1) { delay(1000); }
  }
  
  // 2. Inicializar I2S para micrófono
  if (!inicializarMicrofono()) {
    Serial.println("❌ Error crítico: No se pudo inicializar micrófono");
    mostrarError("Error Micrófono");

    estadoActual = ERROR_SISTEMA;
    while(1) { delay(1000); }
  }
  
  // 3. Inicializar I2S para altavoz
  if (!inicializarAltavoz()) {
    Serial.println("❌ Error crítico: No se pudo inicializar altavoz");
    mostrarError("Error Altavoz");
    estadoActual = ERROR_SISTEMA;
    while(1) { delay(1000); }
  }
  
  // 4. Conectar WiFi
  conectarWiFi();
  
  // 5. Reservar memoria para buffer de audio
  audioBuffer = (uint8_t*)ps_malloc(RECORD_BUFFER_SIZE);
  if (!audioBuffer) {
    Serial.println("❌ Error: No hay memoria suficiente para buffer de audio");
    mostrarError("Sin Memoria");
    estadoActual = ERROR_SISTEMA;
    while(1) { delay(1000); }
  }
  
  Serial.println("\n✅ Sistema iniciado correctamente");
  Serial.println("📝 Presiona ENTER para iniciar grabación");
  Serial.printf("💾 RAM libre: %d bytes\n", ESP.getFreeHeap());
  Serial.printf("💾 PSRAM libre: %d bytes\n", ESP.getFreePsram());
  
  estadoActual = ESPERANDO_COMANDO;
  dibujarCaraKawaii(FELIZ);
  mostrarTextoEstado("¡Listo! Presiona ENTER", COLOR_VERDE);
}

// ============================================================================
// LOOP PRINCIPAL
// ============================================================================

void loop() {
  // Leer comandos desde Serial
  if (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {

      if (estadoActual == ESPERANDO_COMANDO) {
        iniciarGrabacion();
      }
    }
  }
  
  // Animar cara cada 100ms
  unsigned long ahora = millis();
  if (ahora - ultimoFrame > 100) {
    ultimoFrame = ahora;
    animacionFrame++;
    
    if (estadoActual == ESPERANDO_COMANDO || estadoActual == REPRODUCIENDO_RESPUESTA) {
      animarCara();
    }
  }
  
  delay(10);
}

// ============================================================================
// INICIALIZACIÓN DE DISPLAY
// ============================================================================

bool inicializarDisplay() {
  Serial.println("🖥️  Inicializando Display ST7789...");
  
  try {
    // Configurar SPI manualmente
    SPI.begin(TFT_SCK, -1, TFT_MOSI, TFT_CS);
    SPI.setFrequency(40000000); // 40MHz
    
    // Inicializar display
    tft.init(SCREEN_WIDTH, SCREEN_HEIGHT, SPI_MODE2);
    delay(100);
    
    tft.setRotation(0); // Ajustar según orientación deseada
    tft.fillScreen(COLOR_FONDO);
    delay(50);
    
    // Mostrar pantalla de inicio
    dibujarPantallaInicio();
    
    Serial.println("   ✓ Display inicializado correctamente");
    return true;
    
  } catch (...) {
    return false;
  }
}


// ============================================================================
// INICIALIZACIÓN DE MICRÓFONO I2S (INMP441)
// ============================================================================

bool inicializarMicrofono() {
  Serial.println("🎤 Inicializando Micrófono INMP441 (I2S RX)...");
  
  i2s_config_t i2s_config_rx = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = BUFFER_SIZE,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };
  
  i2s_pin_config_t pin_config_rx = {
    .bck_io_num = I2S_MIC_BCLK,
    .ws_io_num = I2S_MIC_LRCK,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_MIC_DIN
  };
  
  esp_err_t err = i2s_driver_install(I2S_NUM_RX, &i2s_config_rx, 0, NULL);
  if (err != ESP_OK) {
    Serial.printf("   ✗ Error instalando driver I2S RX: %d\n", err);
    return false;
  }
  
  err = i2s_set_pin(I2S_NUM_RX, &pin_config_rx);
  if (err != ESP_OK) {
    Serial.printf("   ✗ Error configurando pines I2S RX: %d\n", err);
    return false;
  }
  
  // Limpiar buffer DMA
  i2s_zero_dma_buffer(I2S_NUM_RX);
  
  Serial.println("   ✓ Micrófono inicializado correctamente");
  Serial.printf("   • Sample Rate: %d Hz\n", SAMPLE_RATE);
  Serial.printf("   • Bits: %d\n", BITS_PER_SAMPLE);
  Serial.printf("   • Pines: BCLK=%d, LRCK=%d, DIN=%d\n", 
                I2S_MIC_BCLK, I2S_MIC_LRCK, I2S_MIC_DIN);
  
  return true;
}


// ============================================================================
// INICIALIZACIÓN DE ALTAVOZ I2S (MAX98357A)
// ============================================================================

bool inicializarAltavoz() {
  Serial.println("🔊 Inicializando Altavoz MAX98357A (I2S TX)...");
  
  i2s_config_t i2s_config_tx = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = BUFFER_SIZE,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };
  
  i2s_pin_config_t pin_config_tx = {
    .bck_io_num = I2S_SPEAKER_BCLK,
    .ws_io_num = I2S_SPEAKER_LRCK,
    .data_out_num = I2S_SPEAKER_DOUT,
    .data_in_num = I2S_PIN_NO_CHANGE
  };
  
  esp_err_t err = i2s_driver_install(I2S_NUM_TX, &i2s_config_tx, 0, NULL);
  if (err != ESP_OK) {
    Serial.printf("   ✗ Error instalando driver I2S TX: %d\n", err);
    return false;
  }
  
  err = i2s_set_pin(I2S_NUM_TX, &pin_config_tx);
  if (err != ESP_OK) {
    Serial.printf("   ✗ Error configurando pines I2S TX: %d\n", err);
    return false;
  }
  
  // Limpiar buffer DMA
  i2s_zero_dma_buffer(I2S_NUM_TX);
  
  Serial.println("   ✓ Altavoz inicializado correctamente");
  Serial.printf("   • Sample Rate: %d Hz\n", SAMPLE_RATE);
  Serial.printf("   • Bits: %d\n", BITS_PER_SAMPLE);
  Serial.printf("   • Pines: BCLK=%d, LRCK=%d, DOUT=%d\n", 
                I2S_SPEAKER_BCLK, I2S_SPEAKER_LRCK, I2S_SPEAKER_DOUT);
  
  return true;
}


// ============================================================================
// CONECTAR WIFI
// ============================================================================

void conectarWiFi() {
  Serial.println("📡 Conectando a WiFi...");
  Serial.printf("   • SSID: %s\n", WIFI_SSID);
  
  estadoActual = CONECTANDO_WIFI;
  dibujarCaraKawaii(PENSANDO);
  mostrarTextoEstado("Conectando WiFi...", COLOR_AMARILLO);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 30) {
    delay(500);
    Serial.print(".");
    intentos++;
    
    if (intentos % 3 == 0) {
      dibujarCargando(intentos / 3);
    }
  }
  
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("   ✓ WiFi conectado!");
    Serial.print("   • IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("   • RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    
    dibujarCaraKawaii(FELIZ);
    mostrarTextoEstado("¡WiFi conectado!", COLOR_VERDE);
    delay(1500);
  } else {
    Serial.println("   ✗ Error: No se pudo conectar a WiFi");
    mostrarError("Error WiFi");
    estadoActual = ERROR_SISTEMA;
  }
}

// ============================================================================
// FUNCIONES DE DIBUJO - PANTALLA ST7789
// ============================================================================

void dibujarPantallaInicio() {
  tft.fillScreen(COLOR_FONDO);
  
  tft.setTextColor(COLOR_TEXTO);
  tft.setTextSize(3);
  tft.setCursor(30, 100);
  tft.println("Robot");
  
  tft.setCursor(20, 140);
  tft.println("Asistente");
  
  tft.setTextSize(1);
  tft.setCursor(50, 200);
  tft.println("ESP32-S3 + Gemini");
  
  tft.setTextSize(2);
  tft.setCursor(80, 240);
  tft.println("v2.0");
}



void dibujarCaraKawaii(Emocion emocion) {
  // Limpiar zona central (cara)
  tft.fillRect(0, 60, SCREEN_WIDTH, SCREEN_HEIGHT - 120, COLOR_FONDO);
  
  int caraY = CENTER_Y;
  
  // Ojos
  int ojoIzqX = CENTER_X - 40;
  int ojoDerX = CENTER_X + 40;
  int ojoY = caraY - 20;
  
  // Mejillas rosadas
  tft.fillCircle(CENTER_X - 70, caraY + 15, 15, COLOR_MEJILLA);
  tft.fillCircle(CENTER_X + 70, caraY + 15, 15, COLOR_MEJILLA);
  
  switch (emocion) {
    case FELIZ:
      // Ojos cerrados felices (líneas curvas)
      tft.fillRect(ojoIzqX - 18, ojoY, 36, 6, COLOR_OJO);
      tft.fillRect(ojoDerX - 18, ojoY, 36, 6, COLOR_OJO);
      
      // Boca sonriente grande
      tft.fillCircle(CENTER_X, caraY + 35, 30, COLOR_BOCA);
      tft.fillCircle(CENTER_X, caraY + 22, 30, COLOR_FONDO);
      break;
      
    case TRISTE:
      // Ojos grandes y tristes
      tft.fillCircle(ojoIzqX, ojoY, 12, COLOR_OJO);
      tft.fillCircle(ojoDerX, ojoY, 12, COLOR_OJO);
      tft.fillCircle(ojoIzqX, ojoY + 6, 4, COLOR_CARA);
      tft.fillCircle(ojoDerX, ojoY + 6, 4, COLOR_CARA);
      
      // Lágrimas
      for (int i = 0; i < 3; i++) {
        tft.fillCircle(ojoIzqX - 5, ojoY + 15 + i * 8, 3, 0x1B9F);
        tft.fillCircle(ojoDerX + 5, ojoY + 15 + i * 8, 3, 0x1B9F);
      }
      
      // Boca triste invertida
      tft.fillCircle(CENTER_X, caraY + 45, 28, COLOR_BOCA);
      tft.fillCircle(CENTER_X, caraY + 55, 28, COLOR_FONDO);
      break;
      
    case PENSANDO:
      // Ojos mirando hacia arriba derecha
      tft.fillCircle(ojoIzqX + 5, ojoY - 5, 12, COLOR_OJO);
      tft.fillCircle(ojoDerX + 5, ojoY - 5, 12, COLOR_OJO);
      tft.fillCircle(ojoIzqX + 5, ojoY - 8, 4, COLOR_CARA);
      tft.fillCircle(ojoDerX + 5, ojoY - 8, 4, COLOR_CARA);
      
      // Boca pequeña "hmm"
      tft.fillCircle(CENTER_X, caraY + 30, 10, COLOR_OJO);
      
      // Nubes de pensamiento
      tft.fillCircle(CENTER_X + 60, caraY - 40, 8, COLOR_CARA);
      tft.fillCircle(CENTER_X + 50, caraY - 30, 5, COLOR_CARA);
      tft.fillCircle(CENTER_X + 40, caraY - 20, 3, COLOR_CARA);
      break;
      
    case SORPRENDIDO:
      // Ojos muy abiertos
      tft.fillCircle(ojoIzqX, ojoY, 18, COLOR_OJO);
      tft.fillCircle(ojoDerX, ojoY, 18, COLOR_OJO);
      tft.fillCircle(ojoIzqX, ojoY, 6, COLOR_CARA);
      tft.fillCircle(ojoDerX, ojoY, 6, COLOR_CARA);
      
      // Boca "O" grande
      tft.fillCircle(CENTER_X, caraY + 35, 18, COLOR_OJO);
      tft.fillCircle(CENTER_X, caraY + 35, 12, COLOR_FONDO);
      break;

      
    case ENOJADO:
      // Cejas fruncidas en ángulo
      tft.fillTriangle(ojoIzqX - 20, ojoY - 15, ojoIzqX + 20, ojoY - 20, 
                       ojoIzqX + 20, ojoY - 10, COLOR_OJO);
      tft.fillTriangle(ojoDerX - 20, ojoY - 20, ojoDerX + 20, ojoY - 15, 
                       ojoDerX - 20, ojoY - 10, COLOR_OJO);
      
      // Ojos entrecerrados
      tft.fillRect(ojoIzqX - 12, ojoY, 24, 10, COLOR_OJO);
      tft.fillRect(ojoDerX - 12, ojoY, 24, 10, COLOR_OJO);
      
      // Boca enojada
      tft.fillRect(CENTER_X - 25, caraY + 35, 50, 5, COLOR_BOCA);
      break;
      
    case HABLANDO:
      // Ojos normales
      tft.fillCircle(ojoIzqX, ojoY, 12, COLOR_OJO);
      tft.fillCircle(ojoDerX, ojoY, 12, COLOR_OJO);
      tft.fillCircle(ojoIzqX, ojoY, 4, COLOR_CARA);
      tft.fillCircle(ojoDerX, ojoY, 4, COLOR_CARA);
      
      // Boca abierta (animada)
      if (animacionFrame % 2 == 0) {
        tft.fillCircle(CENTER_X, caraY + 30, 15, COLOR_BOCA);
        tft.fillCircle(CENTER_X, caraY + 30, 10, COLOR_OJO);
      } else {
        tft.fillCircle(CENTER_X, caraY + 30, 12, COLOR_BOCA);
        tft.fillCircle(CENTER_X, caraY + 30, 8, COLOR_OJO);
      }
      break;
  }
  
  emocionActual = emocion;
}

void animarCara() {
  // Parpadeo ocasional
  if (animacionFrame % 40 == 0 && 
      (emocionActual == FELIZ || emocionActual == ESPERANDO_COMANDO)) {
    int caraY = CENTER_Y;
    int ojoIzqX = CENTER_X - 40;
    int ojoDerX = CENTER_X + 40;
    int ojoY = caraY - 20;
    
    // Parpadeo
    tft.fillRect(ojoIzqX - 20, ojoY - 10, 40, 20, COLOR_FONDO);
    tft.fillRect(ojoDerX - 20, ojoY - 10, 40, 20, COLOR_FONDO);
    delay(100);
    dibujarCaraKawaii(emocionActual);
  }
  
  // Animación de boca al hablar
  if (emocionActual == HABLANDO) {
    dibujarCaraKawaii(HABLANDO);
  }
}

void mostrarTextoEstado(String texto, uint16_t color) {
  // Barra inferior
  tft.fillRect(0, SCREEN_HEIGHT - 40, SCREEN_WIDTH, 40, COLOR_FONDO);
  
  tft.setTextColor(color);
  tft.setTextSize(2);
  tft.setCursor(10, SCREEN_HEIGHT - 30);
  tft.println(texto.substring(0, 18)); // Max ~18 caracteres
}



void mostrarRespuestaTexto(String texto) {
  // Área de texto (encima de barra inferior)
  tft.fillRect(0, SCREEN_HEIGHT - 100, SCREEN_WIDTH, 60, COLOR_FONDO);
  
  tft.setTextColor(COLOR_TEXTO);
  tft.setTextSize(1);
  tft.setTextWrap(true);
  
  int y = SCREEN_HEIGHT - 95;
  int maxChars = 38; // Caracteres por línea aproximadamente
  
  for (int i = 0; i < texto.length() && y < SCREEN_HEIGHT - 45; i += maxChars) {
    String linea = texto.substring(i, min((int)(i + maxChars), (int)texto.length()));
    tft.setCursor(5, y);
    tft.println(linea);
    y += 12;
  }
}

void dibujarCargando(int dots) {
  tft.fillRect(0, SCREEN_HEIGHT - 60, SCREEN_WIDTH, 20, COLOR_FONDO);
  tft.setTextSize(2);
  tft.setTextColor(COLOR_AMARILLO);
  tft.setCursor(60, SCREEN_HEIGHT - 55);
  
  String puntitos = "";
  for (int i = 0; i < (dots % 4); i++) {
    puntitos += ".";
  }
  tft.print("Cargando" + puntitos + "   ");
}

void mostrarBarraGrabacion(int progreso) {
  // Barra de progreso de grabación
  int barraX = 20;
  int barraY = SCREEN_HEIGHT - 50;
  int barraAncho = SCREEN_WIDTH - 40;
  int barraAlto = 20;
  
  // Fondo de barra
  tft.drawRect(barraX, barraY, barraAncho, barraAlto, COLOR_TEXTO);
  
  // Progreso (0-100)
  int anchoProgreso = (barraAncho - 4) * progreso / 100;
  tft.fillRect(barraX + 2, barraY + 2, anchoProgreso, barraAlto - 4, COLOR_BOCA);
  
  // Texto de progreso
  tft.setTextColor(COLOR_TEXTO);
  tft.setTextSize(1);
  tft.setCursor(barraX + barraAncho / 2 - 15, barraY - 15);
  tft.print("Grabando: ");
  tft.print(progreso);
  tft.print("%");
}

void mostrarError(String mensaje) {
  tft.fillScreen(0xF800); // Rojo
  
  tft.setTextColor(COLOR_TEXTO);
  tft.setTextSize(2);
  tft.setCursor(30, CENTER_Y - 10);
  tft.println("ERROR");
  
  tft.setTextSize(1);
  tft.setCursor(20, CENTER_Y + 20);
  tft.println(mensaje);
}


// ============================================================================
// CAPTURA DE AUDIO DESDE MICRÓFONO
// ============================================================================

void iniciarGrabacion() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ Error: WiFi desconectado");
    mostrarError("Sin WiFi");
    delay(2000);
    estadoActual = ESPERANDO_COMANDO;
    dibujarCaraKawaii(TRISTE);
    mostrarTextoEstado("Reconectando...", COLOR_AMARILLO);
    conectarWiFi();
    return;
  }
  
  Serial.println("\n🎙️  Iniciando grabación...");
  estadoActual = GRABANDO_AUDIO;
  
  dibujarCaraKawaii(SORPRENDIDO);
  mostrarTextoEstado("¡Escuchando!", COLOR_BOCA);
  
  // Limpiar buffer DMA antes de grabar
  i2s_zero_dma_buffer(I2S_NUM_RX);
  delay(100);
  
  // Grabar audio
  size_t bytesGrabados = grabarAudio(audioBuffer, RECORD_BUFFER_SIZE);
  
  if (bytesGrabados > 0) {
    Serial.printf("   ✓ Grabados %d bytes (%d muestras)\n", 
                  bytesGrabados, bytesGrabados / sizeof(int16_t));
    
    audioBufferSize = bytesGrabados;
    
    // Enviar a la API
    enviarAudioAPI();
    
  } else {
    Serial.println("   ✗ Error: No se grabó audio");
    mostrarError("Error Grabación");
    delay(2000);
    estadoActual = ESPERANDO_COMANDO;
    dibujarCaraKawaii(FELIZ);
    mostrarTextoEstado("Intenta de nuevo", COLOR_AMARILLO);
  }
}

size_t grabarAudio(uint8_t* buffer, size_t bufferSize) {
  size_t bytesLeidos = 0;
  size_t totalBytes = 0;
  
  int16_t* samples = (int16_t*)buffer;
  size_t totalSamples = bufferSize / sizeof(int16_t);
  
  unsigned long inicioGrabacion = millis();
  int ultimoPorcentaje = 0;
  
  Serial.println("   📊 Capturando audio...");
  
  while (totalBytes < bufferSize) {
    // Leer datos del I2S
    esp_err_t result = i2s_read(I2S_NUM_RX, 
                                buffer + totalBytes, 
                                min((size_t)BUFFER_SIZE * 2, bufferSize - totalBytes),
                                &bytesLeidos, 
                                portMAX_DELAY);
    
    if (result == ESP_OK && bytesLeidos > 0) {
      totalBytes += bytesLeidos;
      
      // Actualizar barra de progreso
      int porcentaje = (totalBytes * 100) / bufferSize;
      if (porcentaje != ultimoPorcentaje && porcentaje % 10 == 0) {
        mostrarBarraGrabacion(porcentaje);
        Serial.printf("   • Progreso: %d%%\n", porcentaje);
        ultimoPorcentaje = porcentaje;
      }
    }
    
    // Timeout de seguridad
    if (millis() - inicioGrabacion > (RECORD_TIME * 1000 + 2000)) {
      Serial.println("   ⚠️  Timeout en grabación");
      break;
    }
  }
  
  // Normalizar y aplicar ganancia al audio
  normalizarAudio(samples, totalBytes / sizeof(int16_t));
  
  return totalBytes;
}

void normalizarAudio(int16_t* samples, size_t numSamples) {
  // Encontrar valor máximo
  int16_t maxVal = 0;
  for (size_t i = 0; i < numSamples; i++) {
    int16_t absVal = abs(samples[i]);
    if (absVal > maxVal) {
      maxVal = absVal;
    }
  }
  
  if (maxVal == 0) return; // Evitar división por cero
  
  // Aplicar ganancia para normalizar
  float ganancia = 32767.0 / (float)maxVal * 0.8; // 80% del máximo
  
  Serial.printf("   • Normalizando audio (ganancia: %.2f)\n", ganancia);
  
  for (size_t i = 0; i < numSamples; i++) {
    int32_t valor = (int32_t)(samples[i] * ganancia);
    // Limitar valores
    if (valor > 32767) valor = 32767;
    if (valor < -32768) valor = -32768;
    samples[i] = (int16_t)valor;
  }
}


// ============================================================================
// ENVÍO DE AUDIO A LA API
// ============================================================================

void enviarAudioAPI() {
  Serial.println("\n📤 Enviando audio a la API...");
  estadoActual = ENVIANDO_API;
  
  dibujarCaraKawaii(PENSANDO);
  mostrarTextoEstado("Procesando...", COLOR_AMARILLO);
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("   ✗ WiFi desconectado");
    mostrarError("Sin WiFi");
    delay(2000);
    estadoActual = ESPERANDO_COMANDO;
    dibujarCaraKawaii(TRISTE);
    mostrarTextoEstado("Presiona ENTER", COLOR_AMARILLO);
    return;
  }
  
  HTTPClient http;
  
  String url = String(API_URL) + "/chat";
  Serial.printf("   • URL: %s\n", url.c_str());
  
  http.begin(url);
  http.setTimeout(30000); // 30 segundos timeout
  
  // Crear boundary para multipart/form-data
  String boundary = "----ESP32Boundary" + String(millis());
  http.addHeader("Content-Type", "multipart/form-data; boundary=" + boundary);
  
  // Construir cuerpo del POST
  String bodyStart = "--" + boundary + "\r\n";
  bodyStart += "Content-Disposition: form-data; name=\"device_id\"\r\n\r\n";
  bodyStart += String(DEVICE_ID) + "\r\n";
  bodyStart += "--" + boundary + "\r\n";
  bodyStart += "Content-Disposition: form-data; name=\"file\"; filename=\"audio.wav\"\r\n";
  bodyStart += "Content-Type: audio/wav\r\n\r\n";
  
  String bodyEnd = "\r\n--" + boundary + "--\r\n";
  
  // Crear header WAV
  uint8_t wavHeader[44];
  crearHeaderWAV(wavHeader, audioBufferSize);
  
  // Calcular tamaño total
  size_t totalSize = bodyStart.length() + sizeof(wavHeader) + audioBufferSize + bodyEnd.length();
  
  Serial.printf("   • Tamaño total: %d bytes\n", totalSize);
  
  // Crear buffer temporal para el cuerpo completo
  uint8_t* bodyBuffer = (uint8_t*)malloc(totalSize);
  if (!bodyBuffer) {
    Serial.println("   ✗ Error: No hay memoria para buffer de envío");
    mostrarError("Sin Memoria");
    http.end();
    delay(2000);
    estadoActual = ESPERANDO_COMANDO;
    return;
  }
  
  // Copiar todo al buffer
  size_t offset = 0;
  memcpy(bodyBuffer + offset, bodyStart.c_str(), bodyStart.length());
  offset += bodyStart.length();
  
  memcpy(bodyBuffer + offset, wavHeader, sizeof(wavHeader));
  offset += sizeof(wavHeader);
  
  memcpy(bodyBuffer + offset, audioBuffer, audioBufferSize);
  offset += audioBufferSize;
  
  memcpy(bodyBuffer + offset, bodyEnd.c_str(), bodyEnd.length());
  
  // Enviar POST
  Serial.println("   • Enviando petición HTTP...");
  int httpCode = http.POST(bodyBuffer, totalSize);
  
  free(bodyBuffer);
  
  Serial.printf("   • Código HTTP: %d\n", httpCode);
  
  if (httpCode == 200) {
    Serial.println("   ✓ Respuesta recibida!");
    
    // Leer header con texto de respuesta
    String textoB64 = http.header("X-Response-Text-B64");
    String textoRespuesta = "";
    
    if (textoB64.length() > 0) {
      textoRespuesta = base64Decode(textoB64);
      Serial.println("   📝 Respuesta: " + textoRespuesta);
      
      mostrarRespuestaTexto(textoRespuesta);
    }
    
    // Obtener audio de respuesta
    WiFiClient* stream = http.getStreamPtr();
    size_t audioSize = http.getSize();
    
    if (audioSize > 0 && audioSize < ESP.getFreePsram()) {
      Serial.printf("   🔊 Recibiendo audio: %d bytes\n", audioSize);
      
      uint8_t* audioRespuesta = (uint8_t*)ps_malloc(audioSize);
      if (audioRespuesta) {
        size_t bytesLeidos = stream->readBytes(audioRespuesta, audioSize);
        
        if (bytesLeidos > 0) {
          Serial.printf("   ✓ Audio recibido: %d bytes\n", bytesLeidos);
          reproducirAudio(audioRespuesta, bytesLeidos);
        }
        
        free(audioRespuesta);
      } else {
        Serial.println("   ✗ Error: No hay memoria para audio de respuesta");
      }
    }
    
  } else {
    Serial.printf("   ✗ Error HTTP: %d\n", httpCode);
    String error = http.getString();
    Serial.println("   Detalles: " + error);
    
    mostrarError("Error API");
    delay(2000);
  }
  
  http.end();
  
  // Volver a estado de espera
  delay(1000);
  estadoActual = ESPERANDO_COMANDO;
  dibujarCaraKawaii(FELIZ);
  mostrarTextoEstado("Presiona ENTER", COLOR_VERDE);
  Serial.println("\n✅ Listo para nueva consulta");
}



// Crear header WAV estándar
void crearHeaderWAV(uint8_t* header, size_t dataSize) {
  uint32_t chunkSize = dataSize + 36;
  uint32_t subchunk2Size = dataSize;
  uint16_t audioFormat = 1; // PCM
  uint16_t numChannels = 1; // Mono
  uint32_t sampleRate = SAMPLE_RATE;
  uint16_t bitsPerSample = BITS_PER_SAMPLE;
  uint32_t byteRate = sampleRate * numChannels * bitsPerSample / 8;
  uint16_t blockAlign = numChannels * bitsPerSample / 8;
  
  // RIFF header
  memcpy(header, "RIFF", 4);
  memcpy(header + 4, &chunkSize, 4);
  memcpy(header + 8, "WAVE", 4);
  
  // fmt subchunk
  memcpy(header + 12, "fmt ", 4);
  uint32_t subchunk1Size = 16;
  memcpy(header + 16, &subchunk1Size, 4);
  memcpy(header + 20, &audioFormat, 2);
  memcpy(header + 22, &numChannels, 2);
  memcpy(header + 24, &sampleRate, 4);
  memcpy(header + 28, &byteRate, 4);
  memcpy(header + 32, &blockAlign, 2);
  memcpy(header + 34, &bitsPerSample, 2);
  
  // data subchunk
  memcpy(header + 36, "data", 4);
  memcpy(header + 40, &subchunk2Size, 4);
}

// Decodificador Base64 simple
String base64Decode(String input) {
  const char* b64chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  String output = "";
  int val = 0, valb = -8;
  
  for (char c : input) {
    if (c == '=') break;
    const char* found = strchr(b64chars, c);
    if (!found) continue;
    
    val = (val << 6) + (found - b64chars);
    valb += 6;
    
    if (valb >= 0) {
      output += char((val >> valb) & 0xFF);
      valb -= 8;
    }
  }
  return output;
}


// ============================================================================
// REPRODUCCIÓN DE AUDIO EN ALTAVOZ
// ============================================================================

void reproducirAudio(uint8_t* audioData, size_t audioSize) {
  Serial.println("\n🔊 Reproduciendo audio...");
  estadoActual = REPRODUCIENDO_RESPUESTA;
  
  dibujarCaraKawaii(HABLANDO);
  mostrarTextoEstado("Hablando...", COLOR_VERDE);
  
  // Verificar si tiene header WAV
  size_t offset = 0;
  if (audioSize > 44 && memcmp(audioData, "RIFF", 4) == 0) {
    Serial.println("   • Detectado formato WAV, saltando header");
    offset = 44; // Saltar header WAV estándar
  }
  
  // Datos de audio PCM
  uint8_t* pcmData = audioData + offset;
  size_t pcmSize = audioSize - offset;
  
  Serial.printf("   • Tamaño PCM: %d bytes\n", pcmSize);
  Serial.printf("   • Duración aprox: %.1f segundos\n", 
                (float)pcmSize / (SAMPLE_RATE * sizeof(int16_t)));
  
  // Reproducir en bloques
  size_t bytesEscritos = 0;
  size_t totalEscrito = 0;
  size_t bloqueSize = BUFFER_SIZE * 4; // 2KB por bloque
  
  unsigned long inicioReproduccion = millis();
  
  while (totalEscrito < pcmSize) {
    size_t bytesAEscribir = min(bloqueSize, pcmSize - totalEscrito);
    
    esp_err_t result = i2s_write(I2S_NUM_TX, 
                                 pcmData + totalEscrito, 
                                 bytesAEscribir,
                                 &bytesEscritos, 
                                 portMAX_DELAY);
    
    if (result == ESP_OK) {
      totalEscrito += bytesEscritos;
      
      // Actualizar animación de cara cada 200ms
      if (millis() % 200 < 100) {
        dibujarCaraKawaii(HABLANDO);
      }
      
    } else {
      Serial.printf("   ✗ Error escribiendo I2S: %d\n", result);
      break;
    }
    
    // Timeout de seguridad (máximo 30 segundos)
    if (millis() - inicioReproduccion > 30000) {
      Serial.println("   ⚠️  Timeout en reproducción");
      break;
    }
  }
  
  // Esperar a que termine de reproducir el buffer DMA
  i2s_zero_dma_buffer(I2S_NUM_TX);
  delay(100);
  
  unsigned long duracion = millis() - inicioReproduccion;
  Serial.printf("   ✓ Reproducción completada en %lu ms\n", duracion);
  Serial.printf("   • Bytes escritos: %d / %d\n", totalEscrito, pcmSize);
  
  // Pequeña pausa antes de volver a estar listo
  delay(500);
}

