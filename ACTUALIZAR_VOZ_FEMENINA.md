# 🎤 Actualizar a Voz Femenina Cálida

## Problema Resuelto
La voz anterior era masculina (`es-MX-Neural2-B`) y el código tenía un parámetro incompatible (`ssml_gender`).

## Cambios Realizados
1. ✅ Cambiado a voz femenina: `es-MX-Neural2-A`
2. ✅ Eliminado parámetro `ssml_gender` (incompatible con Neural2)
3. ✅ Ajustado `pitch=1.5` para voz más cálida y femenina
4. ✅ Ajustado `speaking_rate=0.95` para hablar un poco más lento (más cálido)

---

## Pasos para Actualizar en el VPS

### 1️⃣ Conectar al VPS y ir al directorio
```bash
ssh robot@18.221.246.87
cd /home/robot/Robot-ESP32
```

### 2️⃣ Actualizar código desde GitHub
```bash
git pull origin main
```

### 3️⃣ Verificar el archivo .env
Asegúrate de que tu archivo `.env` tenga estas líneas:
```bash
nano .env
```

**Contenido necesario:**
```env
USE_GOOGLE_TTS=true
TTS_VOICE_NAME=es-MX-Neural2-A
TTS_LANGUAGE=es-MX
TTS_SPEAKING_RATE=0.95
TTS_PITCH=1.5
GOOGLE_APPLICATION_CREDENTIALS=/home/robot/Robot-ESP32/google-tts-credentials.json
```

Guarda con `Ctrl+O`, Enter, `Ctrl+X`

### 4️⃣ Reiniciar la API
Detener el proceso actual:
```bash
pkill -f "python.*main.py" || pkill -f uvicorn
```

Iniciar de nuevo:
```bash
source venv/bin/activate
nohup python3 main.py > api.log 2>&1 &
```

O si usas uvicorn:
```bash
nohup uvicorn main:app --host 0.0.0.0 --port 8000 > api.log 2>&1 &
```

### 5️⃣ Verificar que funciona
Revisar los logs:
```bash
tail -f api.log
```

Probar desde la interfaz web:
- Abre http://18.221.246.87
- Envía un mensaje de prueba
- Escucha la respuesta - ¡debería ser con voz femenina cálida! 🎤

---

## 🔧 Otras Voces Disponibles

Si quieres probar otras voces femeninas, edita `TTS_VOICE_NAME` en `.env`:

### Voces Femeninas Recomendadas
- `es-MX-Neural2-A` ⭐ Mexicana, cálida, natural **(ACTUAL)**
- `es-ES-Neural2-A` - Española, clara, profesional
- `es-ES-Neural2-C` - Española, joven, expresiva
- `es-ES-Neural2-D` - Española, dulce, amigable
- `es-US-Neural2-A` - Estadounidense, neutra, clara

### Ajustar Calidez
Edita estos valores en `.env` para cambiar el tono:

```env
# Más lento = más cálido (0.8-1.2 rango recomendado)
TTS_SPEAKING_RATE=0.95

# Pitch más alto = más femenino (0.0-3.0 rango recomendado)
TTS_PITCH=1.5
```

Después de cambiar, reinicia la API (paso 4).

---

## ✅ Verificación Final

**Señales de éxito:**
- ✅ No hay errores en `api.log`
- ✅ La voz suena femenina y natural
- ✅ No se escucha robótica (como gTTS)
- ✅ La interfaz web dice "Conectado"

**Si algo falla:**
1. Revisa que el archivo de credenciales existe: `ls -la google-tts-credentials.json`
2. Revisa los logs: `tail -50 api.log`
3. Verifica que la API responde: `curl http://localhost:8000/health`

---

## 📝 Notas Técnicas

### ¿Por qué se eliminó `ssml_gender`?
Las voces Neural2 de Google Cloud TTS no usan este parámetro porque ya tienen un género definido en el nombre de la voz. Usarlo causa errores.

### ¿Por qué `pitch=1.5`?
Un pitch más alto hace que la voz suene más femenina y cálida. El rango recomendado para voz femenina es 1.0-3.0.

### ¿Por qué `speaking_rate=0.95`?
Hablar ligeramente más lento hace que la voz suene más natural y cálida, menos robótica.
