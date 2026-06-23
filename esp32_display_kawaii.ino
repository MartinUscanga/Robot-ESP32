/**
 * ESP32 + ST7789 Display - Interfaz Kawaii para Gemini API
 * =========================================================
 * 
 * Conexiones ST7789 (240x240):
 * - CS   -> GPIO 5
 * - DC   -> GPIO 2
 * - RST  -> GPIO 15
 * - SCK  -> GPIO 18 (VSPI SCK)
 * - MOSI -> GPIO 23 (VSPI MOSI)
 * - VCC  -> 3.3V
 * - GND  -> GND
 * - BLK  -> 3.3V (backlight siempre encendido, o GPIO para PWM)
 * 
 * IMPORTANTE: 
 * - Actualiza WIFI_SSID y WIFI_PASS con tus credenciales
 * - La API debe estar corriendo en http://18.221.246.87:8000
 * - Usa Monitor Serial a 115200 baudios
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// ========== CONFIGURACIÓN WiFi ==========
const char* WIFI_SSID = "TU_WIFI_SSID";        // ⚠️ CAMBIAR
const char* WIFI_PASS = "TU_WIFI_PASSWORD";    // ⚠️ CAMBIAR

// ========== CONFIGURACIÓN API ==========
const char* API_URL = "http://18.221.246.87:8000/chat/texto";
const char* DEVICE_ID = "robot-kawaii-01";

// ========== CONFIGURACIÓN ST7789 ==========
#define TFT_CS    5
#define TFT_DC    2
#define TFT_RST   15
#define TFT_MOSI  23
#define TFT_SCLK  18

// Crear objeto display
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// ========== COLORES ==========
#define COLOR_FONDO     0x001F    // Azul oscuro
#define COLOR_CARA      0xFFFF    // Blanco
#define COLOR_OJO       0x0000    // Negro
#define COLOR_BOCA      0xF800    // Rojo
#define COLOR_MEJILLA   0xFBEA    // Rosa claro
#define COLOR_TEXTO     0xFFFF    // Blanco

// ========== DIMENSIONES PANTALLA ==========
#define ANCHO   240
#define ALTO    240
#define CENTRO_X (ANCHO / 2)
#define CENTRO_Y (ALTO / 2)

// ========== ESTADO ==========
enum Estado {
  CONECTANDO_WIFI,
  ESPERANDO_ENTRADA,
  ENVIANDO_API,
  MOSTRANDO_RESPUESTA,
  ERROR_CONEXION
};

Estado estadoActual = CONECTANDO_WIFI;
String inputBuffer = "";
String textoRespuesta = "";
unsigned long ultimoFrame = 0;
int animacionFrame = 0;

// ========== EMOCIONES ==========
enum Emocion {
  FELIZ,
  TRISTE,
  PENSANDO,
  SORPRENDIDO,
  ENOJADO
};

Emocion emocionActual = FELIZ;

// ========== BASE64 DECODER (simple) ==========
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

// ========== SETUP ==========
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n\n=== ESP32 Robot Kawaii ===");
  Serial.println("Inicializando...");

  // ===== Inicializar Display =====
  Serial.println("Inicializando ST7789...");
  
  // Configurar SPI manualmente para mayor estabilidad
  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
  SPI.setFrequency(27000000); // 27MHz (más estable que 40MHz)
  
  tft.init(240, 240, SPI_MODE2);
  delay(100);
  
  tft.setRotation(0); // 0, 1, 2, o 3 según orientación deseada
  tft.fillScreen(COLOR_FONDO);
  delay(100);
  
  Serial.println("✓ Display inicializado");
  
  // Mostrar pantalla de inicio
  dibujarPantallaInicio();
  
  // ===== Conectar WiFi =====
  Serial.println("\nConectando a WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 30) {
    delay(500);
    Serial.print(".");
    intentos++;
    
    // Actualizar animación de carga
    if (intentos % 3 == 0) {
      dibujarCargando(intentos / 3);
    }
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WiFi conectado!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    estadoActual = ESPERANDO_ENTRADA;
    dibujarCaraKawaii(FELIZ);
    mostrarTextoEstado("¡Listo! Escribe algo...", COLOR_TEXTO);
  } else {
    Serial.println("\n✗ Error: No se pudo conectar a WiFi");
    estadoActual = ERROR_CONEXION;
    dibujarCaraKawaii(TRISTE);
    mostrarTextoEstado("Error WiFi :(", COLOR_BOCA);
  }
  
  Serial.println("\n=== Sistema listo ===");
  Serial.println("Escribe un mensaje y presiona ENTER");
  Serial.print("\nRAM libre: ");
  Serial.println(ESP.getFreeHeap());
}

// ========== LOOP ==========
void loop() {
  // Leer entrada serial
  if (Serial.available() > 0) {
    char c = Serial.read();
    
    if (c == '\n' || c == '\r') {
      if (inputBuffer.length() > 0) {
        procesarMensaje(inputBuffer);
        inputBuffer = "";
      }
    } else {
      inputBuffer += c;
    }
  }
  
  // Animar cara cada 100ms
  unsigned long ahora = millis();
  if (ahora - ultimoFrame > 100) {
    ultimoFrame = ahora;
    animacionFrame++;
    
    if (estadoActual == ESPERANDO_ENTRADA || estadoActual == MOSTRANDO_RESPUESTA) {
      animarCara();
    }
  }
  
  delay(10); // Pequeño delay para estabilidad
}

// ========== PROCESAR MENSAJE ==========
void procesarMensaje(String mensaje) {
  Serial.println("\n>>> Enviando: " + mensaje);
  estadoActual = ENVIANDO_API;
  
  dibujarCaraKawaii(PENSANDO);
  mostrarTextoEstado("Pensando...", COLOR_TEXTO);
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("✗ WiFi desconectado");
    estadoActual = ERROR_CONEXION;
    dibujarCaraKawaii(TRISTE);
    mostrarTextoEstado("Sin WiFi :(", COLOR_BOCA);
    return;
  }
  
  HTTPClient http;
  http.begin(API_URL);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.setTimeout(15000); // 15 segundos timeout
  
  String body = "mensaje=" + urlEncode(mensaje) + "&device_id=" + String(DEVICE_ID);
  
  Serial.println("Esperando respuesta de API...");
  int httpCode = http.POST(body);
  
  if (httpCode == 200) {
    Serial.println("✓ Respuesta recibida!");
    
    // Leer header con texto en base64
    String textoB64 = http.header("X-Response-Text-B64");
    if (textoB64.length() > 0) {
      textoRespuesta = base64Decode(textoB64);
      Serial.println("<<< Respuesta: " + textoRespuesta);
      
      // Detectar emoción del texto
      emocionActual = detectarEmocion(textoRespuesta);
      
      estadoActual = MOSTRANDO_RESPUESTA;
      dibujarCaraKawaii(emocionActual);
      mostrarRespuesta(textoRespuesta);
      
    } else {
      Serial.println("✗ Error: header X-Response-Text-B64 vacío");
      estadoActual = ERROR_CONEXION;
      dibujarCaraKawaii(TRISTE);
      mostrarTextoEstado("Error en respuesta", COLOR_BOCA);
    }
    
  } else {
    Serial.print("✗ Error HTTP: ");
    Serial.println(httpCode);
    Serial.println(http.getString());
    estadoActual = ERROR_CONEXION;
    dibujarCaraKawaii(ENOJADO);
    mostrarTextoEstado("Error API :(", COLOR_BOCA);
  }
  
  http.end();
  
  // Volver a estado de espera después de 5 segundos
  if (estadoActual == MOSTRANDO_RESPUESTA) {
    delay(5000);
    estadoActual = ESPERANDO_ENTRADA;
    dibujarCaraKawaii(FELIZ);
    mostrarTextoEstado("¡Escribe algo más!", COLOR_TEXTO);
  }
}

// ========== DIBUJAR CARA KAWAII ==========
void dibujarCaraKawaii(Emocion emocion) {
  // Limpiar zona de cara (dejar barras superior e inferior)
  tft.fillRect(0, 40, ANCHO, ALTO - 80, COLOR_FONDO);
  
  int caraY = CENTRO_Y - 10;
  
  // Ojos
  int ojoIzqX = CENTRO_X - 35;
  int ojoDerX = CENTRO_X + 35;
  int ojoY = caraY - 15;
  
  // Mejillas
  tft.fillCircle(CENTRO_X - 60, caraY + 10, 12, COLOR_MEJILLA);
  tft.fillCircle(CENTRO_X + 60, caraY + 10, 12, COLOR_MEJILLA);
  
  switch (emocion) {
    case FELIZ:
      // Ojos cerrados (feliz)
      tft.fillRect(ojoIzqX - 15, ojoY, 30, 5, COLOR_OJO);
      tft.fillRect(ojoDerX - 15, ojoY, 30, 5, COLOR_OJO);
      
      // Boca sonriente
      tft.fillCircle(CENTRO_X, caraY + 25, 25, COLOR_BOCA);
      tft.fillCircle(CENTRO_X, caraY + 15, 25, COLOR_FONDO);
      break;
      
    case TRISTE:
      // Ojos grandes y tristes
      tft.fillCircle(ojoIzqX, ojoY, 10, COLOR_OJO);
      tft.fillCircle(ojoDerX, ojoY, 10, COLOR_OJO);
      tft.fillCircle(ojoIzqX, ojoY + 5, 3, COLOR_CARA);
      tft.fillCircle(ojoDerX, ojoY + 5, 3, COLOR_CARA);
      
      // Boca triste invertida
      tft.fillCircle(CENTRO_X, caraY + 35, 25, COLOR_BOCA);
      tft.fillCircle(CENTRO_X, caraY + 45, 25, COLOR_FONDO);
      break;
      
    case PENSANDO:
      // Ojos mirando hacia arriba
      tft.fillCircle(ojoIzqX, ojoY - 5, 10, COLOR_OJO);
      tft.fillCircle(ojoDerX, ojoY - 5, 10, COLOR_OJO);
      tft.fillCircle(ojoIzqX, ojoY - 8, 3, COLOR_CARA);
      tft.fillCircle(ojoDerX, ojoY - 8, 3, COLOR_CARA);
      
      // Boca pequeña "o"
      tft.fillCircle(CENTRO_X, caraY + 25, 8, COLOR_OJO);
      break;
      
    case SORPRENDIDO:
      // Ojos muy abiertos
      tft.fillCircle(ojoIzqX, ojoY, 15, COLOR_OJO);
      tft.fillCircle(ojoDerX, ojoY, 15, COLOR_OJO);
      tft.fillCircle(ojoIzqX, ojoY, 5, COLOR_CARA);
      tft.fillCircle(ojoDerX, ojoY, 5, COLOR_CARA);
      
      // Boca "O" grande
      tft.fillCircle(CENTRO_X, caraY + 30, 15, COLOR_OJO);
      tft.fillCircle(CENTRO_X, caraY + 30, 10, COLOR_FONDO);
      break;
      
    case ENOJADO:
      // Cejas fruncidas
      tft.fillRect(ojoIzqX - 15, ojoY - 15, 30, 5, COLOR_OJO);
      tft.fillRect(ojoDerX - 15, ojoY - 15, 30, 5, COLOR_OJO);
      
      // Ojos entrecerrados
      tft.fillRect(ojoIzqX - 10, ojoY, 20, 8, COLOR_OJO);
      tft.fillRect(ojoDerX - 10, ojoY, 20, 8, COLOR_OJO);
      
      // Boca enojada (línea recta)
      tft.fillRect(CENTRO_X - 20, caraY + 30, 40, 4, COLOR_BOCA);
      break;
  }
}

// ========== ANIMAR CARA ==========
void animarCara() {
  // Parpadeo simple cada 3 segundos
  if (animacionFrame % 30 == 0 && emocionActual == FELIZ) {
    int caraY = CENTRO_Y - 10;
    int ojoIzqX = CENTRO_X - 35;
    int ojoDerX = CENTRO_X + 35;
    int ojoY = caraY - 15;
    
    // Parpadeo
    tft.fillRect(ojoIzqX - 15, ojoY - 5, 30, 15, COLOR_FONDO);
    tft.fillRect(ojoDerX - 15, ojoY - 5, 30, 15, COLOR_FONDO);
    delay(150);
    dibujarCaraKawaii(emocionActual);
  }
}

// ========== MOSTRAR TEXTO EN BARRA INFERIOR ==========
void mostrarTextoEstado(String texto, uint16_t color) {
  tft.fillRect(0, ALTO - 35, ANCHO, 35, COLOR_FONDO);
  tft.setTextColor(color);
  tft.setTextSize(1);
  tft.setCursor(10, ALTO - 25);
  tft.println(texto.substring(0, 35)); // Max 35 caracteres
}

// ========== MOSTRAR RESPUESTA (TEXTO LARGO) ==========
void mostrarRespuesta(String texto) {
  // Limpiar área de texto (debajo de la cara)
  tft.fillRect(0, ALTO - 70, ANCHO, 35, COLOR_FONDO);
  
  tft.setTextColor(COLOR_TEXTO);
  tft.setTextSize(1);
  tft.setTextWrap(true);
  
  // Mostrar primeras 2 líneas del texto
  int y = ALTO - 65;
  int x = 5;
  int maxChars = 38; // Caracteres por línea
  
  for (int i = 0; i < texto.length() && y < ALTO - 10; i += maxChars) {
    String linea = texto.substring(i, min((int)(i + maxChars), (int)texto.length()));
    tft.setCursor(x, y);
    tft.println(linea);
    y += 12;
  }
}

// ========== PANTALLA DE INICIO ==========
void dibujarPantallaInicio() {
  tft.fillScreen(COLOR_FONDO);
  tft.setTextColor(COLOR_TEXTO);
  tft.setTextSize(2);
  tft.setCursor(30, 100);
  tft.println("Robot Kawaii");
  tft.setTextSize(1);
  tft.setCursor(40, 130);
  tft.println("Powered by Gemini");
}

// ========== ANIMACIÓN DE CARGA ==========
void dibujarCargando(int dots) {
  tft.fillRect(0, 160, ANCHO, 20, COLOR_FONDO);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_TEXTO);
  tft.setCursor(80, 165);
  String puntitos = "Conectando";
  for (int i = 0; i < (dots % 4); i++) {
    puntitos += ".";
  }
  tft.println(puntitos);
}

// ========== DETECTAR EMOCIÓN DEL TEXTO ==========
Emocion detectarEmocion(String texto) {
  texto.toLowerCase();
  
  if (texto.indexOf("jaj") >= 0 || texto.indexOf("jeje") >= 0 || 
      texto.indexOf("genial") >= 0 || texto.indexOf("bien") >= 0) {
    return FELIZ;
  }
  
  if (texto.indexOf("triste") >= 0 || texto.indexOf("pena") >= 0 || 
      texto.indexOf("disculpa") >= 0 || texto.indexOf("lo siento") >= 0) {
    return TRISTE;
  }
  
  if (texto.indexOf("wow") >= 0 || texto.indexOf("increíble") >= 0 || 
      texto.indexOf("sorpre") >= 0 || texto.indexOf("!") >= 0) {
    return SORPRENDIDO;
  }
  
  if (texto.indexOf("no") >= 0 || texto.indexOf("mal") >= 0 || 
      texto.indexOf("error") >= 0) {
    return ENOJADO;
  }
  
  if (texto.indexOf("hmm") >= 0 || texto.indexOf("déjame") >= 0 || 
      texto.indexOf("pens") >= 0) {
    return PENSANDO;
  }
  
  return FELIZ; // Default
}

// ========== URL ENCODE ==========
String urlEncode(String str) {
  String encodedString = "";
  char c;
  char code0;
  char code1;
  
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == ' ') {
      encodedString += '+';
    } else if (isalnum(c)) {
      encodedString += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9) {
        code0 = c - 10 + 'A';
      }
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
    }
  }
  return encodedString;
}
