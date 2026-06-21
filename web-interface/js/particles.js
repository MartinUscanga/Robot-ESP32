/**
 * ✨ Particle System - Sistema de partículas flotantes kawaii
 */

class ParticleSystem {
    constructor() {
        this.container = document.getElementById('particles');
        this.particles = [];
        this.maxParticles = 30;
        this.particleTypes = ['star', 'heart', 'sparkle', 'circle'];
        this.colors = [
            '#FF9ECD', // primary
            '#B4A7FF', // secondary
            '#FFD1DC', // accent
            '#B4F8C8', // success
            '#FFF4A3', // warning
            '#E8F4F8', // light blue
        ];
        
        this.init();
    }

    /**
     * Inicializa el sistema de partículas
     */
    init() {
        // Crear partículas iniciales
        for (let i = 0; i < this.maxParticles; i++) {
            setTimeout(() => {
                this.createParticle();
            }, i * 200); // Aparecen gradualmente
        }

        // Crear nuevas partículas periódicamente
        setInterval(() => {
            if (this.particles.length < this.maxParticles) {
                this.createParticle();
            }
        }, 2000);
    }

    /**
     * Crea una nueva partícula
     */
    createParticle() {
        const particle = document.createElement('div');
        particle.className = 'particle';
        
        // Propiedades aleatorias
        const type = this.randomChoice(this.particleTypes);
        const color = this.randomChoice(this.colors);
        const size = this.randomRange(8, 20);
        const startX = this.randomRange(0, window.innerWidth);
        const duration = this.randomRange(4, 8);
        const delay = this.randomRange(0, 2);
        
        // Aplicar estilos
        particle.style.left = `${startX}px`;
        particle.style.bottom = '-20px';
        particle.style.width = `${size}px`;
        particle.style.height = `${size}px`;
        particle.style.animationDuration = `${duration}s`;
        particle.style.animationDelay = `${delay}s`;
        
        // Tipo de partícula
        if (type === 'star') {
            particle.innerHTML = '⭐';
            particle.style.fontSize = `${size}px`;
            particle.style.background = 'none';
        } else if (type === 'heart') {
            particle.innerHTML = '💕';
            particle.style.fontSize = `${size}px`;
            particle.style.background = 'none';
        } else if (type === 'sparkle') {
            particle.innerHTML = '✨';
            particle.style.fontSize = `${size}px`;
            particle.style.background = 'none';
        } else {
            // Círculo con gradiente
            particle.style.background = `radial-gradient(circle, ${color} 0%, transparent 70%)`;
            particle.style.boxShadow = `0 0 ${size}px ${color}`;
        }
        
        // Agregar al contenedor
        this.container.appendChild(particle);
        this.particles.push(particle);
        
        // Remover cuando termine la animación
        particle.addEventListener('animationend', () => {
            this.removeParticle(particle);
        });
    }

    /**
     * Remueve una partícula
     * @param {HTMLElement} particle 
     */
    removeParticle(particle) {
        const index = this.particles.indexOf(particle);
        if (index > -1) {
            this.particles.splice(index, 1);
        }
        particle.remove();
    }

    /**
     * Crea una explosión de partículas en una posición
     * @param {number} x - Posición X
     * @param {number} y - Posición Y
     * @param {number} count - Cantidad de partículas
     */
    burst(x, y, count = 10) {
        for (let i = 0; i < count; i++) {
            const particle = document.createElement('div');
            particle.className = 'particle burst';
            
            const type = this.randomChoice(['⭐', '✨', '💫', '🌟', '💕', '💖']);
            const size = this.randomRange(15, 30);
            const angle = (Math.PI * 2 * i) / count;
            const velocity = this.randomRange(50, 150);
            const dx = Math.cos(angle) * velocity;
            const dy = Math.sin(angle) * velocity;
            
            particle.innerHTML = type;
            particle.style.fontSize = `${size}px`;
            particle.style.left = `${x}px`;
            particle.style.top = `${y}px`;
            particle.style.position = 'fixed';
            particle.style.pointerEvents = 'none';
            particle.style.zIndex = '9999';
            
            this.container.appendChild(particle);
            
            // Animar
            particle.animate([
                { 
                    transform: 'translate(0, 0) scale(1) rotate(0deg)', 
                    opacity: 1 
                },
                { 
                    transform: `translate(${dx}px, ${dy}px) scale(0.5) rotate(360deg)`, 
                    opacity: 0 
                }
            ], {
                duration: 1000,
                easing: 'cubic-bezier(0.25, 0.46, 0.45, 0.94)'
            }).onfinish = () => {
                particle.remove();
            };
        }
    }

    /**
     * Crea confeti desde arriba
     */
    confetti() {
        const count = 30;
        for (let i = 0; i < count; i++) {
            setTimeout(() => {
                const particle = document.createElement('div');
                particle.className = 'particle confetti';
                
                const emojis = ['🎉', '🎊', '✨', '⭐', '💫', '🌟', '💕', '💖', '🎈'];
                const emoji = this.randomChoice(emojis);
                const startX = this.randomRange(0, window.innerWidth);
                const size = this.randomRange(20, 40);
                const duration = this.randomRange(2, 4);
                const swing = this.randomRange(-100, 100);
                
                particle.innerHTML = emoji;
                particle.style.fontSize = `${size}px`;
                particle.style.left = `${startX}px`;
                particle.style.top = '-50px';
                particle.style.position = 'fixed';
                particle.style.pointerEvents = 'none';
                particle.style.zIndex = '9999';
                
                this.container.appendChild(particle);
                
                // Animar caída con balanceo
                particle.animate([
                    { 
                        transform: 'translateY(0) translateX(0) rotate(0deg)', 
                        opacity: 1 
                    },
                    { 
                        transform: `translateY(${window.innerHeight + 50}px) translateX(${swing}px) rotate(${this.randomRange(360, 720)}deg)`, 
                        opacity: 0.8 
                    }
                ], {
                    duration: duration * 1000,
                    easing: 'cubic-bezier(0.25, 0.46, 0.45, 0.94)'
                }).onfinish = () => {
                    particle.remove();
                };
            }, i * 50);
        }
    }

    /**
     * Crea un rastro de partículas siguiendo el cursor
     * @param {number} x 
     * @param {number} y 
     */
    trail(x, y) {
        const particle = document.createElement('div');
        particle.className = 'particle trail';
        
        const emojis = ['✨', '⭐', '💫'];
        const emoji = this.randomChoice(emojis);
        const size = this.randomRange(10, 20);
        
        particle.innerHTML = emoji;
        particle.style.fontSize = `${size}px`;
        particle.style.left = `${x}px`;
        particle.style.top = `${y}px`;
        particle.style.position = 'fixed';
        particle.style.pointerEvents = 'none';
        particle.style.zIndex = '9999';
        
        this.container.appendChild(particle);
        
        // Animar desvanecimiento
        particle.animate([
            { 
                transform: 'translate(0, 0) scale(1)', 
                opacity: 1 
            },
            { 
                transform: `translate(${this.randomRange(-20, 20)}px, ${this.randomRange(-20, 20)}px) scale(0)`, 
                opacity: 0 
            }
        ], {
            duration: 800,
            easing: 'ease-out'
        }).onfinish = () => {
            particle.remove();
        };
    }

    /**
     * Obtiene un valor aleatorio en un rango
     * @param {number} min 
     * @param {number} max 
     */
    randomRange(min, max) {
        return Math.random() * (max - min) + min;
    }

    /**
     * Obtiene un elemento aleatorio de un array
     * @param {Array} array 
     */
    randomChoice(array) {
        return array[Math.floor(Math.random() * array.length)];
    }

    /**
     * Detiene el sistema de partículas
     */
    stop() {
        this.particles.forEach(p => p.remove());
        this.particles = [];
    }
}

// Crear instancia global
let particleSystem;

// Inicializar cuando el DOM esté listo
document.addEventListener('DOMContentLoaded', () => {
    particleSystem = new ParticleSystem();
    
    // Easter egg: confeti al hacer triple click en el personaje
    let clickCount = 0;
    let clickTimer;
    
    const character = document.getElementById('character');
    if (character) {
        character.addEventListener('click', () => {
            clickCount++;
            
            if (clickCount === 3) {
                particleSystem.confetti();
                assistant.celebrate();
                clickCount = 0;
            }
            
            clearTimeout(clickTimer);
            clickTimer = setTimeout(() => {
                clickCount = 0;
            }, 500);
        });
    }
    
    // Easter egg: rastro de partículas con Ctrl + movimiento del mouse
    let trailEnabled = false;
    
    document.addEventListener('keydown', (e) => {
        if (e.key === 'Control') {
            trailEnabled = true;
        }
    });
    
    document.addEventListener('keyup', (e) => {
        if (e.key === 'Control') {
            trailEnabled = false;
        }
    });
    
    document.addEventListener('mousemove', (e) => {
        if (trailEnabled && Math.random() > 0.7) { // Solo 30% del tiempo para no saturar
            particleSystem.trail(e.clientX, e.clientY);
        }
    });
    
    console.log('✨ Sistema de partículas iniciado!');
    console.log('💡 Easter eggs:');
    console.log('   - Triple click en el robot = confeti 🎉');
    console.log('   - Mantén Ctrl y mueve el mouse = rastro mágico ✨');
});
