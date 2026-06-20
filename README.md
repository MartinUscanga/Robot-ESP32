# Puente ESP32 <-> Gemini

API ligera (FastAPI) que sirve de intermediario entre tu robot ESP32 y la
API de Gemini. Recibe audio (o texto, para pruebas), genera una respuesta
con Gemini, la convierte a voz, y la regresa lista para reproducirse.

Diseñada para correr en un VPS pequeño (1GB de RAM es suficiente, porque
el trabajo pesado lo hace Gemini en la nube, no tu servidor).

```
ESP32 (mic) --audio--> [Esta API] --audio--> Gemini --texto--> [Esta API] --TTS--> WAV --audio--> ESP32 (bocina)
```

## 1. Instalación local (para probar antes de subir al VPS)

```bash
cd esp32-ia-bridge
python3 -m venv venv
source venv/bin/activate          # En Windows: venv\Scripts\activate
pip install -r requirements.txt
```

**Importante:** necesitas `ffmpeg` instalado en el sistema (lo usa `pydub`
para convertir el audio a WAV). Instálalo así:

```bash
# Ubuntu/Debian (igual en el VPS)
sudo apt update && sudo apt install -y ffmpeg

# Mac
brew install ffmpeg
```

## 2. Configurar tu API key de Gemini

1. **Obtén tu API key gratis** en: https://aistudio.google.com/apikey
2. **Copia el archivo de ejemplo**:
   ```bash
   cp .env.example .env
   ```
3. **Edita `.env` y configura tus credenciales**:
   ```bash
   nano .env  # o usa tu editor favorito
   ```

### Variables de entorno disponibles:

| Variable | Requerido | Default | Descripción |
|----------|-----------|---------|-------------|
| `GEMINI_API_KEY` | ✅ Sí | - | Tu API key de Gemini |
| `GEMINI_MODEL` | ⚪ No | `gemini-1.5-flash` | Modelo de IA a usar |
| `TTS_LANG` | ⚪ No | `es` | Idioma del TTS (es, en, fr, etc.) |
| `API_PORT` | ⚪ No | `8000` | Puerto del servidor |
| `API_HOST` | ⚪ No | `0.0.0.0` | Host del servidor |
| `LOG_LEVEL` | ⚪ No | `INFO` | Nivel de logs (DEBUG, INFO, WARNING, ERROR) |
| `MAX_HISTORIAL` | ⚪ No | `12` | Mensajes a recordar por conversación |
| `GEMINI_TIMEOUT` | ⚪ No | `30` | Timeout para Gemini API (segundos) |
| `TTS_TIMEOUT` | ⚪ No | `15` | Timeout para TTS (segundos) |
| `PERSONALIDAD` | ⚪ No | *(ver código)* | Personalidad del asistente |

**Ejemplo de configuración mínima** (`.env`):
```env
GEMINI_API_KEY=AIzaSyAbC123XYZ_tu_api_key_real_aqui
```

**Ejemplo de configuración avanzada** (`.env`):
```env
GEMINI_API_KEY=AIzaSyAbC123XYZ_tu_api_key_real_aqui
GEMINI_MODEL=gemini-1.5-pro
TTS_LANG=es
LOG_LEVEL=DEBUG
MAX_HISTORIAL=20
PERSONALIDAD="Eres un robot asistente formal y profesional que responde de forma concisa."
```

## 3. Levantar el servidor (modo de prueba, en tu computadora)

```bash
uvicorn main:app --reload --host 0.0.0.0 --port 8000
```

Si todo está bien, verás algo como:
```
INFO:     Uvicorn running on http://0.0.0.0:8000
```

## 4. Probar la API (sin necesitar el ESP32)

En otra terminal, con el entorno activado:

```bash
# Verificar que el servidor está vivo
python test_client.py --salud

# Probar con texto (lo más fácil para empezar)
python test_client.py --texto "Hola robot, ¿cómo estás?"

# Probar con un archivo de audio real (simulando al ESP32)
python test_client.py --audio mi_grabacion.wav
```

Cada prueba va a:
1. Mostrarte en consola lo que respondió Gemini (texto)
2. Guardar un archivo `respuesta.wav` con el audio de la respuesta — ábrelo
   con cualquier reproductor para escuchar la voz del robot

También puedes probar manualmente con `curl`:

```bash
curl -X POST http://localhost:8000/chat/texto \
  -F "mensaje=Cuéntame un chiste corto" \
  -F "device_id=prueba" \
  -o respuesta.wav -D -
```

O abrir `http://localhost:8000/docs` en tu navegador — FastAPI genera
automáticamente una interfaz interactiva para probar todos los endpoints.

## 5. Endpoints disponibles

| Endpoint | Método | Para qué |
|---|---|---|
| `/health` | GET | Verificar que el servidor responde |
| `/chat` | POST | Endpoint real: recibe audio (`file`), regresa audio WAV |
| `/chat/texto` | POST | Prueba: recibe texto (`mensaje`), regresa audio WAV |
| `/reset` | POST | Borra la memoria de conversación de un `device_id` |

El campo `device_id` te permite mantener conversaciones separadas si
algún día tienes más de un robot, o quieres separar sesiones.

## 6. Desplegar en tu VPS de 1GB

```bash
# En el VPS
sudo apt update && sudo apt install -y python3-venv ffmpeg

git clone <tu-repositorio>   # o sube los archivos por scp/sftp
cd esp32-ia-bridge
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
cp .env.example .env
nano .env   # pega tu API key real
```

**Correrlo de forma permanente** (para que sobreviva aunque cierres la sesión SSH):

```bash
# Instalar como servicio con systemd
sudo nano /etc/systemd/system/robot-bridge.service
```

Contenido del archivo:
```ini
[Unit]
Description=Puente ESP32 - Gemini
After=network.target

[Service]
User=tu_usuario
WorkingDirectory=/ruta/a/esp32-ia-bridge
ExecStart=/ruta/a/esp32-ia-bridge/venv/bin/uvicorn main:app --host 0.0.0.0 --port 8000 --workers 1
Restart=always

[Install]
WantedBy=multi-user.target
```

Luego:
```bash
sudo systemctl daemon-reload
sudo systemctl enable robot-bridge
sudo systemctl start robot-bridge
sudo systemctl status robot-bridge   # para verificar que esté corriendo
```

**Recomendación de seguridad:** usa siempre **HTTPS** (con Nginx +
Certbot, por ejemplo) si vas a exponer la API a internet, para que tu
API key y el audio del micrófono no viajen sin cifrar. Y considera
agregar un token simple de autenticación entre el ESP32 y tu API
(un header fijo que solo tú conoces) para que nadie más use tu servidor.

## 7. Cómo lo usará el ESP32 (referencia para cuando llegues ahí)

El ESP32 hará una petición HTTP POST tipo `multipart/form-data` al
endpoint `/chat`, mandando el archivo de audio grabado del micrófono
(formato WAV, 16kHz mono recomendado). La respuesta llega como audio
WAV PCM en el cuerpo de la respuesta — ideal porque el ESP32 NO necesita
decodificar MP3 (que sería pesado), solo transmite los samples PCM
directo al I2S/MAX98357A.

Cuando lleguemos a esa parte, el código del ESP32 usará la librería
`HTTPClient` de Arduino para mandar el archivo y leer la respuesta en
streaming hacia el I2S.

## 8. Notas sobre el modelo usado

Este proyecto usa `gemini-1.5-flash` por default (configurable en
`.env`), que entiende audio directamente — no necesitas un paso
separado de "Speech-to-Text". Gemini transcribe y responde en un solo
paso. La conversión de texto a voz (TTS) se hace con `gTTS` (gratis,
sin necesidad de otra API key) — si más adelante quieres mejor calidad
de voz, se puede migrar a las voces nativas de Gemini o a Google Cloud
TTS sin cambiar el resto de la arquitectura.

## 9. Personalización del asistente

Puedes cambiar la personalidad del robot de dos formas:

**Opción 1: Editar `.env`** (recomendado):
```env
PERSONALIDAD="Eres un robot pirata que habla con acento caribeño y termina cada frase con ¡Arrr!"
```

**Opción 2: Editar `main.py`** directamente:
```python
PERSONALIDAD = os.getenv(
    "PERSONALIDAD",
    "Tu nueva personalidad aquí..."
)
```

Las instrucciones de personalidad deben enfatizar:
- **Brevedad** (2-3 frases cortas)
- **No usar markdown** (la respuesta se convierte directamente a voz)
- **Tono conversacional** (como hablar en voz alta)

## 10. Troubleshooting (solución de problemas comunes)

### ❌ Error: "Falta GEMINI_API_KEY"
**Solución:** Verifica que existe el archivo `.env` y que contiene tu API key válida.

### ❌ Error: "ffmpeg no encontrado"
**Solución:** Instala ffmpeg:
```bash
sudo apt install ffmpeg  # Linux
brew install ffmpeg      # Mac
```

### ❌ Error 500 al llamar `/chat`
**Solución:** Revisa los logs del servidor. Causas comunes:
- Formato de audio no soportado (usa WAV o MP3)
- API key de Gemini inválida o cuota excedida
- Timeout en la respuesta de Gemini (aumenta `GEMINI_TIMEOUT`)

### ❌ El audio de respuesta suena extraño
**Solución:** Verifica que el ESP32 esté configurado para reproducir WAV PCM 16kHz mono.

### 🔍 Ver más detalles de ejecución
Cambia el nivel de logs en `.env`:
```env
LOG_LEVEL=DEBUG
```
