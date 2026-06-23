# 🤖 Guía Completa ESP32-S3 con Audio Bidireccional

## 📋 Índice
1. [Hardware Requerido](#hardware-requerido)
2. [Diagrama de Conexiones](#diagrama-de-conexiones)
3. [Instalación del Software](#instalación-del-software)
4. [Configuración](#configuración)
5. [Compilación y Carga](#compilación-y-carga)
6. [Uso del Sistema](#uso-del-sistema)
7. [Solución de Problemas](#solución-de-problemas)

---

## 🔧 Hardware Requerido

### Componentes Principales
- **ESP32-S3** (con PSRAM recomendado)
- **Pantalla ST7789** (240x320 píxeles, SPI)
- **Micrófono INMP441** (I2S MEMS)
- **Amplificador MAX98357A** (I2S DAC)
- **Altavoz** (4-8Ω, 3W)
- **Cables Dupont** (macho-hembra y macho-macho)
- **Fuente de alimentación** USB-C 5V/2A

### Especificaciones Técnicas
- **Sample Rate**: 16kHz
- **Bits per Sample**: 16 bits
- **Formato**: PCM Mono
- **Resolución Pantalla**: 240x320 píxeles

---

## 🔌 Diagrama de Conexiones

### Pantalla ST7789 (SPI)
```
ST7789          ESP32-S3
--------------------------
VCC      →      3.3V
GND      →      GND
CS       →      GPIO 10
DC       →      GPIO 11
RST      →      GPIO 12
SDA/MOSI →      GPIO 13
SCL/SCK  →      GPIO 14
BL       →      3.3V (o PWM)
```

### Micrófono INMP441 (I2S)
```
INMP441         ESP32-S3
--------------------------
VDD      →      3.3V
GND      →      GND
WS/LR    →      GPIO 5
SCK      →      GPIO 4
SD       →      GPIO 6
L/R      →      GND (mono izquierdo)
```

### Altavoz MAX98357A (I2S)
```
MAX98357A       ESP32-S3
--------------------------
VIN      →      5V (o 3.3V)
GND      →      GND
LRCLK    →      GPIO 17
BCLK     →      GPIO 16
DIN      →      GPIO 15
GAIN     →      GND (9dB) o 100K a VDD (15dB)
SD       →      3.3V (siempre activo)

Altavoz Conexión
--------------------------
+        →      MAX98357A (+)
-        →      MAX98357A (-)
```

### Diagrama Visual
```
                     ┌─────────────┐
                     │  ESP32-S3   │
                     │             │
    ┌───────────────►│ GPIO 10-14  │──────────────┐
    │                │             │              │
    │    ┌──────────►│ GPIO 4-6    │◄─────┐       │
    │    │           │             │      │       │
    │    │      ┌───►│ GPIO 15-17  │──┐   │       │
    │    │      │    │             │  │   │       │
    │    │      │    │   3.3V/5V   │  │   │       │
    │    │      │    │   GND       │  │   │       │
    │    │      │    └─────────────┘  │   │       │
    │    │      │                     │   │       │
    ▼    ▼      ▼                     ▼   ▼       ▼
┌────────┐ ┌────────┐           ┌─────────┐ ┌─────────┐
│ST7789  │ │INMP441 │           │MAX98357A│ │  WiFi   │
│Display │ │  Mic   │           │   DAC   │ │ Router  │
└────────┘ └────────┘           └────┬────┘ └─────────┘
                                     │
                                     ▼
                                ┌─────────┐
                                │ Altavoz │
                                │  4-8Ω   │
                                └─────────┘
```

---

## 💻 Instalación del Software

### 1. Instalar Arduino IDE
1. Descargar desde: https://www.arduino.cc/en/software
2. Instalar versión 2.x o superior

### 2. Configurar Soporte para ESP32
1. Abrir Arduino IDE
2. Ir a **Archivo > Preferencias**
3. En "Gestor de URLs Adicionales de Tarjetas", agregar:
   ```
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
4. Ir a **Herramientas > Placa > Gestor de tarjetas**
5. Buscar "esp32" e instalar "**ESP32 by Espressif Systems**" (versión 2.0.14 o superior)

**⚠️ Nota**: El código es compatible con Arduino Core 2.0.14+. Si usas una versión anterior, podrías tener errores de compilación relacionados con `esp_task_wdt_init()`.

### 3. Instalar Bibliotecas Requeridas
En **Herramientas > Administrar Bibliotecas**, instalar:

- **Adafruit GFX Library** (versión 1.11.x)
- **Adafruit ST7735 and ST7789 Library** (versión 1.10.x)

---

## ⚙️ Configuración

### 1. Abrir el Sketch
1. Abrir el archivo `esp32s3_robot_completo.ino`

### 2. Configurar WiFi y API
Buscar estas líneas al inicio del código:

```cpp
#define WIFI_SSID         "Tenda_88ADC0"      // ⚠️ CAMBIAR
#define WIFI_PASSWORD     "924113587"         // ⚠️ CAMBIAR
#define API_URL           "http://18.221.246.87:8000"
#define DEVICE_ID         "esp32s3-display"
```

**Modificar:**
- `WIFI_SSID`: Tu red WiFi
- `WIFI_PASSWORD`: Tu contraseña WiFi
- `API_URL`: URL de tu servidor (mantener el actual si usas el servidor remoto)
- `DEVICE_ID`: Identificador único para tu dispositivo

### 3. Verificar Pines (Opcional)
Si tus conexiones son diferentes, modificar:

```cpp
// Pantalla ST7789
#define TFT_CS    10
#define TFT_DC    11
#define TFT_RST   12
#define TFT_MOSI  13
#define TFT_SCK   14

// Altavoz MAX98357A
#define I2S_SPEAKER_BCLK  16
#define I2S_SPEAKER_LRCK  17
#define I2S_SPEAKER_DOUT  15

// Micrófono INMP441
#define I2S_MIC_BCLK      4
#define I2S_MIC_LRCK      5
#define I2S_MIC_DIN       6
```

---

## 🚀 Compilación y Carga

### 1. Configurar Placa
En **Herramientas**, configurar:

- **Placa**: "ESP32S3 Dev Module"
- **USB CDC On Boot**: "Enabled"
- **USB Mode**: "Hardware CDC and JTAG"
- **PSRAM**: "OPI PSRAM" (si tu ESP32-S3 tiene PSRAM)
- **Partition Scheme**: "Huge APP (3MB No OTA/1MB SPIFFS)"
- **Upload Speed**: "921600"
- **CPU Frequency**: "240MHz (WiFi)"
- **Flash Mode**: "QIO 80MHz"
- **Flash Size**: "4MB" (o el tamaño de tu módulo)
- **Puerto**: Seleccionar el puerto COM correspondiente

### 2. Compilar
1. Clic en el botón **Verificar** (✓)
2. Esperar a que compile sin errores

### 3. Cargar
1. Conectar ESP32-S3 mediante USB
2. Si no entra en modo bootloader automáticamente:
   - Mantener presionado el botón **BOOT**
   - Presionar y soltar el botón **RESET**
   - Soltar el botón **BOOT**
3. Clic en el botón **Cargar** (→)
4. Esperar a que termine la carga

---

## 🎮 Uso del Sistema

### Inicio del Sistema
1. Al encender, verás en la pantalla:
   ```
   Robot Asistente
   ESP32-S3 + Gemini
   v2.0
   ```

2. Se conectará automáticamente a WiFi

3. Una vez conectado, mostrará una cara feliz y el mensaje:
   ```
   ¡Listo! Presiona ENTER
   ```

### Interacción por Voz

#### Opción 1: Monitor Serial (Pruebas)
1. Abrir **Monitor Serial** a 115200 baudios
2. Presionar **ENTER** para iniciar grabación
3. Hablar durante 5 segundos
4. El sistema:
   - Captura el audio
   - Lo envía a la API
   - Recibe la respuesta
   - La reproduce por el altavoz
   - Muestra el texto en pantalla

#### Opción 2: Botón Físico (Futuro)
Puedes agregar un botón en GPIO 0 para activar la grabación sin computadora.

### Estados del Sistema

| Estado | Pantalla | Emoción |
|--------|----------|---------|
| **Esperando** | "¡Listo! Presiona ENTER" | 😊 Feliz |
| **Grabando** | Barra de progreso | 😲 Sorprendido |
| **Procesando** | "Procesando..." | 🤔 Pensando |
| **Hablando** | Respuesta en texto | 😄 Hablando |
| **Error** | "Error XXX" | 😠 Enojado |

---

## 🔧 Solución de Problemas

### Errores de Compilación

#### Error: "invalid conversion from 'int' to 'const esp_task_wdt_config_t*'"
**Causa**: Versión incompatible del Arduino Core ESP32

**Solución**:
1. Actualizar a ESP32 Arduino Core 2.0.14 o superior
2. En **Gestor de tarjetas**, desinstalar versión actual
3. Instalar versión 2.0.14 o más reciente
4. Reiniciar Arduino IDE

El código incluye compatibilidad automática con ambas versiones.

#### Error: "Adafruit_ST7789 was not declared"
**Causa**: Bibliotecas no instaladas

**Solución**:
1. Ir a **Herramientas > Administrar Bibliotecas**
2. Instalar:
   - Adafruit GFX Library
   - Adafruit ST7735 and ST7789 Library
3. Reiniciar Arduino IDE

### Pantalla no funciona
**Síntoma**: Pantalla en blanco o con basura

**Soluciones**:

1. Verificar conexiones (especialmente CS, DC, RST)
2. Verificar que la alimentación sea 3.3V
3. Probar diferentes rotaciones (cambiar `tft.setRotation(0)` a 1, 2 o 3)
4. Verificar que SPI funcione:
   ```cpp
   SPI.begin(TFT_SCK, -1, TFT_MOSI, TFT_CS);
   ```

### Micrófono no captura audio
**Síntoma**: Audio silencioso o ruidoso

**Soluciones**:
1. Verificar que L/R del INMP441 esté conectado a GND
2. Verificar alimentación 3.3V estable
3. Verificar conexión de todos los pines I2S
4. Comprobar que los pines BCLK, LRCK y DIN sean correctos
5. Verificar en Monitor Serial: debe mostrar "Micrófono inicializado"

### Altavoz no reproduce
**Síntoma**: Sin sonido o distorsionado

**Soluciones**:
1. Verificar que SD del MAX98357A esté conectado a 3.3V
2. Verificar alimentación (5V preferido)
3. Verificar polaridad del altavoz
4. Ajustar ganancia (GAIN a GND para 9dB, o 100K a VDD para 15dB)
5. Comprobar que los pines I2S TX sean correctos

### WiFi no conecta
**Síntoma**: "Error WiFi" en pantalla

**Soluciones**:
1. Verificar SSID y contraseña
2. Asegurarse de usar red 2.4GHz (ESP32 no soporta 5GHz)
3. Verificar que el router esté encendido
4. Acercarse al router
5. Verificar en Monitor Serial el RSSI (debe ser > -70dBm)

### Error de memoria
**Síntoma**: "Sin Memoria" en pantalla

**Soluciones**:
1. Verificar que PSRAM esté habilitado en configuración de placa
2. Usar Partition Scheme: "Huge APP"
3. Verificar en Monitor Serial:
   ```
   RAM libre: XXXXX bytes
   PSRAM libre: XXXXX bytes
   ```

### API no responde
**Síntoma**: "Error API" o timeout

**Soluciones**:
1. Verificar que el servidor esté corriendo:
   ```bash
   curl http://18.221.246.87:8000/health
   ```
2. Verificar conectividad:
   ```bash
   ping 18.221.246.87
   ```
3. Revisar logs del servidor
4. Verificar firewall del servidor

### Audio distorsionado
**Síntoma**: Audio con ruido o cortado

**Soluciones**:
1. Usar cables cortos (< 20cm para I2S)
2. Agregar capacitor de 100μF en alimentación del MAX98357A
3. Usar fuente de alimentación adecuada (2A mínimo)
4. Separar cables de audio de cables de alimentación
5. Reducir ganancia del MAX98357A

---

## 📊 Monitor Serial - Mensajes

### Inicio Exitoso
```
╔════════════════════════════════════════╗
║  ESP32-S3 Robot Asistente v2.0        ║
║  Con Audio Bidireccional + Display    ║
╚════════════════════════════════════════╝

🖥️  Inicializando Display ST7789...
   ✓ Display inicializado correctamente

🎤 Inicializando Micrófono INMP441 (I2S RX)...
   ✓ Micrófono inicializado correctamente
   • Sample Rate: 16000 Hz
   • Bits: 16
   • Pines: BCLK=4, LRCK=5, DIN=6

🔊 Inicializando Altavoz MAX98357A (I2S TX)...
   ✓ Altavoz inicializado correctamente
   • Sample Rate: 16000 Hz
   • Bits: 16
   • Pines: BCLK=16, LRCK=17, DOUT=15

📡 Conectando a WiFi...
   • SSID: Tenda_88ADC0
   ✓ WiFi conectado!
   • IP: 192.168.0.100
   • RSSI: -45 dBm

✅ Sistema iniciado correctamente
📝 Presiona ENTER para iniciar grabación
💾 RAM libre: 245672 bytes
💾 PSRAM libre: 8388608 bytes
```

### Durante Grabación
```
🎙️  Iniciando grabación...
   📊 Capturando audio...
   • Progreso: 10%
   • Progreso: 20%
   ...
   ✓ Grabados 160000 bytes (80000 muestras)
   • Normalizando audio (ganancia: 2.35)
```

### Durante Envío
```
📤 Enviando audio a la API...
   • URL: http://18.221.246.87:8000/chat
   • Tamaño total: 160044 bytes
   • Enviando petición HTTP...
   • Código HTTP: 200
   ✓ Respuesta recibida!
   📝 Respuesta: Hola, ¿en qué puedo ayudarte hoy?
```

### Durante Reproducción
```
🔊 Reproduciendo audio...
   • Detectado formato WAV, saltando header
   • Tamaño PCM: 89600 bytes
   • Duración aprox: 2.8 segundos
   ✓ Reproducción completada en 2845 ms
   • Bytes escritos: 89600 / 89600

✅ Listo para nueva consulta
```

---

## 🎨 Personalización

### Cambiar Colores de la Interfaz
Modificar estas constantes:

```cpp
#define COLOR_FONDO       0x001F    // Azul oscuro
#define COLOR_CARA        0xFFFF    // Blanco
#define COLOR_OJO         0x0000    // Negro
#define COLOR_BOCA        0xF800    // Rojo
#define COLOR_MEJILLA     0xFBEA    // Rosa claro
#define COLOR_TEXTO       0xFFFF    // Blanco
#define COLOR_VERDE       0x07E0    // Verde
#define COLOR_AMARILLO    0xFFE0    // Amarillo
```

**Formato RGB565**: Use este convertidor https://chrishewett.com/blog/true-rgb565-colour-picker/

### Cambiar Tiempo de Grabación
Modificar:
```cpp
#define RECORD_TIME       5  // Segundos (de 1 a 10)
```

### Ajustar Ganancia de Micrófono
En la función `normalizarAudio()`, modificar:
```cpp
float ganancia = 32767.0 / (float)maxVal * 0.8; // 80% del máximo
```

---

## 📐 Especificaciones Técnicas

### Consumo de Energía
- **Idle**: ~150mA @ 5V
- **WiFi activo**: ~250mA @ 5V
- **Grabando**: ~280mA @ 5V
- **Reproduciendo**: ~350mA @ 5V (con altavoz a volumen medio)

### Memoria
- **Flash**: ~1.2MB usado
- **RAM**: ~180KB usado
- **PSRAM**: ~160KB usado (para buffers de audio)

### Latencia
- **Grabación**: 5 segundos (configurable)
- **Envío a API**: 1-3 segundos (depende de red)
- **Procesamiento Gemini**: 2-5 segundos
- **Reproducción**: 1-5 segundos (depende de respuesta)
- **Total**: ~10-18 segundos por interacción

---

## 🆘 Soporte

### Recursos Adicionales
- **Documentación ESP32-S3**: https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/
- **Adafruit GFX Guide**: https://learn.adafruit.com/adafruit-gfx-graphics-library
- **I2S Audio Guide**: https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/peripherals/i2s.html

### Contacto
Si tienes problemas, revisa:
1. Este documento completo
2. Los mensajes en Monitor Serial (115200 baudios)
3. El archivo `TROUBLESHOOTING.md` del proyecto

---

## 📝 Notas Importantes

### Seguridad
- ⚠️ **No expongas tu WiFi password en el código si subes a GitHub**
- Usa archivo `.env` o configuración externa para credenciales
- El API_URL debe usar HTTPS en producción

### Limitaciones Conocidas
- Sample rate fijo de 16kHz (óptimo para voz)
- Grabación máxima de 10 segundos (límite de memoria)
- WiFi solo 2.4GHz
- Requiere PSRAM para buffers de audio grandes

### Futuras Mejoras
- [ ] Botón físico para activación
- [ ] Detección de voz (VAD) automática
- [ ] Streaming de audio en lugar de grabar completo
- [ ] Soporte para múltiples idiomas
- [ ] Configuración vía web (sin recompilar)
- [ ] OTA (actualización por aire)

---

## ✅ Checklist de Verificación

Antes de reportar un problema, verifica:

- [ ] Todas las conexiones están correctas según el diagrama
- [ ] La alimentación es estable (5V/2A mínimo)
- [ ] El ESP32-S3 tiene PSRAM habilitado
- [ ] Las bibliotecas están instaladas correctamente
- [ ] WiFi SSID y password son correctos
- [ ] El servidor API está corriendo y accesible
- [ ] Monitor Serial muestra mensajes de inicio correctos
- [ ] No hay errores de compilación

---

**¡Listo! Tu ESP32-S3 Robot Asistente está configurado y funcionando.** 🎉

Para actualizaciones, visita: https://github.com/MartinUscanga/Robot-ESP32
