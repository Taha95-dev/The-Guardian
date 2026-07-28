// ── Checkout Modal ──
const modal = document.getElementById('checkout-modal');
const modalClose = document.querySelector('.modal-close');

// Open modal on buy button clicks
function openModal(e) {
    e.preventDefault();
    modal.classList.add('active');
}

document.querySelectorAll('#hero-buy, #pricing-buy').forEach(btn => {
    btn.addEventListener('click', openModal);
});

// Close modal
modalClose.addEventListener('click', function() {
    modal.classList.remove('active');
});

// Close on outside click
modal.addEventListener('click', function(e) {
    if (e.target === modal) {
        modal.classList.remove('active');
    }
});

// Close on Escape key
document.addEventListener('keydown', function(e) {
    if (e.key === 'Escape' && modal.classList.contains('active')) {
        modal.classList.remove('active');
    }
});

// Proceed to Polar checkout
document.getElementById('modal-buy').addEventListener('click', function() {
    window.location.href = 'https://buy.polar.sh/polar_cl_czZ7LW2XcJR1zNjUoiGbBYU4MNOI4ShcvcR6l00HLU9';
});

// ── Smooth Scroll ──
document.querySelectorAll('a[href^="#"]').forEach(anchor => {
    anchor.addEventListener('click', function(e) {
        e.preventDefault();
        const target = document.querySelector(this.getAttribute('href'));
        if (target) {
            target.scrollIntoView({ behavior: 'smooth' });
        }
    });
});

// ── Navbar Shrink on Scroll ──
const navbar = document.querySelector('.navbar');
if (navbar) {
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
}

console.log('🔷 The Guardian website loaded successfully!');
console.log('📦 Built by a 13-year-old developer.');
console.log('🛒 Click "Buy Now" to purchase The Guardian.');

// ── Analytics ──
const analytics = {
    // Track page views
    pageView: function(page) {
        console.log(`📊 Page View: ${page}`);
        this.sendEvent('page_view', { page });
    },
    
    // Track events
    event: function(name, properties = {}) {
        console.log(`📊 Event: ${name}`, properties);
        this.sendEvent(name, properties);
    },
    
    // Send to server (localStorage for now)
    sendEvent: function(name, properties) {
        const data = {
            event: name,
            properties: properties,
            timestamp: new Date().toISOString(),
            url: window.location.href,
            userAgent: navigator.userAgent
        };
        
        // Store in localStorage for the analytics dashboard
        const events = JSON.parse(localStorage.getItem('guardian_analytics') || '[]');
        events.push(data);
        localStorage.setItem('guardian_analytics', JSON.stringify(events));
        
        console.log('📊 Analytics:', data);
    }
};

// ── Track Events ──

// Page view
analytics.pageView(window.location.pathname);

// Track playground usage
document.addEventListener('DOMContentLoaded', function() {
    // Track when playground is loaded
    if (window.location.pathname.includes('playground')) {
        analytics.event('playground_loaded');
    }
    
    // Track run button clicks
    const runBtn = document.getElementById('run-btn');
    if (runBtn) {
        runBtn.addEventListener('click', function() {
            analytics.event('playground_run', {
                codeLength: editor ? editor.getValue().length : 0
            });
        });
    }
    
    // Track example usage
    document.querySelectorAll('.example-btn').forEach(btn => {
        btn.addEventListener('click', function() {
            analytics.event('playground_example', {
                example: this.dataset.example
            });
        });
    });
});

// Track buy button clicks
document.querySelectorAll('#hero-buy, #pricing-buy, #modal-buy').forEach(btn => {
    if (btn) {
        btn.addEventListener('click', function() {
            analytics.event('buy_click', {
                source: this.id || 'unknown'
            });
        });
    }
});

// Track consulting page visits
if (window.location.pathname.includes('consulting')) {
    analytics.event('consulting_page_view');
}

// Track outbound links
document.querySelectorAll('a[href^="http"]').forEach(link => {
    link.addEventListener('click', function() {
        analytics.event('outbound_link', {
            url: this.href,
            text: this.textContent
        });
    });
});

console.log('📊 Analytics initialized!');
