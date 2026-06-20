# 🚀 Inicio Rápido - Robot ESP32

Esta guía te llevará de cero a tener la API funcionando en **menos de 5 minutos**.

---

## ✅ Requisitos previos

- **Python 3.8+** instalado
- **ffmpeg** instalado en tu sistema
- **API key de Gemini** (gratis en https://aistudio.google.com/apikey)

---

## 📦 Paso 1: Instalar dependencias

```bash
# Clonar o navegar al directorio del proyecto
cd Robot-ESP32

# Crear entorno virtual
python3 -m venv venv

# Activar entorno virtual
source venv/bin/activate  # Linux/Mac
# venv\Scripts\activate   # Windows

# Instalar dependencias
pip install -r requirements.txt
```

**⚠️ Instalar ffmpeg:**
```bash
# Ubuntu/Debian
sudo apt update && sudo apt install -y ffmpeg

# Mac
brew install ffmpeg

# Windows
# Descargar desde: https://ffmpeg.org/download.html
```

---

## 🔑 Paso 2: Configurar API Key

```bash
# Copiar el archivo de ejemplo
cp .env.example .env

# Editar el archivo .env
nano .env  # o usa tu editor favorito
```

**Pega tu API key:**
```env
GEMINI_API_KEY=TU_API_KEY_AQUI
```

*Consigue tu API key gratis en: https://aistudio.google.com/apikey*

---

## 🎮 Paso 3: Iniciar el servidor

```bash
uvicorn main:app --reload --host 0.0.0.0 --port 8000
```

**✅ Si ves esto, está funcionando:**
```
INFO:     Uvicorn running on http://0.0.0.0:8000
INFO:     Application startup complete.
```

---

## 🧪 Paso 4: Probar la API

**Opción A: Verificar estado**
```bash
curl http://localhost:8000/health
```

**Opción B: Prueba con texto (más fácil)**
```bash
python test_client.py --texto "Hola robot, ¿cómo estás?"
```

**Opción C: Prueba con audio**
```bash
python test_client.py --audio tu_archivo.wav
```

**Opción D: Interfaz web interactiva**

Abre en tu navegador: http://localhost:8000/docs

---

## 📊 Endpoints disponibles

| Endpoint | Método | Descripción |
|----------|--------|-------------|
| `/health` | GET | Verifica que el servidor esté vivo |
| `/chat` | POST | Recibe audio, devuelve audio WAV |
| `/chat/texto` | POST | Recibe texto, devuelve audio WAV (para pruebas) |
| `/reset` | POST | Borra el historial de conversación |

---

## 🎨 Personalización rápida

**Cambiar el idioma del TTS:**
```env
TTS_LANG=en  # inglés
TTS_LANG=fr  # francés
```

**Cambiar el modelo de Gemini:**
```env
GEMINI_MODEL=gemini-1.5-pro  # más inteligente pero más lento
```

**Cambiar la personalidad:**
```env
PERSONALIDAD="Eres un robot científico que habla de forma técnica y precisa"
```

---

## 🐛 Problemas comunes

### ❌ "Falta GEMINI_API_KEY"
**Solución:** Verifica que `.env` existe y contiene tu API key.

### ❌ "ffmpeg no encontrado"
**Solución:** Instala ffmpeg (ver Paso 1).

### ❌ Error 500 en `/chat`
**Solución:** Revisa los logs. Posibles causas:
- API key inválida
- Cuota de Gemini excedida
- Formato de audio no soportado

### 🔍 Ver logs detallados
Agrega en `.env`:
```env
LOG_LEVEL=DEBUG
```

---

## 🚀 Siguiente paso: Despliegue en producción

Lee la [guía completa en README.md](README.md#6-desplegar-en-tu-vps-de-1gb) para aprender a:
- Desplegar en un VPS
- Configurar como servicio systemd
- Usar HTTPS con Nginx
- Agregar autenticación

---

## 📚 Recursos útiles

- **Documentación de Gemini:** https://ai.google.dev/gemini-api/docs
- **Consola de API:** https://aistudio.google.com
- **gTTS (Text-to-Speech):** https://gtts.readthedocs.io
- **FastAPI:** https://fastapi.tiangolo.com

---

## 💡 Tips

1. **Prueba primero con `/chat/texto`** antes de usar audio real
2. **Usa el endpoint `/health`** para monitoreo
3. **El historial se borra al reiniciar** el servidor (es en RAM)
4. **Cada `device_id` tiene su propia conversación** independiente
5. **El audio de respuesta es WAV PCM 16kHz mono** (fácil para ESP32)

---

¿Listo? ¡Ahora tu API está funcionando! 🎉
