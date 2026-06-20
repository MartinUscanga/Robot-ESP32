# 🚀 Guía de Despliegue en RackNerd VPS

Esta guía resuelve problemas específicos de despliegue en VPS de RackNerd, incluyendo errores de región bloqueada.

---

## ⚠️ Problema: Error 400 en VPS de Los Ángeles

**Síntoma:**
```
400 FAILED_PRECONDITION: User location is not supported for the API use.
```

**Aunque tu VPS está en Los Ángeles (USA)**, algunos rangos de IPs de datacenters pueden estar bloqueados por Google.

---

## 🔍 Diagnóstico Rápido

### **Paso 1: Verificar tu IP y ubicación**

```bash
# Conectar a tu VPS
ssh root@TU_IP_RACKNERD

# Ver IP pública
curl ifconfig.me

# Ver ubicación detectada
curl https://ipinfo.io/json
```

**Busca:**
- `"country": "US"` ← ¿Dice US?
- `"org": "AS..."` ← ¿Qué proveedor muestra?

---

### **Paso 2: Probar acceso directo a Gemini**

```bash
# Prueba desde el VPS
curl -X POST \
  "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=TU_API_KEY" \
  -H "Content-Type: application/json" \
  -d '{"contents":[{"parts":[{"text":"test"}]}]}'
```

**Reemplaza `TU_API_KEY` con tu key real.**

#### **✅ Si funciona:**
```json
{"candidates": [{"content": {"parts": [{"text": "..."}]}}]}
```
→ El problema está en tu código Python. **Ve a Solución A**

#### **❌ Si falla:**
```json
{"error": {"code": 400, "message": "User location is not supported"}}
```
→ Tu IP está bloqueada. **Ve a Solución B**

---

## ✅ Solución A: Actualizar librerías (si curl funciona)

```bash
# En tu VPS
cd ~/Robot-ESP32
source venv/bin/activate

# Actualizar librerías
pip install --upgrade google-genai pip

# Reinstalar dependencias
pip install -r requirements.txt

# Reiniciar servicio
sudo systemctl restart robot-esp32

# Ver logs
sudo journalctl -u robot-esp32 -f
```

**Probar:**
```bash
curl -X POST http://localhost:8000/chat/texto \
  -F "mensaje=Hola" \
  -F "device_id=test"
```

---

## ✅ Solución B: Usar Cloudflare WARP (si curl falla)

**Cloudflare WARP** es un proxy gratuito que cambia tu IP de salida.

### **Instalación:**

```bash
# 1. Agregar repositorio de Cloudflare
curl -fsSL https://pkg.cloudflareclient.com/pubkey.gpg | sudo gpg --yes --dearmor --output /usr/share/keyrings/cloudflare-warp-archive-keyring.gpg

echo "deb [arch=amd64 signed-by=/usr/share/keyrings/cloudflare-warp-archive-keyring.gpg] https://pkg.cloudflareclient.com/ $(lsb_release -cs) main" | sudo tee /etc/apt/sources.list.d/cloudflare-client.list

# 2. Instalar
sudo apt update
sudo apt install -y cloudflare-warp

# 3. Registrar
warp-cli register

# 4. Conectar
warp-cli connect

# 5. Verificar
warp-cli status
```

**Deberías ver:**
```
Status update: Connected
```

### **Verificar nueva IP:**

```bash
curl ifconfig.me
curl https://ipinfo.io/json
```

**La IP debería haber cambiado.**

### **Probar Gemini de nuevo:**

```bash
curl -X POST \
  "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=TU_API_KEY" \
  -H "Content-Type: application/json" \
  -d '{"contents":[{"parts":[{"text":"test"}]}]}'
```

**✅ Si ahora funciona:**

```bash
# Reiniciar tu servicio
sudo systemctl restart robot-esp32

# Ver logs
sudo journalctl -u robot-esp32 -f

# Probar API
curl http://localhost:8000/health
```

---

## ✅ Solución C: Usar Proxy SOCKS5 Externo

Si WARP no funciona, puedes usar un proxy externo.

### **1. Obtener proxy SOCKS5 gratuito:**

**Opciones:**
- **Webshare:** https://www.webshare.io (10 proxies gratis, incluye SOCKS5)
- **ProxyScrape:** https://proxyscrape.com/home
- **SSH Tunnel a otro VPS:** Si tienes otro VPS en región soportada

### **2. Configurar en `.env`:**

```bash
cd ~/Robot-ESP32
nano .env
```

**Agregar:**
```env
SOCKS5_PROXY=socks5://usuario:password@proxy-host.com:1080
```

O sin autenticación:
```env
SOCKS5_PROXY=socks5://proxy-host.com:1080
```

### **3. Actualizar código e instalar dependencias:**

```bash
# Actualizar código desde GitHub
git pull origin main

# Instalar nueva dependencia (httpx[socks])
pip install -r requirements.txt

# Reiniciar
sudo systemctl restart robot-esp32
```

### **4. Verificar logs:**

```bash
sudo journalctl -u robot-esp32 -f
```

**Deberías ver:**
```
🌐 Usando proxy SOCKS5: socks5://proxy-host.com:1080
```

---

## ✅ Solución D: Crear túnel SSH a otro servidor

Si tienes acceso a otro servidor en región soportada (ej: AWS, DigitalOcean):

### **En tu servidor RackNerd:**

```bash
# Crear túnel SOCKS5 a otro servidor
ssh -D 1080 -C -N usuario@servidor-en-usa.com &

# Esto crea un proxy SOCKS5 en localhost:1080
```

### **Configurar en `.env`:**

```env
SOCKS5_PROXY=socks5://localhost:1080
```

### **Hacer el túnel permanente (con systemd):**

```bash
sudo nano /etc/systemd/system/ssh-tunnel.service
```

**Contenido:**
```ini
[Unit]
Description=SSH Tunnel to US Server
After=network.target

[Service]
Type=simple
User=root
ExecStart=/usr/bin/ssh -D 1080 -C -N -o ServerAliveInterval=60 usuario@servidor-usa.com
Restart=always
RestartSec=10

[Install]
WantedBy=multi-user.target
```

**Activar:**
```bash
sudo systemctl daemon-reload
sudo systemctl enable ssh-tunnel
sudo systemctl start ssh-tunnel
sudo systemctl status ssh-tunnel
```

---

## 🧪 Pruebas Finales

### **1. Desde el VPS:**

```bash
# Verificar salud
curl http://localhost:8000/health

# Probar con texto
curl -X POST http://localhost:8000/chat/texto \
  -F "mensaje=Hola robot, preséntate" \
  -F "device_id=test" \
  -o respuesta.wav

# Ver si se generó el archivo
ls -lh respuesta.wav

# Escuchar audio (si tienes X11 forwarding)
file respuesta.wav
```

### **2. Desde tu máquina local:**

```bash
# Verificar salud
curl http://TU_IP_RACKNERD:8000/health

# Probar endpoint
curl -X POST http://TU_IP_RACKNERD:8000/chat/texto \
  -F "mensaje=Hola" \
  -F "device_id=test" \
  -o respuesta.wav

# Reproducir audio
# Abre respuesta.wav con tu reproductor
```

---

## 📊 Resumen de Soluciones

| Solución | Dificultad | Costo | Velocidad |
|----------|------------|-------|-----------|
| **A: Actualizar librerías** | ⭐ Fácil | Gratis | Rápido |
| **B: Cloudflare WARP** | ⭐⭐ Media | Gratis | Rápido |
| **C: Proxy SOCKS5** | ⭐⭐⭐ Media-Alta | Gratis/Pago | Medio |
| **D: Túnel SSH** | ⭐⭐⭐⭐ Alta | Requiere 2do VPS | Rápido |

**Recomendación:** Probar en orden (A → B → C → D)

---

## 🆘 Comandos de Debugging

```bash
# Ver logs en tiempo real
sudo journalctl -u robot-esp32 -f

# Ver últimos 100 logs
sudo journalctl -u robot-esp32 -n 100

# Verificar estado del servicio
sudo systemctl status robot-esp32

# Reiniciar servicio
sudo systemctl restart robot-esp32

# Probar manualmente (sin systemd)
cd ~/Robot-ESP32
source venv/bin/activate
uvicorn main:app --host 0.0.0.0 --port 8000
# Observa los errores en consola

# Ver puertos abiertos
sudo netstat -tulpn | grep 8000

# Ver procesos de Python
ps aux | grep python

# Verificar IP actual
curl ifconfig.me

# Verificar ubicación
curl https://ipinfo.io/json
```

---

## 💡 Tips Adicionales

### **1. Si WARP causa problemas con SSH:**

```bash
# Desconectar WARP temporalmente
warp-cli disconnect

# Hacer tus cambios por SSH

# Reconectar WARP
warp-cli connect
```

### **2. Configurar WARP para excluir SSH:**

```bash
# Excluir puerto 22 (SSH) de WARP
warp-cli add-excluded-route 0.0.0.0/0
warp-cli delete-excluded-route 0.0.0.0/0
```

### **3. Logs detallados:**

```env
# En .env
LOG_LEVEL=DEBUG
```

---

## 📞 Soporte

Si ninguna solución funciona:

1. **Verifica tu API key:** https://aistudio.google.com/apikey
2. **Verifica cuota de Gemini:** Puede estar excedida
3. **Contacta a RackNerd:** Pregunta si tienen IPs en rango no bloqueado
4. **Considera migrar a otro proveedor:** DigitalOcean, Vultr, AWS

---

## ✅ Checklist Final

- [ ] VPS muestra `"country": "US"` en ipinfo.io
- [ ] curl directo a Gemini API funciona
- [ ] `/health` responde correctamente
- [ ] `/chat/texto` genera audio
- [ ] Servicio corre con systemd
- [ ] Logs no muestran errores
- [ ] Prueba desde máquina local funciona

---

¡Buena suerte! 🚀
