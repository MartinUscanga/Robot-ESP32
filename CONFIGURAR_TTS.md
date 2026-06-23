# 🎤 Configurar Text-to-Speech (TTS)

Guía para cambiar de gTTS a Google Cloud TTS (voces más naturales).

---

## ⚡ **Instalación Rápida**

### **1. Instalar dependencia:**

```bash
pip install google-cloud-texttospeech
```

### **2. Configurar credenciales de Google Cloud:**

#### **a) Crear proyecto en Google Cloud:**

1. Ve a: https://console.cloud.google.com
2. Crear nuevo proyecto: "Robot-ESP32-TTS"
3. Habilitar API: **Cloud Text-to-Speech API**

#### **b) Crear Service Account:**

1. Ve a: **IAM & Admin** → **Service Accounts**
2. Click **Create Service Account**
3. Nombre: `robot-tts`
4. Role: `Cloud Text-to-Speech User`
5. Click **Create Key** → JSON
6. Descargar el archivo JSON

#### **c) Configurar credenciales en el VPS:**

```bash
# Subir el archivo JSON a tu VPS
scp service-account-key.json usuario@tu-vps:/home/usuario/Robot-ESP32/

# En tu VPS
cd ~/Robot-ESP32
export GOOGLE_APPLICATION_CREDENTIALS="service-account-key.json"

# Hacer permanente (agregar a .bashrc o .env)
echo 'export GOOGLE_APPLICATION_CREDENTIALS="/home/usuario/Robot-ESP32/service-account-key.json"' >> ~/.bashrc
```

### **3. Activar Google TTS en tu proyecto:**

```bash
nano ~/Robot-ESP32/.env
```

Agregar:

```bash
USE_GOOGLE_TTS=true
TTS_VOICE_NAME=es-MX-Neural2-B
TTS_LANGUAGE=es-MX
TTS_SPEAKING_RATE=1.0
TTS_PITCH=0.0
```

### **4. Reiniciar API:**

```bash
pkill -f uvicorn
cd ~/Robot-ESP32
source venv/bin/activate
uvicorn main:app --host 0.0.0.0 --port 8000 --reload
```

---

## 🎙️ **Voces Disponibles**

### **Español México (Recomendado):**

| Voz | Género | Descripción |
|-----|--------|-------------|
| `es-MX-Neural2-A` | Femenina | Natural, cálida |
| `es-MX-Neural2-B` | Masculina | Profesional, clara (⭐ Recomendada) |

### **Español España:**

| Voz | Género | Descripción |
|-----|--------|-------------|
| `es-ES-Neural2-A` | Femenina | Elegante, profesional |
| `es-ES-Neural2-B` | Masculina | Profunda, autoritaria |
| `es-ES-Neural2-C` | Femenina | Joven, amigable |
| `es-ES-Neural2-D` | Femenina | Madura, confiable |
| `es-ES-Neural2-E` | Masculina | Versátil, expresiva |

### **Español Estados Unidos:**

| Voz | Género | Descripción |
|-----|--------|-------------|
| `es-US-Neural2-A` | Femenina | Latina, energética |
| `es-US-Neural2-B` | Masculina | Profesional, clara |
| `es-US-Neural2-C` | Masculina | Amigable, casual |

---

## ⚙️ **Personalización Avanzada**

### **Velocidad de habla:**

```bash
TTS_SPEAKING_RATE=1.2  # Más rápido (0.25 a 4.0)
TTS_SPEAKING_RATE=0.8  # Más lento
```

### **Tono de voz:**

```bash
TTS_PITCH=5.0   # Más agudo (-20 a 20)
TTS_PITCH=-5.0  # Más grave
```

### **Ejemplo - Voz de JARVIS:**

```bash
USE_GOOGLE_TTS=true
TTS_VOICE_NAME=es-ES-Neural2-B  # Voz masculina España (elegante)
TTS_LANGUAGE=es-ES
TTS_SPEAKING_RATE=0.95  # Ligeramente más lento (más elegante)
TTS_PITCH=-2.0  # Un poco más grave (más autoritario)
```

### **Ejemplo - Voz de FRIDAY (más casual):**

```bash
USE_GOOGLE_TTS=true
TTS_VOICE_NAME=es-US-Neural2-A  # Voz femenina US
TTS_LANGUAGE=es-US
TTS_SPEAKING_RATE=1.1  # Un poco más rápido
TTS_PITCH=2.0  # Un poco más agudo
```

---

## 🧪 **Probar Voces**

```bash
cd ~/Robot-ESP32
python3 tts_google.py
```

Esto generará un archivo `test_output.wav` con una voz de prueba.

---

## 💰 **Costos**

### **Google Cloud TTS:**

- 🆓 **GRATIS**: 1 millón de caracteres/mes (Standard)
- 🆓 **GRATIS**: 100,000 caracteres/mes (Neural/WaveNet)
- Después: $4 USD por millón de caracteres

### **Estimación de uso:**

- 1 respuesta promedio: ~100 caracteres
- 1,000 respuestas/mes = 100,000 caracteres
- **Con Neural = 100% GRATIS** (dentro del tier gratuito)

---

## 🔄 **Volver a gTTS**

Si quieres volver al TTS anterior:

```bash
nano ~/Robot-ESP32/.env
```

Cambiar:

```bash
USE_GOOGLE_TTS=false
```

Reiniciar API.

---

## 🐛 **Solución de Problemas**

### **Error: "Could not automatically determine credentials"**

```bash
# Verificar que la variable está configurada
echo $GOOGLE_APPLICATION_CREDENTIALS

# Debe mostrar la ruta al JSON
# Si no, ejecutar:
export GOOGLE_APPLICATION_CREDENTIALS="/ruta/completa/al/archivo.json"
```

### **Error: "Permission denied"**

Tu Service Account no tiene permisos. En Google Cloud Console:
1. IAM & Admin → Service Accounts
2. Selecciona tu cuenta
3. Agrega rol: `Cloud Text-to-Speech User`

### **Error: "API not enabled"**

```bash
# Ve a Google Cloud Console
# APIs & Services → Library
# Busca: "Cloud Text-to-Speech API"
# Click "Enable"
```

---

## 🎯 **Recomendación Final**

Para un asistente tipo JARVIS:

```bash
USE_GOOGLE_TTS=true
TTS_VOICE_NAME=es-ES-Neural2-B
TTS_LANGUAGE=es-ES
TTS_SPEAKING_RATE=0.95
TTS_PITCH=-2.0
```

Esto da una voz:
- ✅ Masculina
- ✅ Elegante y sofisticada
- ✅ Ligeramente grave
- ✅ Ritmo pausado y controlado

---

¡Disfruta de tu nuevo TTS! 🎤✨
