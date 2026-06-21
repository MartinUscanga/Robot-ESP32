#!/bin/bash
# ========================================
# 🔄 Actualizar Proyecto desde GitHub
# ========================================
#
# Este script actualiza tanto la API como la interfaz web
# desde el repositorio de GitHub

set -e  # Salir si hay algún error

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}"
echo "╔═══════════════════════════════════════╗"
echo "║  🔄 Actualizar desde GitHub          ║"
echo "╚═══════════════════════════════════════╝"
echo -e "${NC}"

# ========================================
# CONFIGURACIÓN
# ========================================

# Directorio del proyecto (ajusta si es diferente)
PROJECT_DIR="${PROJECT_DIR:-$HOME/Robot-ESP32}"
WEB_DIR="${WEB_DIR:-/var/www/robot-assistant}"
BRANCH="${BRANCH:-main}"

# ========================================
# FUNCIONES
# ========================================

print_step() {
    echo -e "\n${GREEN}➜${NC} $1"
}

print_error() {
    echo -e "${RED}✗${NC} $1"
}

print_success() {
    echo -e "${GREEN}✓${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}⚠${NC} $1"
}

# ========================================
# ACTUALIZAR API
# ========================================

update_api() {
    print_step "Actualizando API desde GitHub..."
    
    cd "$PROJECT_DIR"
    
    # Guardar cambios locales si los hay
    if [[ -n $(git status -s) ]]; then
        print_warning "Hay cambios locales, haciendo stash..."
        git stash
        STASHED=1
    fi
    
    # Actualizar desde GitHub
    print_step "Descargando última versión..."
    git fetch origin
    git pull origin $BRANCH
    
    print_success "API actualizada"
    
    # Restaurar cambios locales si los había
    if [ "$STASHED" = "1" ]; then
        print_warning "Restaurando cambios locales..."
        git stash pop || true
    fi
}

# ========================================
# ACTUALIZAR INTERFAZ WEB
# ========================================

update_web_interface() {
    print_step "Actualizando interfaz web..."
    
    if [ ! -d "$WEB_DIR" ]; then
        print_warning "Directorio $WEB_DIR no existe, creándolo..."
        sudo mkdir -p "$WEB_DIR"
        sudo chown -R $USER:$USER "$WEB_DIR"
    fi
    
    # Copiar archivos de la interfaz web
    print_step "Copiando archivos..."
    cp -r "$PROJECT_DIR/web-interface/"* "$WEB_DIR/"
    
    # Remover archivos innecesarios
    rm -f "$WEB_DIR"/*.sh "$WEB_DIR"/*.md 2>/dev/null || true
    
    print_success "Interfaz web actualizada"
}

# ========================================
# REINSTALAR DEPENDENCIAS
# ========================================

reinstall_dependencies() {
    print_step "¿Reinstalar dependencias de Python? [y/N]"
    read -r response
    
    if [[ "$response" =~ ^[Yy]$ ]]; then
        cd "$PROJECT_DIR"
        
        if [ ! -d "venv" ]; then
            print_warning "Creando entorno virtual..."
            python3 -m venv venv
        fi
        
        source venv/bin/activate
        pip install -r requirements.txt --upgrade
        
        print_success "Dependencias actualizadas"
    fi
}

# ========================================
# REINICIAR SERVICIOS
# ========================================

restart_services() {
    print_step "Reiniciando servicios..."
    
    # Reiniciar API
    if systemctl is-active --quiet robot-api; then
        print_step "Reiniciando servicio robot-api..."
        sudo systemctl restart robot-api
        print_success "Servicio robot-api reiniciado"
    else
        print_warning "Servicio robot-api no está configurado"
        print_step "¿Reiniciar API manualmente? [y/N]"
        read -r response
        
        if [[ "$response" =~ ^[Yy]$ ]]; then
            print_step "Matando procesos de uvicorn..."
            pkill -f uvicorn || true
            
            print_step "Iniciando API..."
            cd "$PROJECT_DIR"
            source venv/bin/activate
            nohup uvicorn main:app --host 0.0.0.0 --port 8000 > /tmp/robot-api.log 2>&1 &
            
            print_success "API iniciada en background"
            print_warning "Logs en: /tmp/robot-api.log"
        fi
    fi
    
    # Reiniciar Nginx si existe
    if command -v nginx > /dev/null 2>&1; then
        print_step "Recargando Nginx..."
        sudo systemctl reload nginx || true
        print_success "Nginx recargado"
    fi
}

# ========================================
# VERIFICAR ESTADO
# ========================================

verify_status() {
    print_step "Verificando estado de los servicios..."
    
    # Verificar API
    sleep 2
    if curl -s http://localhost:8000/health > /dev/null 2>&1; then
        print_success "API está respondiendo"
    else
        print_error "API no está respondiendo"
        print_warning "Revisa los logs: tail -f /tmp/robot-api.log"
    fi
    
    # Verificar interfaz web
    if [ -f "$WEB_DIR/index.html" ]; then
        print_success "Interfaz web desplegada"
    else
        print_warning "Interfaz web no encontrada en $WEB_DIR"
    fi
}

# ========================================
# MOSTRAR RESUMEN
# ========================================

show_summary() {
    echo ""
    echo -e "${GREEN}╔═══════════════════════════════════════╗${NC}"
    echo -e "${GREEN}║    ✨ Actualización completada! ✨   ║${NC}"
    echo -e "${GREEN}╚═══════════════════════════════════════╝${NC}"
    echo ""
    echo "📍 Servicios:"
    echo -e "   API: ${BLUE}http://localhost:8000/health${NC}"
    echo -e "   Web: ${BLUE}http://$(hostname -I | awk '{print $1}')${NC}"
    echo ""
    echo "🔧 Comandos útiles:"
    echo "   Ver logs de API:"
    echo "     tail -f /tmp/robot-api.log"
    echo ""
    echo "   Reiniciar API:"
    echo "     sudo systemctl restart robot-api"
    echo ""
    echo "   Ver estado:"
    echo "     systemctl status robot-api"
    echo ""
}

# ========================================
# MAIN
# ========================================

main() {
    update_api
    update_web_interface
    reinstall_dependencies
    restart_services
    verify_status
    show_summary
}

# Ejecutar según argumentos
case "${1:-all}" in
    api)
        update_api
        restart_services
        verify_status
        ;;
    web)
        update_web_interface
        restart_services
        verify_status
        ;;
    all)
        main
        ;;
    *)
        echo "Uso: $0 [comando]"
        echo ""
        echo "Comandos:"
        echo "  all (default) - Actualizar todo"
        echo "  api          - Solo actualizar API"
        echo "  web          - Solo actualizar interfaz web"
        exit 1
        ;;
esac
