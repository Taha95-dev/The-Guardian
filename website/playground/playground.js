// ── CodeMirror Editor ──
const editor = CodeMirror.fromTextArea(document.getElementById('axiom-editor'), {
    mode: 'javascript',
    theme: 'material-darker',
    lineNumbers: true,
    indentUnit: 4,
    tabSize: 4,
    indentWithTabs: false,
    keyMap: 'vim',
    extraKeys: {
        'Ctrl-Enter': runCode,
        'Cmd-Enter': runCode,
        'Ctrl-S': function(cm) { cm.save(); },
    },
    vimMode: true,
});

// ── Output Elements ──
const outputContent = document.getElementById('output-content');
const statusBadge = document.getElementById('status-badge');
const executionTime = document.getElementById('execution-time');

// ── Example Code ──
const examples = {
    hello: `// Hello World in Axiom
println("Hello, Axiom!")
println("Welcome to The Guardian Playground!")`,

    variables: `// Variables in Axiom
let x = 42
let y = 3.14
let name = "Axiom"
let active = true

println("x: " + x)
println("y: " + y)
println("name: " + name)
println("active: " + active)`,

    types: `// Types in Axiom
let int_val = 42
let float_val = 3.14159
let string_val = "Hello, World!"
let bool_val = true

println("Integer: " + int_val)
println("Float: " + float_val)
println("String: " + string_val)
println("Bool: " + bool_val)`,

    if: `// If/Else in Axiom
let x = 42
let y = 10

if (x > y) {
    println("x is greater than y")
} else {
    println("x is less than or equal to y")
}

// More examples
let age = 13
if (age < 18) {
    println("You're a teenager!")
} else {
    println("You're an adult!")
}`,

    loop: `// While Loop in Axiom
let i = 0

while (i < 5) {
    println("i = " + i)
    i = i + 1
}

println("Done!")`,
};

// ── Load Example ──
document.querySelectorAll('.example-btn').forEach(btn => {
    btn.addEventListener('click', function() {
        const exampleName = this.dataset.example;
        if (examples[exampleName]) {
            editor.setValue(examples[exampleName]);
            // Reset output
            outputContent.textContent = '// Loaded example. Click "Run" to execute.';
            statusBadge.className = 'status-badge idle';
            statusBadge.textContent = 'Ready';
        }
    });
});

// ── Clear Output ──
document.getElementById('clear-btn').addEventListener('click', function() {
    outputContent.textContent = '// Output cleared. Click "Run" to execute your code.';
    statusBadge.className = 'status-badge idle';
    statusBadge.textContent = 'Ready';
    executionTime.textContent = '⏱️ Ready';
});

// ── Fullscreen ──
document.getElementById('fullscreen-btn').addEventListener('click', function() {
    const editorSection = document.querySelector('.editor-section');
    if (editorSection.requestFullscreen) {
        editorSection.requestFullscreen();
    }
});

// ── Run Code ──
document.getElementById('run-btn').addEventListener('click', runCode);

async function runCode() {
    const code = editor.getValue();
    if (!code.trim()) {
        outputContent.textContent = '// Please enter some Axiom code.';
        return;
    }

    // Update status
    statusBadge.className = 'status-badge running';
    statusBadge.textContent = 'Running...';
    executionTime.textContent = '⏱️ Running...';
    outputContent.textContent = '// Compiling and executing...\n';

    const startTime = performance.now();

    try {
        // ── Send to backend ──
        // For now, we'll simulate execution with a delay
        // TODO: Connect to actual Axiom compiler/VM
        
        // Simulate compilation and execution
        await new Promise(resolve => setTimeout(resolve, 500));
        
        // For demo, we'll just show a mock output
        const mockOutput = simulateAxiom(code);
        
        const endTime = performance.now();
        const elapsed = ((endTime - startTime) / 1000).toFixed(3);
        
        outputContent.textContent = mockOutput;
        statusBadge.className = 'status-badge success';
        statusBadge.textContent = '✅ Success';
        executionTime.textContent = `⏱️ ${elapsed}s`;

    } catch (error) {
        outputContent.textContent = `// Error: ${error.message}`;
        statusBadge.className = 'status-badge error';
        statusBadge.textContent = '❌ Error';
        executionTime.textContent = '⏱️ Failed';
    }
}

// ── Mock Axiom Interpreter (for demo) ──
function simulateAxiom(code) {
    const lines = code.split('\n');
    const output = [];
    const variables = {};
    
    for (let line of lines) {
        line = line.trim();
        if (!line || line.startsWith('//')) continue;
        
        // Handle let statements
        if (line.startsWith('let ')) {
            const parts = line.replace('let ', '').split(' = ');
            if (parts.length === 2) {
                const name = parts[0].trim();
                let value = parts[1].trim();
                
                // Parse value
                if (value === 'true') {
                    variables[name] = true;
                } else if (value === 'false') {
                    variables[name] = false;
                } else if (!isNaN(parseFloat(value))) {
                    variables[name] = parseFloat(value);
                } else if (value.startsWith('"') && value.endsWith('"')) {
                    variables[name] = value.slice(1, -1);
                }
                output.push(`> ${name} = ${variables[name]}`);
            }
        }
        
        // Handle println
        else if (line.startsWith('println(')) {
            const content = line.slice(8, -1).trim();
            const result = evaluateExpression(content, variables);
            output.push(result);
        }
        
        // Handle print
        else if (line.startsWith('print(')) {
            const content = line.slice(6, -1).trim();
            const result = evaluateExpression(content, variables);
            output.push(result);
        }
        
        // Handle if
        else if (line.startsWith('if ')) {
            // Simple mock - just execute the body
            output.push('// if statement (simulated)');
        }
        
        // Handle while
        else if (line.startsWith('while ')) {
            output.push('// while loop (simulated)');
        }
    }
    
    return output.join('\n');
}

function evaluateExpression(expr, variables) {
    // Handle string concatenation
    const parts = expr.split(' + ');
    if (parts.length > 1) {
        return parts.map(p => {
            p = p.trim();
            if (p.startsWith('"') && p.endsWith('"')) {
                return p.slice(1, -1);
            }
            return variables[p] !== undefined ? variables[p] : p;
        }).join('');
    }
    
    // Handle variable lookup
    if (variables[expr] !== undefined) {
        return String(variables[expr]);
    }
    
    // Handle string literal
    if (expr.startsWith('"') && expr.endsWith('"')) {
        return expr.slice(1, -1);
    }
    
    return expr;
}

// ── Keyboard Shortcuts ──
document.addEventListener('keydown', function(e) {
    if ((e.ctrlKey || e.metaKey) && e.key === 'Enter') {
        e.preventDefault();
        runCode();
    }
});

console.log('🔷 The Guardian Playground loaded!');
console.log('📝 Write Axiom code and click "Run" to execute.');
console.log('⌨️  Press Ctrl+Enter to run.');
