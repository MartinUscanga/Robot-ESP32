#!/bin/bash
# ========================================
# 🚀 Script de Deployment para VPS
# ========================================
#
# Este script facilita el despliegue de la interfaz web kawaii en tu VPS
# Puedes usarlo de diferentes formas dependiendo de tu configuración

set -e  # Salir si hay algún error

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}"
echo "╔═══════════════════════════════════════╗"
echo "║  🤖 Deploy Interfaz Web Kawaii       ║"
echo "╔═══════════════════════════════════════╗"
echo -e "${NC}"

# ========================================
# CONFIGURACIÓN - Edita estos valores
# ========================================

# Datos del VPS
VPS_USER="${VPS_USER:-root}"                    # Usuario SSH (por defecto: root)
VPS_HOST="${VPS_HOST:-tu-vps-ip.com}"          # IP o dominio de tu VPS
VPS_PORT="${VPS_PORT:-22}"                      # Puerto SSH (por defecto: 22)

# Rutas en el VPS
WEB_DIR="${WEB_DIR:-/var/www/robot-assistant}"  # Dónde se guardará la web
NGINX_AVAILABLE="/etc/nginx/sites-available"
NGINX_ENABLED="/etc/nginx/sites-enabled"

# Configuración del dominio (opcional)
DOMAIN="${DOMAIN:-}"  # Deja vacío si usas solo IP

# ========================================
# FUNCIONES
# ========================================

print_step() {
    echo -e "\n${GREEN}➜${NC} $1"
}

print_error() {
    echo -e "${RED}✗${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}⚠${NC} $1"
}

print_success() {
    echo -e "${GREEN}✓${NC} $1"
}

# Verificar si tenemos las variables necesarias
check_config() {
    print_step "Verificando configuración..."
    
    if [ "$VPS_HOST" = "tu-vps-ip.com" ]; then
        print_error "Debes configurar VPS_HOST con la IP o dominio de tu VPS"
        echo ""
        echo "Edita este archivo o ejecuta:"
        echo "  export VPS_HOST='tu-ip-o-dominio'"
        echo "  export VPS_USER='tu-usuario'"
        echo "  ./deploy.sh"
        exit 1
    fi
    
    print_success "Configuración OK"
}

# Test de conexión SSH
test_ssh() {
    print_step "Probando conexión SSH..."
    
    if ssh -p "$VPS_PORT" -o ConnectTimeout=5 "$VPS_USER@$VPS_HOST" "echo 'Conexión exitosa'" > /dev/null 2>&1; then
        print_success "Conexión SSH OK"
        return 0
    else
        print_error "No se pudo conectar al VPS"
        echo ""
        echo "Asegúrate de:"
        echo "  1. Tener acceso SSH configurado (usa ssh-copy-id si es la primera vez)"
        echo "  2. El VPS_HOST y VPS_PORT son correctos"
        echo "  3. El firewall permite conexiones SSH"
        echo ""
        echo "Para configurar SSH sin contraseña:"
        echo "  ssh-copy-id -p $VPS_PORT $VPS_USER@$VPS_HOST"
        exit 1
    fi
}

# Copiar archivos al VPS
upload_files() {
    print_step "Subiendo archivos al VPS..."
    
    # Crear directorio si no existe
    ssh -p "$VPS_PORT" "$VPS_USER@$VPS_HOST" "mkdir -p $WEB_DIR"
    
    # Copiar archivos
    rsync -avz -e "ssh -p $VPS_PORT" \
        --exclude='*.sh' \
        --exclude='.git' \
        --exclude='README.md' \
        ./ "$VPS_USER@$VPS_HOST:$WEB_DIR/"
    
    print_success "Archivos subidos a $WEB_DIR"
}

# Instalar Nginx si no está instalado
install_nginx() {
    print_step "Verificando Nginx..."
    
    if ssh -p "$VPS_PORT" "$VPS_USER@$VPS_HOST" "command -v nginx > /dev/null 2>&1"; then
        print_success "Nginx ya está instalado"
    else
        print_warning "Nginx no está instalado. Instalando..."
        ssh -p "$VPS_PORT" "$VPS_USER@$VPS_HOST" "apt update && apt install -y nginx"
        print_success "Nginx instalado"
    fi
}

# Configurar Nginx
setup_nginx() {
    print_step "Configurando Nginx..."
    
    # Determinar el server_name
    if [ -n "$DOMAIN" ]; then
        SERVER_NAME="$DOMAIN www.$DOMAIN"
    else
        SERVER_NAME="$VPS_HOST"
    fi
    
    # Crear configuración de Nginx
    cat << EOF | ssh -p "$VPS_PORT" "$VPS_USER@$VPS_HOST" "cat > $NGINX_AVAILABLE/robot-assistant"
server {
    listen 80;
    server_name $SERVER_NAME;
    
    root $WEB_DIR;
    index index.html;
    
    # Logs
    access_log /var/log/nginx/robot-assistant-access.log;
    error_log /var/log/nginx/robot-assistant-error.log;
    
    # Servir archivos estáticos
    location / {
        try_files \$uri \$uri/ /index.html;
        
        # Headers de seguridad
        add_header X-Frame-Options "SAMEORIGIN" always;
        add_header X-Content-Type-Options "nosniff" always;
        add_header X-XSS-Protection "1; mode=block" always;
    }
    
    # Cache para assets estáticos
    location ~* \.(css|js|jpg|jpeg|png|gif|ico|svg|woff|woff2|ttf|eot)$ {
        expires 1y;
        add_header Cache-Control "public, immutable";
    }
    
    # Comprimir archivos de texto
    gzip on;
    gzip_types text/plain text/css application/json application/javascript text/xml application/xml application/xml+rss text/javascript;
    gzip_min_length 256;
}
EOF
    
    # Habilitar sitio
    ssh -p "$VPS_PORT" "$VPS_USER@$VPS_HOST" "ln -sf $NGINX_AVAILABLE/robot-assistant $NGINX_ENABLED/"
    
    # Test y reload de Nginx
    ssh -p "$VPS_PORT" "$VPS_USER@$VPS_HOST" "nginx -t && systemctl reload nginx"
    
    print_success "Nginx configurado"
}

# Configurar firewall
setup_firewall() {
    print_step "Configurando firewall..."
    
    if ssh -p "$VPS_PORT" "$VPS_USER@$VPS_HOST" "command -v ufw > /dev/null 2>&1"; then
        ssh -p "$VPS_PORT" "$VPS_USER@$VPS_HOST" "
            ufw allow 80/tcp > /dev/null 2>&1 || true
            ufw allow 443/tcp > /dev/null 2>&1 || true
            ufw allow 8000/tcp > /dev/null 2>&1 || true
        "
        print_success "Firewall configurado (puertos 80, 443, 8000)"
    else
        print_warning "UFW no está instalado, saltando configuración de firewall"
    fi
}

# Configurar SSL con Let's Encrypt (opcional)
setup_ssl() {
    if [ -z "$DOMAIN" ]; then
        print_warning "No se configuró un dominio, saltando SSL"
        return
    fi
    
    print_step "¿Deseas configurar SSL/HTTPS con Let's Encrypt? (requiere dominio) [y/N]"
    read -r response
    
    if [[ "$response" =~ ^[Yy]$ ]]; then
        print_step "Instalando Certbot..."
        ssh -p "$VPS_PORT" "$VPS_USER@$VPS_HOST" "
            apt install -y certbot python3-certbot-nginx
            certbot --nginx -d $DOMAIN -d www.$DOMAIN --non-interactive --agree-tos --email admin@$DOMAIN
        "
        print_success "SSL configurado"
    fi
}

# Mostrar resumen
show_summary() {
    echo ""
    echo -e "${GREEN}╔═══════════════════════════════════════╗${NC}"
    echo -e "${GREEN}║     ✨ Deployment completado! ✨     ║${NC}"
    echo -e "${GREEN}╚═══════════════════════════════════════╝${NC}"
    echo ""
    echo "📍 Tu interfaz web está disponible en:"
    
    if [ -n "$DOMAIN" ]; then
        echo -e "   ${BLUE}http://$DOMAIN${NC}"
        echo -e "   ${BLUE}https://$DOMAIN${NC} (si configuraste SSL)"
    else
        echo -e "   ${BLUE}http://$VPS_HOST${NC}"
    fi
    
    echo ""
    echo "⚙️  No olvides configurar la URL de tu API en la interfaz web:"
    echo -e "   ${YELLOW}http://$VPS_HOST:8000${NC}"
    echo ""
    echo "🔧 Comandos útiles:"
    echo "   Ver logs de Nginx:"
    echo "     ssh -p $VPS_PORT $VPS_USER@$VPS_HOST 'tail -f /var/log/nginx/robot-assistant-access.log'"
    echo ""
    echo "   Reiniciar Nginx:"
    echo "     ssh -p $VPS_PORT $VPS_USER@$VPS_HOST 'systemctl restart nginx'"
    echo ""
    echo "   Actualizar archivos:"
    echo "     ./deploy.sh"
    echo ""
}

# ========================================
# MAIN
# ========================================

main() {
    check_config
    test_ssh
    upload_files
    install_nginx
    setup_nginx
    setup_firewall
    setup_ssl
    show_summary
}

# Ejecutar si no se pasa ningún argumento
if [ $# -eq 0 ]; then
    main
else
    case "$1" in
        upload)
            check_config
            test_ssh
            upload_files
            print_success "Archivos actualizados"
            ;;
        nginx)
            check_config
            test_ssh
            setup_nginx
            print_success "Nginx reconfigurado"
            ;;
        ssl)
            check_config
            test_ssh
            setup_ssl
            ;;
        *)
            echo "Uso: $0 [comando]"
            echo ""
            echo "Comandos disponibles:"
            echo "  (sin comando)  - Deploy completo"
            echo "  upload        - Solo subir archivos"
            echo "  nginx         - Solo reconfigurar Nginx"
            echo "  ssl           - Solo configurar SSL"
            exit 1
            ;;
    esac
fi
