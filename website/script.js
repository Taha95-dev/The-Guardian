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
