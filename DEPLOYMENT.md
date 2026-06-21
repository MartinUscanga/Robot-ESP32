# 🚀 Guía Rápida de Deployment al VPS

## 📋 Opción 1: Comandos Rápidos (Copy & Paste)

### 🔄 Actualizar TODO desde GitHub

```bash
# Conectarte a tu VPS
ssh usuario@tu-vps-ip

# Navegar al directorio del proyecto
cd ~/Robot-ESP32

# Actualizar código desde GitHub
git stash                    # Guardar cambios locales
git pull origin main         # Descargar última versión
git stash pop                # Restaurar cambios locales (opcional)

# Actualizar dependencias de Python (si hay cambios)
source venv/bin/activate
pip install -r requirements.txt --upgrade

# Reiniciar la API
pkill -f uvicorn
nohup uvicorn main:app --host 0.0.0.0 --port 8000 > /tmp/robot-api.log 2>&1 &

# Actualizar interfaz web
cp -r web-interface/* /var/www/robot-assistant/

# Verificar que todo está funcionando
curl http://localhost:8000/health

echo "✅ Todo actualizado!"
```

---

## 🎯 Opción 2: Script Automatizado

### Paso 1: Descargar el script de actualización

```bash
# En tu VPS
cd ~/Robot-ESP32
git pull origin main
chmod +x update-from-github.sh
```

### Paso 2: Ejecutar el script

```bash
# Actualizar TODO (API + interfaz web)
./update-from-github.sh

# O solo la API
./update-from-github.sh api

# O solo la interfaz web
./update-from-github.sh web
```

---

## 📦 Primera Instalación en VPS

Si es la primera vez que instalas en tu VPS:

### 1. Clonar el repositorio

```bash
# Conectarte a tu VPS
ssh usuario@tu-vps-ip

# Clonar el repo
cd ~
git clone https://github.com/MartinUscanga/Robot-ESP32.git
cd Robot-ESP32
```

### 2. Instalar dependencias del sistema

```bash
sudo apt update
sudo apt install -y python3-venv python3-pip nginx ffmpeg
```

### 3. Configurar Python y dependencias

```bash
# Crear entorno virtual
python3 -m venv venv
source venv/bin/activate

# Instalar dependencias
pip install -r requirements.txt
```

### 4. Configurar variables de entorno

```bash
# Copiar .env de ejemplo
cp .env.example .env

# Editar con tu API key de Gemini
nano .env
```

Pega tu API key:
```
GEMINI_API_KEY=tu-api-key-aqui
```

### 5. Iniciar la API

```bash
# Iniciar en background
nohup uvicorn main:app --host 0.0.0.0 --port 8000 > /tmp/robot-api.log 2>&1 &

# Verificar que funciona
curl http://localhost:8000/health
```

### 6. Desplegar interfaz web con Nginx

```bash
# Crear directorio para la web
sudo mkdir -p /var/www/robot-assistant
sudo chown -R $USER:$USER /var/www/robot-assistant

# Copiar archivos
cp -r web-interface/* /var/www/robot-assistant/

# Configurar Nginx
sudo nano /etc/nginx/sites-available/robot-assistant
```

Pega esta configuración (reemplaza `tu-ip` con tu IP real):

```nginx
server {
    listen 80;
    server_name tu-ip;
    
    root /var/www/robot-assistant;
    index index.html;
    
    location / {
        try_files $uri $uri/ /index.html;
    }
    
    location ~* \.(css|js|jpg|jpeg|png|gif|ico|svg)$ {
        expires 1y;
        add_header Cache-Control "public, immutable";
    }
    
    access_log /var/log/nginx/robot-assistant-access.log;
    error_log /var/log/nginx/robot-assistant-error.log;
}
```

```bash
# Habilitar sitio
sudo ln -s /etc/nginx/sites-available/robot-assistant /etc/nginx/sites-enabled/
sudo nginx -t
sudo systemctl reload nginx
```

### 7. Configurar firewall

```bash
sudo ufw allow 22/tcp    # SSH
sudo ufw allow 80/tcp    # HTTP
sudo ufw allow 8000/tcp  # API
sudo ufw enable
```

### 8. ¡Listo! Accede desde tu navegador

```
http://tu-vps-ip
```

---

## 🔧 Comandos Útiles para el Día a Día

### Ver logs de la API

```bash
tail -f /tmp/robot-api.log
```

### Reiniciar la API

```bash
pkill -f uvicorn
cd ~/Robot-ESP32
source venv/bin/activate
nohup uvicorn main:app --host 0.0.0.0 --port 8000 > /tmp/robot-api.log 2>&1 &
```

### Ver estado de Nginx

```bash
sudo systemctl status nginx
sudo nginx -t  # Verificar configuración
```

### Ver logs de Nginx

```bash
tail -f /var/log/nginx/robot-assistant-access.log
tail -f /var/log/nginx/robot-assistant-error.log
```

### Actualizar solo código (sin reinstalar dependencias)

```bash
cd ~/Robot-ESP32
git pull origin main
cp -r web-interface/* /var/www/robot-assistant/
```

### Verificar que todo funciona

```bash
# API
curl http://localhost:8000/health

# Interfaz web
curl http://localhost/
```

---

## 🆘 Solución de Problemas Comunes

### Error: "git pull dice que hay conflictos"

```bash
# Guardar tus cambios locales
git stash

# Actualizar
git pull origin main

# Ver qué cambios tenías
git stash list

# Aplicar tus cambios de vuelta (opcional)
git stash pop
```

### Error: "La API no inicia"

```bash
# Ver el error completo
tail -30 /tmp/robot-api.log

# Verificar dependencias
cd ~/Robot-ESP32
source venv/bin/activate
pip install -r requirements.txt
```

### Error: "502 Bad Gateway en Nginx"

Significa que la API no está corriendo:

```bash
# Verificar si está corriendo
ps aux | grep uvicorn

# Iniciar si no está corriendo
cd ~/Robot-ESP32
source venv/bin/activate
nohup uvicorn main:app --host 0.0.0.0 --port 8000 > /tmp/robot-api.log 2>&1 &
```

### Error: CORS sigue apareciendo

```bash
# Verificar que main.py tiene el middleware CORS
grep -A 5 "CORSMiddleware" ~/Robot-ESP32/main.py

# Si no aparece nada, actualiza desde GitHub:
cd ~/Robot-ESP32
git pull origin main

# Y reinicia la API
pkill -f uvicorn
source venv/bin/activate
uvicorn main:app --host 0.0.0.0 --port 8000
```

---

## 🔄 Configurar Auto-reinicio (Systemd)

Para que la API se reinicie automáticamente si se cae:

### 1. Crear servicio systemd

```bash
sudo nano /etc/systemd/system/robot-api.service
```

Pega esto (ajusta las rutas si es necesario):

```ini
[Unit]
Description=Robot ESP32 API - Gemini Bridge
After=network.target

[Service]
Type=simple
User=tu-usuario
WorkingDirectory=/home/tu-usuario/Robot-ESP32
Environment="PATH=/home/tu-usuario/Robot-ESP32/venv/bin"
ExecStart=/home/tu-usuario/Robot-ESP32/venv/bin/uvicorn main:app --host 0.0.0.0 --port 8000
Restart=always
RestartSec=10

[Install]
WantedBy=multi-user.target
```

### 2. Habilitar e iniciar el servicio

```bash
sudo systemctl daemon-reload
sudo systemctl enable robot-api
sudo systemctl start robot-api
```

### 3. Comandos del servicio

```bash
# Ver estado
sudo systemctl status robot-api

# Reiniciar
sudo systemctl restart robot-api

# Detener
sudo systemctl stop robot-api

# Ver logs
sudo journalctl -u robot-api -f
```

Ahora cuando actualices desde GitHub:

```bash
cd ~/Robot-ESP32
git pull origin main
sudo systemctl restart robot-api
```

---

## ✅ Checklist de Deployment

- [ ] Repositorio clonado en VPS
- [ ] Dependencias de Python instaladas
- [ ] Variables de entorno configuradas (.env)
- [ ] API funcionando en puerto 8000
- [ ] CORS configurado en main.py
- [ ] Nginx instalado y configurado
- [ ] Interfaz web desplegada en /var/www/robot-assistant
- [ ] Firewall configurado (puertos 22, 80, 8000)
- [ ] Servicio systemd configurado (opcional pero recomendado)
- [ ] Todo funciona desde el navegador

---

## 📞 ¿Necesitas Ayuda?

Si encuentras algún error:

1. **Revisa los logs:**
   - API: `tail -f /tmp/robot-api.log`
   - Nginx: `tail -f /var/log/nginx/robot-assistant-error.log`

2. **Verifica que todo está corriendo:**
   - API: `curl http://localhost:8000/health`
   - Nginx: `sudo systemctl status nginx`

3. **Verifica la configuración:**
   - CORS en main.py
   - Firewall: `sudo ufw status`
   - Nginx: `sudo nginx -t`

¡Buena suerte! 🚀✨
