# 🤖 Robot Kawaii - ESP32 + Gemini

Sistema completo de robot interactivo con pantalla ST7789 y backend Gemini API.  
El ESP32 muestra una cara kawaii animada, envía mensajes de texto/audio a Gemini, y muestra las respuestas con emociones detectadas automáticamente.

**Componentes:**
- 🎨 **ESP32 + Display ST7789:** Interfaz visual con cara kawaii animada
- 🌐 **API FastAPI:** Puente entre ESP32 y Gemini
- 🤖 **Google Gemini:** IA conversacional con personalidad customizable

```
ESP32 (texto/audio) ──► [API FastAPI] ──► Gemini AI
        ▲                    │                │
        │                    ▼                ▼
    [Display ST7789]  ◄── [TTS] ◄───── [Respuesta texto]
```

---

## 🎯 Características

### Backend (API FastAPI)
- ✅ Conversión texto a voz (TTS) en español
- ✅ Historial de conversación por dispositivo
- ✅ Retry automático ante errores 503 de Gemini
- ✅ Soporte proxy HTTP/HTTPS/SOCKS5
- ✅ Health check y métricas
- ✅ Personalidad customizable

### Frontend (ESP32)
- ✅ Cara kawaii animada con 5 emociones (feliz, triste, pensando, sorprendido, enojado)
- ✅ Detección automática de emoción desde texto
- ✅ Display 240x240 con animaciones optimizadas (sin parpadeos)
- ✅ Memoria optimizada para ESP32 (sin buffers grandes)
- ✅ WiFi con reconexión automática
- ✅ Parpadeo realista de ojos

---

## 📦 Componentes de Hardware

| Componente | Especificación | Cantidad | Precio aprox. |
|------------|----------------|----------|---------------|
| ESP32 DevKit | 30 pines, WiFi | 1 | $8-12 USD |
| Display ST7789 | 240x240 píxeles, SPI, 3.3V | 1 | $10-15 USD |
| Cables Dupont | M-M o M-F, 10-20cm | 8 | $2-3 USD |
| Fuente 5V/1A | Micro USB o DC Jack | 1 | $3-5 USD |
| **TOTAL** | | | **~$25-35 USD** |

Ver [DIAGRAMA_CONEXION.md](DIAGRAMA_CONEXION.md) para diagrama detallado de conexiones.

---

## ⚡ Inicio Rápido

### Paso 1: Configurar API Backend

```bash
# Clonar repositorio
git clone https://github.com/MartinUscanga/Robot-ESP32.git
cd Robot-ESP32

# Instalar ffmpeg (requerido para audio)
sudo apt update && sudo apt install -y ffmpeg  # Linux
brew install ffmpeg                            # Mac

# Crear entorno virtual e instalar dependencias
python3 -m venv venv
source venv/bin/activate  # Windows: venv\Scripts\activate
pip install -r requirements.txt

# Configurar API key de Gemini
cp .env.example .env
nano .env  # Editar y agregar tu GEMINI_API_KEY
```

**Obtener API key gratis:** https://aistudio.google.com/apikey

```bash
# Correr servidor
python main.py
```

API estará en `http://localhost:8000` (o tu IP pública del VPS).

### Paso 2: Configurar ESP32

1. **Instalar Arduino IDE:** https://www.arduino.cc/en/software
2. **Agregar soporte ESP32:**
   - Archivo → Preferencias → URLs Adicionales:
   - `https://dl.espressif.com/dl/package_esp32_index.json`
   - Herramientas → Placa → Gestor de Tarjetas → Buscar "ESP32" → Instalar
3. **Instalar librerías requeridas:**
   - Programa → Incluir Librería → Administrar Librerías
   - Buscar e instalar:
     - `Adafruit GFX Library`
     - `Adafruit ST7789`
4. **Abrir código:** `esp32_display_kawaii.ino`
5. **Actualizar credenciales WiFi:**
   ```cpp
   const char* WIFI_SSID = "TU_WIFI_SSID";        // ⚠️ CAMBIAR
   const char* WIFI_PASS = "TU_WIFI_PASSWORD";    // ⚠️ CAMBIAR
   ```
6. **Verificar URL de API:**
   ```cpp
   // Para uso local
   const char* API_URL = "http://192.168.1.X:8000/chat/texto";
   
   // Para uso con VPS
   const char* API_URL = "http://18.221.246.87:8000/chat/texto";
   ```
7. **Conectar hardware** según [DIAGRAMA_CONEXION.md](DIAGRAMA_CONEXION.md)
8. **Seleccionar placa:** Herramientas → Placa → ESP32 Dev Module
9. **Subir código** al ESP32
10. **Abrir Serial Monitor** a 115200 baudios

---

## 📡 Endpoints de API

| Endpoint | Método | Descripción | Ejemplo |
|----------|--------|-------------|---------|
| `/health` | GET | Verifica estado del servidor | `curl http://localhost:8000/health` |
| `/chat` | POST | Envía audio, recibe respuesta en audio WAV | Ver test_client.py |
| `/chat/texto` | POST | Envía texto, recibe respuesta en audio WAV | Ver abajo ⬇️ |
| `/reset` | POST | Borra historial de conversación | `curl -X POST -F "device_id=robot-01" /reset` |

### Ejemplo de uso desde terminal:

```bash
# Verificar salud
curl http://localhost:8000/health

# Enviar texto (simula ESP32)
curl -X POST http://localhost:8000/chat/texto \
  -F "mensaje=Hola, cómo estás?" \
  -F "device_id=robot-kawaii-01" \
  --output respuesta.wav

# Ver texto de respuesta (en header base64)
curl -X POST http://localhost:8000/chat/texto \
  -F "mensaje=Cuéntame un chiste" \
  -F "device_id=robot-01" \
  -I | grep "X-Response-Text-B64"

# Reproducir respuesta
ffplay respuesta.wav  # Linux/Mac
```

### Usar desde Python:

```bash
# Test completo
python test_client.py --salud
python test_client.py --texto "Hola robot"
python test_client.py --audio mi_audio.wav
```

---

## 🔑 Variables de Entorno (.env)

| Variable | Requerido | Default | Descripción |
|----------|-----------|---------|-------------|
| `GEMINI_API_KEY` | ✅ **SÍ** | - | API key de Gemini ([obtener aquí](https://aistudio.google.com/apikey)) |
| `GEMINI_MODEL` | ⚪ No | `gemini-1.5-flash` | Modelo de Gemini |
| `TTS_LANG` | ⚪ No | `es` | Idioma del TTS (es, en, fr, etc.) |
| `API_PORT` | ⚪ No | `8000` | Puerto del servidor |
| `API_HOST` | ⚪ No | `0.0.0.0` | Host del servidor |
| `LOG_LEVEL` | ⚪ No | `INFO` | Nivel de logs (DEBUG, INFO, WARNING, ERROR) |
| `MAX_HISTORIAL` | ⚪ No | `12` | Turnos de conversación a recordar |
| `GEMINI_TIMEOUT` | ⚪ No | `30` | Timeout Gemini API (segundos) |
| `TTS_TIMEOUT` | ⚪ No | `15` | Timeout TTS (segundos) |
| `HTTP_PROXY` | ⚪ No | - | Proxy HTTP para Gemini API |
| `HTTPS_PROXY` | ⚪ No | - | Proxy HTTPS para Gemini API |
| `SOCKS5_PROXY` | ⚪ No | - | Proxy SOCKS5 para Gemini API |
| `PERSONALIDAD` | ⚪ No | *(ver .env.example)* | Personalidad del asistente |

**Ejemplo de configuración mínima** (`.env`):
```env
GEMINI_API_KEY=AIzaSyAbC123XYZ_tu_api_key_real_aqui
```

Ver [.env.example](.env.example) para configuración completa con todos los parámetros.

---

## 🚀 Despliegue en Producción

### Opción 1: AWS EC2 (Recomendado)

Ver guía completa en [RACKNERD_SETUP.md](RACKNERD_SETUP.md#aws-ec2-deployment).

**Setup rápido:**
```bash
# En tu instancia EC2
sudo apt update && sudo apt install -y python3-pip python3-venv ffmpeg

git clone https://github.com/MartinUscanga/Robot-ESP32.git
cd Robot-ESP32

python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt

# Configurar .env
cp .env.example .env
nano .env  # Agregar GEMINI_API_KEY

# Configurar systemd service
sudo nano /etc/systemd/system/robot-bridge.service
```

**Contenido de robot-bridge.service:**
```ini
[Unit]
Description=Robot Kawaii - Gemini Bridge
After=network.target

[Service]
User=ubuntu
WorkingDirectory=/home/ubuntu/Robot-ESP32
Environment="PATH=/home/ubuntu/Robot-ESP32/venv/bin:/usr/bin"
ExecStart=/home/ubuntu/Robot-ESP32/venv/bin/uvicorn main:app --host 0.0.0.0 --port 8000
Restart=always

[Install]
WantedBy=multi-user.target
```

```bash
# Habilitar y arrancar servicio
sudo systemctl daemon-reload
sudo systemctl enable robot-bridge
sudo systemctl start robot-bridge
sudo systemctl status robot-bridge
```

**Security Groups necesarios (AWS):**
- Puerto 22 (SSH)
- Puerto 8000 (API) - desde 0.0.0.0/0 o tu IP específica

### Opción 2: VPS (RackNerd, DigitalOcean, etc.)

Mismo proceso que AWS, pero sin necesidad de configurar Security Groups.  
Ver [RACKNERD_SETUP.md](RACKNERD_SETUP.md) para troubleshooting de región bloqueada.

---

## 🔧 Troubleshooting

### 🚨 Problema: Pantalla ST7789 parpadeando / estática negra

Ver guía completa: [ESP32_TROUBLESHOOTING.md](ESP32_TROUBLESHOOTING.md)

**Soluciones rápidas:**
- ✅ Usar cables cortos (< 20cm)
- ✅ Bajar frecuencia SPI a 27MHz
- ✅ Agregar `delay(100)` después de `tft.init()`
- ✅ Verificar alimentación (mínimo 500mA)
- ✅ Verificar VCC = **3.3V** (NO 5V)
- ✅ Probar diferentes modos SPI (MODE0, MODE2, MODE3)

**Código de prueba mínimo:**
```cpp
#include <Adafruit_ST7789.h>
#include <SPI.h>

#define TFT_CS 5
#define TFT_DC 2
#define TFT_RST 15

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(115200);
  SPI.begin(18, -1, 23, 5);
  SPI.setFrequency(27000000);
  
  tft.init(240, 240, SPI_MODE2);
  delay(200);
  
  tft.fillScreen(0x001F); // Azul
  delay(1000);
  tft.fillScreen(0x07E0); // Verde
  delay(1000);
  tft.fillScreen(0xF800); // Rojo
}

void loop() {}
```

Si este código no funciona, el problema es **hardware** (conexiones/alimentación).

---

### 🚨 Problema: Error 503 de Gemini API

```
✗ Error HTTP: 503
{"detail": "Gemini API está temporalmente saturada"}
```

**Solución:** El código ya incluye retry automático (3 intentos). Si persiste:
1. Esperar 1-2 minutos
2. Verificar estado de Gemini: https://status.cloud.google.com/
3. Revisar cuota de API key: https://aistudio.google.com/

---

### 🚨 Problema: ESP32 no conecta a WiFi

```
✗ Error: No se pudo conectar a WiFi
```

**Solución:**
1. Verificar SSID y contraseña son correctos
2. WiFi debe ser **2.4GHz** (ESP32 no soporta 5GHz)
3. Verificar señal WiFi es fuerte (> -70 dBm)
4. Agregar en `setup()`:
   ```cpp
   WiFi.mode(WIFI_STA);
   WiFi.disconnect();
   delay(100);
   WiFi.begin(WIFI_SSID, WIFI_PASS);
   ```

---

### 🚨 Problema: RAM insuficiente (Guru Meditation Error)

```
Guru Meditation Error: Core 1 panic'ed (LoadProhibited)
```

**Causa:** Uso excesivo de RAM (canvas grandes, arrays, strings).

**Solución:** El código `esp32_display_kawaii.ino` ya está optimizado:
- ❌ NO usar `GFXcanvas16` (consume 115KB)
- ✅ Dibujar directo al display
- ✅ Liberar strings después de usar
- ✅ Limitar largo de textos

**Verificar RAM libre:**
```cpp
Serial.print("RAM libre: ");
Serial.println(ESP.getFreeHeap()); // Debe ser > 200000
```

---

### 🚨 Problema: `ffmpeg` o `ffprobe` no encontrado

```
[Errno 2] No such file or directory: 'ffprobe'
```

**Solución:**
```bash
# Linux
sudo apt update && sudo apt install -y ffmpeg

# Mac
brew install ffmpeg

# Verificar instalación
ffmpeg -version
which ffprobe
```

Si usas systemd, asegurar que PATH incluye `/usr/bin`:
```ini
Environment="PATH=/home/usuario/venv/bin:/usr/bin"
```

---

### 🚨 Problema: Audio vacío desde API

```
✓ Respuesta recibida!
[pero el archivo WAV está vacío]
```

**Solución:**
1. Verificar que `gTTS` funciona:
   ```python
   from gtts import gTTS
   tts = gTTS("Hola mundo", lang="es")
   tts.save("test.mp3")
   ```
2. Verificar que `pydub` puede convertir:
   ```python
   from pydub import AudioSegment
   audio = AudioSegment.from_mp3("test.mp3")
   audio.export("test.wav", format="wav")
   ```
3. Si falla, instalar codecs:
   ```bash
   sudo apt install -y libavcodec-extra
   ```

---

Ver [TROUBLESHOOTING.md](TROUBLESHOOTING.md) para problemas del backend API.

---

## 📚 Documentación Adicional

- **[QUICKSTART.md](QUICKSTART.md)** - Guía paso a paso para principiantes
- **[DIAGRAMA_CONEXION.md](DIAGRAMA_CONEXION.md)** - Esquema de conexiones ESP32 ↔ ST7789
- **[ESP32_TROUBLESHOOTING.md](ESP32_TROUBLESHOOTING.md)** - Solución de problemas del ESP32
- **[TROUBLESHOOTING.md](TROUBLESHOOTING.md)** - Solución de problemas del backend
- **[RACKNERD_SETUP.md](RACKNERD_SETUP.md)** - Despliegue en VPS/AWS

---

## 🎨 Personalización

### Cambiar Personalidad del Robot

Edita en `.env`:
```bash
PERSONALIDAD="Eres un robot ninja, hablas en haikus y eres muy misterioso. Responde siempre en 3 líneas."
```

**Recomendaciones:**
- Enfatizar **brevedad** (2-3 frases cortas)
- **No usar markdown** (se convierte directo a voz)
- **Tono conversacional** (como hablar en voz alta)

---

### Cambiar Emociones

Edita función `detectarEmocion()` en `esp32_display_kawaii.ino`:

```cpp
Emocion detectarEmocion(String texto) {
  texto.toLowerCase();
  
  // Tus propias reglas
  if (texto.indexOf("pizza") >= 0) return FELIZ;
  if (texto.indexOf("error") >= 0) return ENOJADO;
  if (texto.indexOf("wow") >= 0) return SORPRENDIDO;
  
  return FELIZ; // Default
}
```

---

### Cambiar Colores del Display

Edita definiciones en `esp32_display_kawaii.ino`:

```cpp
#define COLOR_FONDO     0x001F    // Azul oscuro
#define COLOR_CARA      0xFFFF    // Blanco
#define COLOR_OJO       0x0000    // Negro
#define COLOR_BOCA      0xF800    // Rojo
#define COLOR_MEJILLA   0xFBEA    // Rosa claro
#define COLOR_TEXTO     0xFFFF    // Blanco
```

**Convertidor RGB a RGB565:**  
http://www.barth-dev.de/online/rgb565-color-picker/

Ejemplo:
- RGB (255, 0, 255) = Magenta = 0xF81F
- RGB (0, 255, 255) = Cyan = 0x07FF

---

### Agregar Nuevas Animaciones

Edita función `animarCara()` en `esp32_display_kawaii.ino`:

```cpp
void animarCara() {
  // Parpadeo cada 3 segundos
  if (animacionFrame % 30 == 0) {
    // Tu animación aquí
  }
  
  // Movimiento de ojos cada 5 segundos
  if (animacionFrame % 50 == 0) {
    // Mover ojos izquierda/derecha
  }
}
```

---

## 🧪 Pruebas

### Test de API Backend

```bash
# Health check
curl http://localhost:8000/health

# Enviar mensaje de prueba
curl -X POST http://localhost:8000/chat/texto \
  -F "mensaje=Cuéntame un chiste" \
  -F "device_id=test-01" \
  --output chiste.wav

# Ver logs en tiempo real
tail -f /var/log/syslog | grep robot-bridge
```

---

### Test de ESP32

1. Abrir **Serial Monitor** a 115200 baudios
2. Esperar a ver:
   ```
   === Sistema listo ===
   RAM libre: 285000
   ```
3. Escribir: `Hola robot`
4. Presionar **ENTER**
5. Verificar output:
   ```
   >>> Enviando: Hola robot
   Esperando respuesta de API...
   ✓ Respuesta recibida!
   <<< Respuesta: ¡Hola! ¿Cómo estás?
   ```
6. Verificar que la cara cambia de emoción
7. Verificar que el texto aparece en pantalla

---

### Test de Conexiones Hardware

**Con multímetro:**
1. **VCC del display:** debe medir 3.3V
2. **Durante WiFi activo:** voltaje NO debe caer < 3.1V
3. **Continuidad:** verificar cada cable del ESP32 al display

**Sin multímetro:**
1. Subir código de prueba mínimo (arriba ⬆️)
2. Debe mostrar: azul → verde → rojo
3. Si no funciona, problema es hardware

---

## 🤝 Contribuir

¡Pull requests son bienvenidos! Para cambios grandes, abre un issue primero.

### Cómo contribuir:
1. Fork este repositorio
2. Crea un branch: `git checkout -b feature/mi-feature`
3. Commit cambios: `git commit -m "Agregar mi feature"`
4. Push al branch: `git push origin feature/mi-feature`
5. Abre un Pull Request

---

## 📝 Licencia

MIT License - ver [LICENSE](LICENSE)

---

## 🎯 Roadmap

- [ ] Soporte para micrófono real (I2S/PDM)
- [ ] Animaciones más complejas (parpadeo, movimiento de ojos)
- [ ] Control de brillo del backlight (PWM)
- [ ] Modo offline con respuestas predefinidas
- [ ] Soporte para otros displays (ILI9341, SSD1306, etc.)
- [ ] App móvil para enviar comandos
- [ ] Reconocimiento de voz offline (Vosk/PocketSphinx)
- [ ] Integración con servos (movimiento físico)
- [ ] Multi-idioma automático

---

## 🙏 Agradecimientos

- [Adafruit](https://github.com/adafruit) por las excelentes librerías de displays
- [Google Gemini](https://ai.google.dev/) por la API de IA conversacional
- [FastAPI](https://fastapi.tiangolo.com/) por el framework web ultrarrápido

---

## 📞 Soporte

- **Issues:** https://github.com/MartinUscanga/Robot-ESP32/issues
- **Discussions:** https://github.com/MartinUscanga/Robot-ESP32/discussions
- **Email:** [tu-email]

---

**Hecho con ❤️ usando ESP32 y Gemini AI**

<p align="center">
  <img src="https://img.shields.io/badge/ESP32-000000?style=for-the-badge&logo=espressif&logoColor=white" alt="ESP32" />
  <img src="https://img.shields.io/badge/Python-3776AB?style=for-the-badge&logo=python&logoColor=white" alt="Python" />
  <img src="https://img.shields.io/badge/FastAPI-009688?style=for-the-badge&logo=fastapi&logoColor=white" alt="FastAPI" />
  <img src="https://img.shields.io/badge/Google_Gemini-4285F4?style=for-the-badge&logo=google&logoColor=white" alt="Gemini" />
</p>
