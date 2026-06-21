# 🎤 Solución: Voces de Google Cloud TTS

## Problema Encontrado

Al intentar usar voces Neural2 de México (`es-MX-Neural2-A`), se recibía el error:

```
❌ Error en Google TTS: 400 Voice 'es-MX-Neural2-A' does not exist. Is it misspelled?
```

## ¿Por qué pasa esto?

Las voces **Neural2** y algunas **WaveNet** no están disponibles en todos los proyectos de Google Cloud:

### Voces que Requieren Configuración Adicional:
- ❌ `es-MX-Neural2-A` - Requiere proyecto con facturación
- ❌ `es-MX-Wavenet-A` - No siempre disponible
- ❌ Voces Neural2 en general - Requieren APIs específicas habilitadas

### Voces Garantizadas (Disponibles en Cualquier Proyecto):
- ✅ `es-ES-Standard-A` - Española femenina
- ✅ `es-ES-Standard-B` - Española masculina
- ✅ `es-US-Standard-A` - Latina femenina
- ✅ `es-US-Standard-B` - Latino masculino

---

## ✅ Solución Implementada

Usar voces **Standard** que están garantizadas en cualquier proyecto de Google Cloud, incluso en tier gratuito.

### Configuración Actual (Funcionando):

```env
USE_GOOGLE_TTS=true
TTS_VOICE_NAME=es-ES-Standard-A
TTS_LANGUAGE=es-ES
TTS_SPEAKING_RATE=0.95
TTS_PITCH=1.5
```

**Resultado:**
- ✅ Voz femenina española clara y natural
- ✅ Mucho mejor que gTTS (voz robótica)
- ✅ Funciona en cualquier proyecto de Google Cloud
- ✅ No requiere facturación activada

---

## 🔍 Cómo Verificar Voces Disponibles

Si quieres saber qué voces están disponibles en tu proyecto:

```bash
cd /home/robot/Robot-ESP32
source venv/bin/activate
python3 << 'EOF'
from google.cloud import texttospeech
import os

os.environ['GOOGLE_APPLICATION_CREDENTIALS'] = '/home/robot/Robot-ESP32/google-tts-credentials.json'
client = texttospeech.TextToSpeechClient()

print("\n🎤 VOCES DISPONIBLES:\n")

for lang_code in ['es-ES', 'es-US', 'es-MX']:
    print(f"\n{'='*50}")
    print(f"  {lang_code}")
    print(f"{'='*50}")
    
    try:
        voices = client.list_voices(language_code=lang_code)
        
        if not voices.voices:
            print(f"  ❌ No hay voces disponibles")
            continue
            
        for voice in voices.voices:
            gender = "👩 Femenina" if "FEMALE" in str(voice.ssml_gender) else "👨 Masculina"
            print(f"  {voice.name}")
            print(f"    {gender}")
            print()
    except Exception as e:
        print(f"  ❌ Error: {e}\n")
EOF
```

---

## 🎛️ Mejorar la Calidad de Voz

### 1. Probar Otras Voces Standard

Edita `.env` y prueba:

```env
# Voz española femenina (actual)
TTS_VOICE_NAME=es-ES-Standard-A

# Voz latina estadounidense femenina
TTS_VOICE_NAME=es-US-Standard-A
```

### 2. Ajustar Tono y Velocidad

```env
# Hablar más lento = más cálido
TTS_SPEAKING_RATE=0.90  # (0.85-1.0 recomendado)

# Pitch más alto = más femenino
TTS_PITCH=2.0  # (1.5-3.0 recomendado)
```

### 3. Activar Facturación para Voces Premium

Si quieres acceso a voces Neural2 (máxima calidad):

1. Ve a https://console.cloud.google.com/billing
2. Vincula una tarjeta de crédito a tu proyecto "Gemini Project"
3. Habilita la API de Text-to-Speech
4. Prueba voces como:
   - `es-ES-Neural2-A` - Ultra natural, profesional
   - `es-ES-Wavenet-C` - Muy natural, expresiva

**Costo aproximado:**
- ~$4 USD por 1 millón de caracteres (Neural2)
- ~$16 USD por 1 millón de caracteres (Wavenet)
- Con uso normal: menos de $1 al mes

---

## 🔄 Después de Cambiar Configuración

Siempre reinicia la API:

```bash
pkill -f uvicorn
source venv/bin/activate
nohup uvicorn main:app --host 0.0.0.0 --port 8000 --env-file .env > api.log 2>&1 &
```

Verifica los logs:

```bash
tail -20 api.log
```

---

## 📊 Comparación de Voces

| Tipo | Calidad | Costo | Disponibilidad |
|------|---------|-------|----------------|
| **Standard** | ⭐⭐⭐ Buena | Gratis | ✅ Todas los proyectos |
| **WaveNet** | ⭐⭐⭐⭐ Muy buena | Bajo | ⚠️ Requiere configuración |
| **Neural2** | ⭐⭐⭐⭐⭐ Excelente | Muy bajo | ⚠️ Requiere facturación |
| **gTTS** (antiguo) | ⭐ Robótica | Gratis | ✅ Siempre disponible |

---

## ✅ Recomendación

**Para desarrollo/producción básica:**
- Usar `es-ES-Standard-A` (actual) ✅
- Funciona en cualquier proyecto
- Calidad muy superior a gTTS
- Costo: $0

**Para producción premium:**
- Activar facturación
- Usar `es-ES-Neural2-A` o `es-ES-Wavenet-C`
- Calidad ultra natural
- Costo: ~$0.50-$1 USD/mes con uso normal

---

## 🐛 Troubleshooting

### Error: "Voice does not exist"
- ✅ Verifica que usas una voz Standard: `es-ES-Standard-A`
- ✅ Verifica que `TTS_LANGUAGE` coincide con la región de la voz
- ✅ Ejecuta el script de verificación de voces disponibles

### Error: "Permission denied"
- ✅ Verifica que el archivo de credenciales existe: `ls -la google-tts-credentials.json`
- ✅ Verifica la ruta en `.env`: `GOOGLE_APPLICATION_CREDENTIALS`

### Sigue usando gTTS (voz robótica)
- ✅ Verifica que `USE_GOOGLE_TTS=true` en `.env`
- ✅ Reinicia la API después de cambiar `.env`
- ✅ Revisa los logs: `tail -30 api.log`

### La voz no suena femenina
- ✅ Aumenta el pitch: `TTS_PITCH=2.0` o `TTS_PITCH=2.5`
- ✅ Verifica que usas una voz con `-A` o `-C` (generalmente femeninas)
- ✅ Prueba diferentes voces Standard

---

## 📚 Referencias

- **Google Cloud TTS Voces:** https://cloud.google.com/text-to-speech/docs/voices
- **Pricing:** https://cloud.google.com/text-to-speech/pricing
- **API Reference:** https://cloud.google.com/text-to-speech/docs/reference/rest

---

**Última actualización:** Junio 21, 2026  
**Estado:** ✅ Funcionando con `es-ES-Standard-A`
