# 🤖 ESP32-S3 Robot Asistente con Audio Bidireccional

<div align="center">

![ESP32-S3](https://img.shields.io/badge/ESP32--S3-Espressif-red)
![Arduino](https://img.shields.io/badge/Arduino-Compatible-blue)
![I2S Audio](https://img.shields.io/badge/Audio-I2S%20Bidirectional-green)
![ST7789](https://img.shields.io/badge/Display-ST7789%20240x320-purple)

</div>

## 📝 Descripción

Sistema completo de asistente de voz basado en ESP32-S3 con:
- 🎤 **Captura de audio** mediante micrófono INMP441 (I2S)
- 🔊 **Reproducción de audio** mediante MAX98357A (I2S DAC)
- 🖥️ **Interfaz visual kawaii** en pantalla ST7789 240x320
- 🌐 **Conexión WiFi** con API de Gemini
- 😊 **Emociones animadas** según contexto

## 🎯 Características Principales

### Audio Bidireccional I2S
- Sample rate: 16kHz
- Formato: PCM 16-bit mono
- Normalización automática de ganancia
- Buffers optimizados con PSRAM

### Interfaz Visual Kawaii
- 6 emociones diferentes (Feliz, Triste, Pensando, Sorprendido, Enojado, Hablando)
- Animaciones fluidas de parpadeo y boca
- Barra de progreso durante grabación
- Mensajes de estado en tiempo real

### Comunicación API
- HTTP POST con multipart/form-data
- Envío de audio en formato WAV
- Recepción de audio y texto
- Manejo robusto de errores

## 🔧 Hardware Necesario

| Componente | Modelo | Pines ESP32-S3 |
|------------|--------|----------------|
| **Micrófono** | INMP441 | BCLK=4, LRCK=5, DIN=6 |
| **Altavoz/DAC** | MAX98357A | BCLK=16, LRCK=17, DOUT=15 |
| **Pantalla** | ST7789 | CS=10, DC=11, RST=12, MOSI=13, SCK=14 |
| **Altavoz** | 4-8Ω 3W | Conectado a MAX98357A |

## 📦 Bibliotecas Requeridas

```
Adafruit GFX Library (>=1.11.0)
Adafruit ST7735 and ST7789 Library (>=1.10.0)
```

Instalar desde el Gestor de Bibliotecas de Arduino IDE.

## ⚙️ Configuración Rápida

### 1. Configurar WiFi y API

Editar estas líneas en `esp32s3_robot_completo.ino`:

```cpp
#define WIFI_SSID         "TU_WIFI"           // ⚠️ CAMBIAR
#define WIFI_PASSWORD     "TU_PASSWORD"       // ⚠️ CAMBIAR
#define API_URL           "http://18.221.246.87:8000"
#define DEVICE_ID         "esp32s3-display"
```

### 2. Configuración de Arduino IDE

**Herramientas:**
- Placa: `ESP32S3 Dev Module`
- PSRAM: `OPI PSRAM`
- Partition Scheme: `Huge APP (3MB No OTA/1MB SPIFFS)`
- Upload Speed: `921600`
- USB CDC On Boot: `Enabled`

### 3. Compilar y Cargar

1. Conectar ESP32-S3 por USB
2. Seleccionar puerto COM
3. Presionar botón BOOT si es necesario
4. Cargar código

## 🚀 Uso

### Inicio
Al encender, el sistema:
1. Inicializa pantalla (muestra logo)
2. Configura I2S para micrófono y altavoz
3. Conecta a WiFi
4. Muestra cara feliz con mensaje "¡Listo!"

### Interacción
1. **Presionar ENTER** en Monitor Serial (115200 baudios)
2. **Hablar** durante 5 segundos
3. El sistema muestra barra de progreso
4. **Esperar** respuesta (cara pensando)
5. **Escuchar** respuesta por altavoz (cara hablando)
6. Ver texto de respuesta en pantalla

### Estados Visuales

| Estado | Emoción | Color Texto |
|--------|---------|-------------|
| Listo | 😊 Feliz | Verde |
| Grabando | 😲 Sorprendido | Rojo |
| Procesando | 🤔 Pensando | Amarillo |
| Hablando | 😄 Hablando (animado) | Verde |
| Error | 😠 Enojado | Rojo |

## 📊 Especificaciones Técnicas

### Memoria
- **Flash**: ~1.2MB
- **RAM**: ~180KB
- **PSRAM**: ~160KB (buffers de audio)

### Rendimiento
- **Latencia total**: 10-18 segundos
  - Grabación: 5s
  - Envío: 1-3s
  - Procesamiento: 2-5s
  - Reproducción: 1-5s

### Consumo
- **Idle**: 150mA @ 5V
- **WiFi activo**: 250mA @ 5V
- **Grabando**: 280mA @ 5V
- **Reproduciendo**: 350mA @ 5V

## 🛠️ Funciones Principales

### Inicialización
```cpp
bool inicializarDisplay()       // Configura ST7789
bool inicializarMicrofono()     // Configura I2S RX (INMP441)
bool inicializarAltavoz()       // Configura I2S TX (MAX98357A)
void conectarWiFi()             // Conecta a red WiFi
```

### Audio
```cpp
size_t grabarAudio(buffer, size)              // Captura desde micrófono
void normalizarAudio(samples, numSamples)     // Aplica ganancia
void reproducirAudio(audioData, audioSize)    // Reproduce en altavoz
```

### Pantalla
```cpp
void dibujarCaraKawaii(Emocion emocion)       // Dibuja cara con emoción
void animarCara()                             // Anima parpadeo y boca
void mostrarTextoEstado(texto, color)         // Muestra mensaje inferior
void mostrarBarraGrabacion(progreso)          // Barra de progreso 0-100%
```

### API
```cpp
void enviarAudioAPI()                         // Envía audio y recibe respuesta
void crearHeaderWAV(header, dataSize)         // Crea header WAV estándar
String base64Decode(input)                    // Decodifica texto de respuesta
```

## 🐛 Troubleshooting

### Pantalla en blanco
- Verificar conexiones SPI
- Probar diferentes rotaciones (0-3)
- Verificar alimentación 3.3V

### Sin audio de micrófono
- Conectar L/R del INMP441 a GND
- Verificar pines I2S
- Comprobar mensaje "Micrófono inicializado" en Serial

### Altavoz sin sonido
- Conectar SD del MAX98357A a 3.3V
- Verificar alimentación 5V
- Verificar polaridad del altavoz

### Error WiFi
- Usar red 2.4GHz (ESP32 no soporta 5GHz)
- Verificar SSID y password
- Acercarse al router

### Error de memoria
- Habilitar PSRAM en configuración
- Usar Partition Scheme "Huge APP"

## 📚 Documentación Completa

Para guía detallada de instalación y configuración, ver:
- **[ESP32S3_SETUP.md](./ESP32S3_SETUP.md)** - Guía completa paso a paso
- **[TROUBLESHOOTING.md](./TROUBLESHOOTING.md)** - Solución de problemas

## 🔄 Diagrama de Flujo

```
┌─────────────┐
│   Inicio    │
└──────┬──────┘
       │
       ├──► Inicializar Display
       ├──► Inicializar I2S Micrófono
       ├──► Inicializar I2S Altavoz
       ├──► Conectar WiFi
       │
       ▼
┌─────────────────┐
│ Esperar Comando │◄────────────┐
└────────┬────────┘             │
         │                      │
         ▼                      │
   [ENTER presionado]           │
         │                      │
         ▼                      │
┌─────────────────┐             │
│ Grabar 5 seg    │             │
│ (barra progreso)│             │
└────────┬────────┘             │
         │                      │
         ▼                      │
┌─────────────────┐             │
│ Enviar a API    │             │
│ (cara pensando) │             │
└────────┬────────┘             │
         │                      │
         ▼                      │
┌─────────────────┐             │
│Recibir Respuesta│             │
│ (texto + audio) │             │
└────────┬────────┘             │
         │                      │
         ▼                      │
┌─────────────────┐             │
│ Reproducir Audio│             │
│ (cara hablando) │             │
└────────┬────────┘             │
         │                      │
         └──────────────────────┘
```

## 🎨 Personalización

### Cambiar tiempo de grabación
```cpp
#define RECORD_TIME  5  // Cambiar a 1-10 segundos
```

### Ajustar ganancia de micrófono
```cpp
// En normalizarAudio()
float ganancia = 32767.0 / (float)maxVal * 0.8; // 0.5 a 1.0
```

### Modificar colores
```cpp
#define COLOR_FONDO    0x001F  // RGB565
#define COLOR_BOCA     0xF800  // Usar convertidor RGB565
```

## 📄 Licencia

Este proyecto es parte de [Robot-ESP32](https://github.com/MartinUscanga/Robot-ESP32)

## 🤝 Contribuciones

Las contribuciones son bienvenidas! Por favor:
1. Fork el proyecto
2. Crea una rama para tu feature
3. Commit tus cambios
4. Push a la rama
5. Abre un Pull Request

## 📧 Soporte

Si tienes problemas:
1. Revisa la [Guía de Instalación](./ESP32S3_SETUP.md)
2. Consulta [Troubleshooting](./TROUBLESHOOTING.md)
3. Abre un issue en GitHub

---

**Hecho con ❤️ para la comunidad maker**
