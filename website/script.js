// ── Smooth Scroll ──
document.querySelectorAll('a[href^="#"]').forEach(anchor => {
    anchor.addEventListener('click', function (e) {
        e.preventDefault();
        const target = document.querySelector(this.getAttribute('href'));
        if (target) {
            target.scrollIntoView({ behavior: 'smooth' });
        }
    });
});

// ── Navbar Shrink on Scroll ──
const navbar = document.querySelector('.navbar');
let lastScroll = 0;

window.addEventListener('scroll', () => {
    const currentScroll = window.pageYOffset;
    if (currentScroll > 50) {
        navbar.style.padding = '0.4rem 0';
        navbar.style.background = 'rgba(10, 10, 15, 0.95)';
    } else {
        navbar.style.padding = '0.8rem 0';
        navbar.style.background = 'rgba(10, 10, 15, 0.85)';
    }
    lastScroll = currentScroll;
});

// ── Counter Animation ──
const counters = document.querySelectorAll('.stat-number[data-count]');

const animateCounter = (el) => {
    const target = parseInt(el.getAttribute('data-count'));
    const duration = 2000;
    const startTime = performance.now();

    const updateCounter = (currentTime) => {
        const elapsed = currentTime - startTime;
        const progress = Math.min(elapsed / duration, 1);
        const current = Math.floor(progress * target);
        el.textContent = current;

        if (progress < 1) {
            requestAnimationFrame(updateCounter);
        } else {
            el.textContent = target;
        }
    };

    requestAnimationFrame(updateCounter);
};

// ── Intersection Observer for counters ──
const observer = new IntersectionObserver((entries) => {
    entries.forEach(entry => {
        if (entry.isIntersecting) {
            const el = entry.target;
            if (el.getAttribute('data-count') && !el.classList.contains('animated')) {
                el.classList.add('animated');
                animateCounter(el);
            }
        }
    });
}, { threshold: 0.3 });

counters.forEach(counter => observer.observe(counter));

// ── Feature Cards Hover Effect ──
document.querySelectorAll('.feature-card').forEach(card => {
    card.addEventListener('mousemove', (e) => {
        const rect = card.getBoundingClientRect();
        const x = (e.clientX - rect.left) / rect.width;
        const y = (e.clientY - rect.top) / rect.height;
        card.style.setProperty('--x', x);
        card.style.setProperty('--y', y);
    });
});

// ── Buy Button Alert (temporary) ──
document.querySelector('.buy-button')?.addEventListener('click', (e) => {
    e.preventDefault();
    alert('🚀 The Guardian is coming soon!\n\nJoin the waitlist or check back later.');
});

console.log('🔷 The Guardian website loaded successfully!');
console.log('📦 Built by a 13-year-old developer.');
