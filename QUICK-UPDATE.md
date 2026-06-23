# ⚡ Actualización Rápida desde GitHub

## 🚀 Un solo comando para actualizar TODO

```bash
cd ~/Robot-ESP32 && git stash && git pull origin main && git stash pop && cp -r web-interface/* /var/www/robot-assistant/ && pkill -f uvicorn && source venv/bin/activate && nohup uvicorn main:app --host 0.0.0.0 --port 8000 > /tmp/robot-api.log 2>&1 & && echo "✅ Actualizado! API logs: tail -f /tmp/robot-api.log"
```

---

## 📝 Desglose del comando (por si quieres entenderlo)

```bash
# 1. Ir al directorio del proyecto
cd ~/Robot-ESP32

# 2. Guardar cambios locales (si los hay)
git stash

# 3. Descargar última versión desde GitHub
git pull origin main

# 4. Restaurar cambios locales
git stash pop

# 5. Actualizar interfaz web
cp -r web-interface/* /var/www/robot-assistant/

# 6. Reiniciar API
pkill -f uvicorn
source venv/bin/activate
nohup uvicorn main:app --host 0.0.0.0 --port 8000 > /tmp/robot-api.log 2>&1 &

# 7. Listo!
echo "✅ Actualizado!"
```

---

## 🎯 Comandos individuales

### Solo actualizar código

```bash
cd ~/Robot-ESP32 && git pull origin main
```

### Solo actualizar interfaz web

```bash
cp -r ~/Robot-ESP32/web-interface/* /var/www/robot-assistant/
```

### Solo reiniciar API

```bash
pkill -f uvicorn && cd ~/Robot-ESP32 && source venv/bin/activate && uvicorn main:app --host 0.0.0.0 --port 8000
```

---

## ✅ Verificar que funciona

```bash
# Verificar API
curl http://localhost:8000/health

# Ver logs
tail -f /tmp/robot-api.log

# Ver interfaz web
curl http://localhost/
```

---

## 💾 Guardar este comando como alias

Agrega esto a tu `~/.bashrc`:

```bash
echo 'alias update-robot="cd ~/Robot-ESP32 && git pull origin main && cp -r web-interface/* /var/www/robot-assistant/ && pkill -f uvicorn && source venv/bin/activate && nohup uvicorn main:app --host 0.0.0.0 --port 8000 > /tmp/robot-api.log 2>&1 & && echo \"✅ Actualizado!\""' >> ~/.bashrc
source ~/.bashrc
```

Ahora solo ejecuta:

```bash
update-robot
```

¡Y listo! 🎉
