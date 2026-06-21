# 🎤 Actualizar a Voz Femenina Cálida con Google Cloud TTS

## ✅ Estado Actual (Funcionando)
- **Voz configurada:** `es-ES-Standard-A` (Femenina española, calidad estándar)
- **Motor TTS:** Google Cloud Text-to-Speech ✅
- **Ya NO usa gTTS** (voz robótica antigua)

## Problema Resuelto
1. La voz anterior era masculina (`es-MX-Neural2-B`)
2. El código tenía un parámetro incompatible (`ssml_gender`)
3. Las voces Neural2 de México no estaban disponibles en el proyecto
4. **Solución:** Usar voces españolas estándar que están disponibles en todos los proyectos

## Cambios Realizados
1. ✅ Cambiado a voz femenina española: `es-ES-Standard-A`
2. ✅ Eliminado parámetro `ssml_gender` (incompatible con Neural2)
3. ✅ Ajustado `pitch=1.5` para voz más cálida y femenina
4. ✅ Ajustado `speaking_rate=0.95` para hablar un poco más lento (más cálido)
5. ✅ Configurado fallback a gTTS si Google Cloud TTS falla

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
TTS_VOICE_NAME=es-ES-Standard-A
TTS_LANGUAGE=es-ES
TTS_SPEAKING_RATE=0.95
TTS_PITCH=1.5
GOOGLE_APPLICATION_CREDENTIALS=/home/robot/Robot-ESP32/google-tts-credentials.json
```

**NOTA:** Usamos `es-ES-Standard-A` porque las voces Neural2 de México (`es-MX-Neural2-A`) no están disponibles en todos los proyectos de Google Cloud. Las voces Standard están garantizadas en cualquier proyecto.

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

### Voces Españolas Femeninas (Garantizadas)
- `es-ES-Standard-A` ⭐ **ACTUAL** - Española, clara, buena calidad
- `es-ES-Wavenet-C` - Española, muy natural, mejor calidad
- `es-ES-Wavenet-D` - Española, dulce, cálida
- `es-US-Standard-A` - Latina (USA), neutra

### Voces Neural2 (Máxima Calidad - requiere proyecto con facturación activa)
- `es-ES-Neural2-A` - Española, profesional, ultra natural
- `es-ES-Neural2-C` - Española, joven, expresiva
- `es-ES-Neural2-D` - Española, dulce, amigable

**IMPORTANTE:** Si pruebas una voz y da error "Voice does not exist", vuelve a `es-ES-Standard-A` que está garantizada.

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
Las voces Neural2 y modernas de Google Cloud TTS no usan este parámetro porque ya tienen un género definido en el nombre de la voz. Usarlo causa errores.

### ¿Por qué usar `es-ES-Standard-A` en lugar de `es-MX-Neural2-A`?
Las voces Neural2 de México no están disponibles por defecto en todos los proyectos de Google Cloud. Algunas voces requieren:
1. Tener facturación activada en el proyecto
2. Habilitar APIs específicas
3. Estar en ciertas regiones

Las voces **Standard** están garantizadas en cualquier proyecto gratuito, por eso es la configuración recomendada para máxima compatibilidad.

### ¿Por qué `pitch=1.5`?
Un pitch más alto hace que la voz suene más femenina y cálida. El rango recomendado para voz femenina es 1.0-3.0.

### ¿Por qué `speaking_rate=0.95`?
Hablar ligeramente más lento hace que la voz suene más natural y cálida, menos robótica.
