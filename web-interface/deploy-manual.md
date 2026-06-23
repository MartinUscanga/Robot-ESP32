# 📦 Guía de Deployment Manual al VPS

Si prefieres subir los archivos manualmente sin usar el script automatizado, sigue estos pasos:

## 📋 Requisitos Previos

1. **Acceso SSH a tu VPS**
   ```bash
   ssh usuario@tu-vps-ip
   ```

2. **Servidor web instalado** (Nginx o Apache)
   - Si no tienes ninguno, te recomiendo Nginx (instrucciones abajo)

3. **Tu API de Gemini corriendo** en el VPS (puerto 8000)

---

## 🚀 Opción 1: Deployment con Nginx (Recomendado)

### Paso 1: Instalar Nginx en tu VPS

```bash
# Conectarte a tu VPS
ssh usuario@tu-vps-ip

# Instalar Nginx
sudo apt update
sudo apt install -y nginx

# Verificar que está corriendo
sudo systemctl status nginx
```

### Paso 2: Crear directorio para la web

```bash
# Crear directorio
sudo mkdir -p /var/www/robot-assistant

# Dar permisos a tu usuario (reemplaza 'usuario' con tu nombre de usuario)
sudo chown -R $USER:$USER /var/www/robot-assistant
```

### Paso 3: Subir archivos desde tu máquina local

```bash
# Desde tu máquina local, en el directorio web-interface/
rsync -avz --progress \
  --exclude='*.sh' \
  --exclude='*.md' \
  --exclude='.git' \
  ./ usuario@tu-vps-ip:/var/www/robot-assistant/

# O usando SCP si no tienes rsync:
scp -r index.html css/ js/ usuario@tu-vps-ip:/var/www/robot-assistant/
```

### Paso 4: Configurar Nginx

```bash
# En tu VPS, crear archivo de configuración
sudo nano /etc/nginx/sites-available/robot-assistant
```

Pega esta configuración:

```nginx
server {
    listen 80;
    server_name tu-vps-ip;  # Reemplaza con tu IP o dominio
    
    root /var/www/robot-assistant;
    index index.html;
    
    # Logs
    access_log /var/log/nginx/robot-assistant-access.log;
    error_log /var/log/nginx/robot-assistant-error.log;
    
    # Servir archivos estáticos
    location / {
        try_files $uri $uri/ /index.html;
        
        # Headers de seguridad
        add_header X-Frame-Options "SAMEORIGIN" always;
        add_header X-Content-Type-Options "nosniff" always;
        add_header X-XSS-Protection "1; mode=block" always;
    }
    
    # Cache para assets
    location ~* \.(css|js|jpg|jpeg|png|gif|ico|svg)$ {
        expires 1y;
        add_header Cache-Control "public, immutable";
    }
    
    # Compresión
    gzip on;
    gzip_types text/plain text/css application/json application/javascript text/xml application/xml text/javascript;
    gzip_min_length 256;
}
```

### Paso 5: Habilitar el sitio

```bash
# Crear enlace simbólico
sudo ln -s /etc/nginx/sites-available/robot-assistant /etc/nginx/sites-enabled/

# Verificar configuración
sudo nginx -t

# Recargar Nginx
sudo systemctl reload nginx
```

### Paso 6: Configurar firewall

```bash
# Permitir tráfico HTTP/HTTPS
sudo ufw allow 80/tcp
sudo ufw allow 443/tcp
sudo ufw allow 8000/tcp  # Para la API de Gemini
```

### Paso 7: Acceder a tu interfaz

Abre tu navegador en: `http://tu-vps-ip`

---

## 🌐 Opción 2: Deployment con Apache

### Paso 1: Instalar Apache

```bash
# En tu VPS
sudo apt update
sudo apt install -y apache2

# Verificar que está corriendo
sudo systemctl status apache2
```

### Paso 2: Crear directorio y subir archivos

```bash
# Crear directorio
sudo mkdir -p /var/www/robot-assistant

# Dar permisos
sudo chown -R $USER:$USER /var/www/robot-assistant

# Desde tu máquina local:
rsync -avz ./ usuario@tu-vps-ip:/var/www/robot-assistant/
```

### Paso 3: Configurar Apache

```bash
# Crear configuración
sudo nano /etc/apache2/sites-available/robot-assistant.conf
```

Pega esto:

```apache
<VirtualHost *:80>
    ServerName tu-vps-ip
    DocumentRoot /var/www/robot-assistant
    
    <Directory /var/www/robot-assistant>
        Options -Indexes +FollowSymLinks
        AllowOverride All
        Require all granted
    </Directory>
    
    ErrorLog ${APACHE_LOG_DIR}/robot-assistant-error.log
    CustomLog ${APACHE_LOG_DIR}/robot-assistant-access.log combined
</VirtualHost>
```

### Paso 4: Habilitar sitio

```bash
# Habilitar el sitio
sudo a2ensite robot-assistant

# Recargar Apache
sudo systemctl reload apache2
```

---

## 🐍 Opción 3: Servidor Python Simple (Solo para testing)

Si solo quieres probar rápidamente sin configurar Nginx/Apache:

```bash
# En tu VPS, ve al directorio
cd /var/www/robot-assistant

# Inicia servidor Python
python3 -m http.server 8080

# Accede en: http://tu-vps-ip:8080
```

**⚠️ Nota:** Esta opción NO es para producción, solo para pruebas.

---

## 🔒 Configurar HTTPS/SSL (Opcional pero recomendado)

Si tienes un dominio apuntando a tu VPS:

### Con Let's Encrypt (Gratis)

```bash
# Instalar Certbot
sudo apt install -y certbot python3-certbot-nginx

# Obtener certificado (reemplaza tu-dominio.com)
sudo certbot --nginx -d tu-dominio.com -d www.tu-dominio.com

# Certbot configurará SSL automáticamente
```

**Nota:** Necesitas tener un dominio DNS apuntando a tu VPS para que esto funcione.

---

## ⚙️ Configurar CORS en tu API

Para que la interfaz web pueda comunicarse con tu API desde el navegador, necesitas habilitar CORS.

### En tu VPS, edita `main.py`:

```bash
nano ~/Robot-ESP32/main.py
```

Agrega estas líneas después de `app = FastAPI(...)`:

```python
from fastapi.middleware.cors import CORSMiddleware

# ... código existente ...

app = FastAPI(title="Puente ESP32 <-> Gemini", version="1.0.0")

# ⬇️ AGREGAR ESTAS LÍNEAS ⬇️
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # En producción, especifica tu dominio
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)
```

Luego reinicia tu API:

```bash
# Si usas uvicorn directamente:
pkill -f uvicorn
uvicorn main:app --host 0.0.0.0 --port 8000

# Si usas systemd (servicio):
sudo systemctl restart robot-api
```

---

## 🔄 Actualizar la interfaz web después

Cuando hagas cambios locales y quieras actualizar el VPS:

```bash
# Desde tu máquina local:
rsync -avz ./ usuario@tu-vps-ip:/var/www/robot-assistant/

# No necesitas reiniciar Nginx, los cambios son inmediatos
```

---

## 🧪 Verificar que todo funciona

### 1. Verificar que la web está accesible:
```bash
curl http://tu-vps-ip
# Deberías ver HTML
```

### 2. Verificar que la API responde:
```bash
curl http://tu-vps-ip:8000/health
# Deberías ver: {"status":"ok",...}
```

### 3. Verificar CORS (desde tu navegador):
- Abre la consola de desarrollador (F12)
- Ve a la pestaña "Network"
- Intenta enviar un mensaje
- No debería haber errores de CORS

---

## 📱 Configurar la interfaz web

Una vez que todo esté funcionando:

1. Abre `http://tu-vps-ip` en tu navegador
2. Haz click en el botón de configuración (⚙️)
3. Ingresa la URL de tu API: `http://tu-vps-ip:8000`
4. Guarda la configuración
5. ¡Empieza a chatear! 💬✨

---

## 🐛 Solución de Problemas

### Error: "No se pudo conectar con el servidor"

1. **Verifica que tu API esté corriendo:**
   ```bash
   curl http://localhost:8000/health
   ```

2. **Verifica el firewall:**
   ```bash
   sudo ufw status
   # Debe mostrar: 8000 ALLOW
   ```

3. **Revisa logs de la API:**
   ```bash
   journalctl -u robot-api -f  # Si usas systemd
   # O
   tail -f /var/log/robot-api.log
   ```

### Error CORS en el navegador

Asegúrate de haber agregado el middleware CORS en `main.py` (ver arriba).

### Nginx no inicia

```bash
# Ver logs de error
sudo tail -f /var/log/nginx/error.log

# Verificar sintaxis
sudo nginx -t
```

### No puedo acceder desde fuera

1. **Verifica firewall del VPS:**
   ```bash
   sudo ufw status
   ```

2. **Verifica firewall del proveedor cloud:**
   - AWS: Security Groups
   - Google Cloud: Firewall Rules
   - DigitalOcean: Firewalls
   - Vultr: Firewall

---

## 📋 Checklist de Deployment

- [ ] Nginx/Apache instalado
- [ ] Archivos subidos a `/var/www/robot-assistant`
- [ ] Configuración de Nginx/Apache creada
- [ ] Sitio habilitado
- [ ] Firewall configurado (puertos 80, 8000)
- [ ] CORS configurado en la API
- [ ] API corriendo en el puerto 8000
- [ ] Interfaz web accesible desde el navegador
- [ ] Configuración de API guardada en la interfaz
- [ ] Chat funcionando correctamente

---

¡Listo! Tu asistente kawaii ya está en internet 🌐✨

Si tienes problemas, revisa los logs:
- Nginx: `/var/log/nginx/robot-assistant-error.log`
- API: `/var/log/robot-api.log`
- Navegador: Consola de desarrollador (F12)
