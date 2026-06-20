# 🔧 Guía de Solución de Problemas

Esta guía te ayuda a resolver los errores más comunes al usar la API del Robot ESP32.

---

## ❌ Error: "User location is not supported for the API use"

### **Descripción del error:**
```
400 FAILED_PRECONDITION: User location is not supported for the API use.
```

### **Causa:**
Google Gemini API tiene restricciones geográficas y no está disponible en todas las regiones del mundo.

### **Solución 1: Usar un VPN (Recomendado)**

#### **VPNs gratuitos:**
- **ProtonVPN:** https://protonvpn.com (sin límite de tiempo, 3 países gratis)
- **Windscribe:** https://windscribe.com (10GB/mes gratis)
- **TunnelBear:** https://www.tunnelbear.com (500MB/mes gratis)

#### **Países soportados por Gemini API:**
- 🇺🇸 Estados Unidos
- 🇬🇧 Reino Unido  
- 🇨🇦 Canadá
- 🇩🇪 Alemania
- 🇫🇷 Francia
- 🇯🇵 Japón
- 🇦🇺 Australia
- [Lista completa](https://ai.google.dev/available_regions)

#### **Pasos:**
1. Instala y activa el VPN
2. Conéctate a un país soportado (ej: Estados Unidos)
3. Verifica tu IP: `curl ifconfig.me`
4. Reinicia el servidor:
   ```bash
   # Detén el servidor (Ctrl+C)
   uvicorn main:app --reload
   ```
5. Prueba de nuevo:
   ```bash
   python test_client.py --texto "Hola robot"
   ```

---

### **Solución 2: Usar un Proxy HTTP**

Si tienes acceso a un proxy en región soportada:

#### **Configurar en `.env`:**
```env
HTTP_PROXY=http://tu-proxy.com:8080
HTTPS_PROXY=http://tu-proxy.com:8080
```

O con autenticación:
```env
HTTP_PROXY=http://usuario:password@tu-proxy.com:8080
HTTPS_PROXY=http://usuario:password@tu-proxy.com:8080
```

#### **Servicios de proxy gratuitos/pagos:**
- **Webshare:** https://www.webshare.io (10 proxies gratis)
- **ProxyScrape:** https://proxyscrape.com
- **BrightData:** https://brightdata.com (trial disponible)

---

### **Solución 3: Desplegar en un VPS en región soportada**

Despliega el servidor en un VPS ubicado en región soportada:

#### **Proveedores recomendados:**
- **DigitalOcean:** Droplet en Nueva York/San Francisco (desde $6/mes)
- **Linode:** VPS en USA/Europa (desde $5/mes)
- **Vultr:** Cloud Compute en múltiples regiones (desde $3.50/mes)
- **AWS:** EC2 t2.micro (capa gratuita por 12 meses)
- **Google Cloud:** Compute Engine (créditos gratis $300)

#### **Pasos:**
```bash
# En tu VPS (región USA/Europa)
git clone https://github.com/TuUsuario/Robot-ESP32.git
cd Robot-ESP32
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
cp .env.example .env
nano .env  # Configura tu GEMINI_API_KEY

# Instalar ffmpeg
sudo apt update && sudo apt install -y ffmpeg

# Iniciar servidor
uvicorn main:app --host 0.0.0.0 --port 8000
```

Luego desde tu máquina local:
```bash
# Prueba remota
curl http://TU_VPS_IP:8000/health
```

---

## ❌ Error: "Falta GEMINI_API_KEY"

### **Descripción:**
```
RuntimeError: ❌ Falta GEMINI_API_KEY
```

### **Causa:**
No existe el archivo `.env` o no contiene la API key.

### **Solución:**
```bash
# 1. Crear .env desde el ejemplo
cp .env.example .env

# 2. Editar y agregar tu API key
nano .env

# Pega esto:
GEMINI_API_KEY=TU_API_KEY_AQUI

# 3. Obtén tu API key en:
# https://aistudio.google.com/apikey
```

---

## ❌ Error: "ffmpeg no encontrado"

### **Descripción:**
```
FileNotFoundError: ffmpeg not found
```

### **Causa:**
`pydub` requiere `ffmpeg` instalado en el sistema para procesar audio.

### **Solución:**

#### **Linux (Ubuntu/Debian):**
```bash
sudo apt update
sudo apt install -y ffmpeg
```

#### **Mac:**
```bash
brew install ffmpeg
```

#### **Windows:**
1. Descarga ffmpeg: https://ffmpeg.org/download.html
2. Extrae el ZIP
3. Agrega `bin/` al PATH del sistema
4. Reinicia la terminal

#### **Verificar instalación:**
```bash
ffmpeg -version
```

---

## ❌ Error 500: Internal Server Error

### **Posibles causas y soluciones:**

#### **1. API key inválida o cuota excedida**
```bash
# Verifica tu API key en:
https://aistudio.google.com/apikey

# Revisa los logs del servidor
# Busca mensajes como:
# "API key not valid" o "Quota exceeded"
```

**Solución:**
- Genera una nueva API key
- Verifica límites de uso en Google AI Studio
- Espera a que se renueve la cuota (se resetea mensualmente)

#### **2. Formato de audio no soportado**
```bash
# Gemini acepta: WAV, MP3, OGG, FLAC
# Verifica el formato de tu audio
file mi_audio.wav
```

**Solución:**
- Convierte el audio a formato soportado:
```bash
ffmpeg -i entrada.m4a -ar 16000 salida.wav
```

#### **3. Timeout de Gemini API**
**Solución:**
```env
# En .env, aumenta el timeout
GEMINI_TIMEOUT=60
```

---

## ❌ Error: "Connection refused"

### **Descripción:**
```
Connection refused (localhost:8000)
```

### **Causa:**
El servidor no está corriendo.

### **Solución:**
```bash
# Verifica que el servidor esté corriendo
ps aux | grep uvicorn

# Si no está corriendo, inícialo
uvicorn main:app --reload --host 0.0.0.0 --port 8000
```

---

## ❌ El audio suena distorsionado o no se reproduce

### **Posibles causas:**

#### **1. Formato no compatible con el reproductor**
**Solución:**
- Usa VLC Media Player (reproduce todos los formatos)
- Verifica el formato:
```bash
file respuesta.wav
# Debe mostrar: WAV audio, 16000 Hz, mono
```

#### **2. Audio corrupto**
**Solución:**
```bash
# Reproduce con ffplay (incluido con ffmpeg)
ffplay respuesta.wav

# O convierte de nuevo
ffmpeg -i respuesta.wav -ar 16000 -ac 1 respuesta_fixed.wav
```

---

## 🐛 Debugging avanzado

### **Activar logs detallados:**

```env
# En .env
LOG_LEVEL=DEBUG
```

Reinicia el servidor y verás información detallada:
```
DEBUG: Audio recibido: 245892 bytes
DEBUG: Subiendo audio a Gemini...
DEBUG: Gemini respondió: "Hola, ¿cómo estás?"
DEBUG: Generando TTS...
DEBUG: Audio WAV generado: 45123 bytes
```

### **Verificar conexión a Gemini:**

```bash
# Prueba directa a la API de Gemini
curl -X POST \
  "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=TU_API_KEY" \
  -H "Content-Type: application/json" \
  -d '{"contents":[{"parts":[{"text":"Hola"}]}]}'
```

Si esto falla, el problema es con tu acceso a Gemini (región o API key).

### **Verificar variables de entorno:**

```bash
# Ver configuración actual
curl http://localhost:8000/health

# Debe mostrar:
{
  "status": "ok",
  "modelo": "gemini-1.5-flash",
  "idioma_tts": "es",
  "dispositivos_activos": 0,
  "max_historial": 12
}
```

---

## 📚 Recursos útiles

- **Documentación de Gemini API:** https://ai.google.dev/gemini-api/docs
- **Regiones soportadas:** https://ai.google.dev/available_regions
- **Consola de API (para probar):** https://aistudio.google.com
- **Issues de este proyecto:** https://github.com/TuUsuario/Robot-ESP32/issues

---

## 💡 ¿No encuentras la solución?

1. **Revisa los logs del servidor** (la terminal donde corre uvicorn)
2. **Activa DEBUG mode** (`LOG_LEVEL=DEBUG` en `.env`)
3. **Prueba con curl** para aislar el problema:
   ```bash
   curl http://localhost:8000/health
   curl -X POST http://localhost:8000/chat/texto -F "mensaje=test"
   ```
4. **Abre un issue** en GitHub con:
   - Logs completos del error
   - Tu configuración (sin API keys)
   - Pasos para reproducir el problema

---

¿Sigue sin funcionar? Contacta o revisa los logs detalladamente. La mayoría de los problemas se resuelven con VPN o verificando la API key.
