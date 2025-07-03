// ReefControl Dashboard Widget Helper
// Versão simplificada sem Web Components

class DashboardWidget {
    constructor(config) {
        this.config = config;
        this.chart = null;
        this.data = [];
        this.element = null;
        this.min = Infinity;
        this.max = -Infinity;
    }

    create() {
        this.element = document.createElement('div');
        this.element.className = 'widget-card';
        this.element.innerHTML = `
            <div class="widget-header">
                <div class="widget-title">
                    <span class="widget-icon">${this.getIcon()}</span>
                    <h3>${this.config.title}</h3>
                </div>
                <span class="widget-status ${this.config.status || 'offline'}"></span>
            </div>
            <div class="widget-body">
                <div class="widget-value">
                    <span class="value">--</span>
                    <span class="unit">${this.config.unit}</span>
                </div>
                <div class="widget-chart">
                    <canvas id="chart-${this.config.type}" width="300" height="150"></canvas>
                </div>
            </div>
            <div class="widget-footer">
                <div class="widget-stats">
                    <span class="min">Min: --</span>
                    <span class="max">Max: --</span>
                </div>
                <div class="widget-update">
                    <span class="last-update">--</span>
                </div>
            </div>
        `;

        this.initializeChart();
        return this.element;
    }

    getIcon() {
        const icons = {
            'temp': '🌡️',
            'ph': '⚗️',
            'tds': '💧',
            'level': '📏',
            'default': '📊'
        };
        return icons[this.config.type] || icons.default;
    }

    initializeChart() {
        const canvas = this.element.querySelector(`#chart-${this.config.type}`);
        const ctx = canvas.getContext('2d');
        
        this.chart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: [],
                datasets: [{
                    label: this.config.title,
                    data: [],
                    borderColor: this.getChartColor(),
                    backgroundColor: this.getChartColor() + '20',
                    borderWidth: 2,
                    fill: true,
                    tension: 0.4,
                    pointRadius: 0,
                    pointHoverRadius: 4
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                interaction: {
                    intersect: false,
                    mode: 'index'
                },
                scales: {
                    x: {
                        display: false,
                        grid: { display: false }
                    },
                    y: {
                        display: false,
                        grid: { display: false }
                    }
                },
                plugins: {
                    legend: { display: false },
                    tooltip: {
                        backgroundColor: 'rgba(0,0,0,0.8)',
                        titleColor: 'white',
                        bodyColor: 'white',
                        borderColor: this.getChartColor(),
                        borderWidth: 1
                    }
                },
                animation: {
                    duration: 750,
                    easing: 'easeInOutQuart'
                }
            }
        });
    }

    getChartColor() {
        const colors = {
            'temp': '#e74c3c',
            'ph': '#9b59b6',
            'tds': '#3498db',
            'level': '#2ecc71',
            'default': '#34495e'
        };
        return colors[this.config.type] || colors.default;
    }

    updateValue(value, timestamp = new Date()) {
        if (typeof value !== 'number' || isNaN(value)) return;

        // Atualizar valor atual
        const valueElement = this.element.querySelector('.value');
        valueElement.textContent = this.formatValue(value);

        // Atualizar min/max
        this.min = Math.min(this.min, value);
        this.max = Math.max(this.max, value);

        this.element.querySelector('.min').textContent = `Min: ${this.formatValue(this.min)}`;
        this.element.querySelector('.max').textContent = `Max: ${this.formatValue(this.max)}`;

        // Atualizar timestamp
        this.element.querySelector('.last-update').textContent = timestamp.toLocaleTimeString();

        // Adicionar ao gráfico
        this.addToChart(value, timestamp);

        // Atualizar status visual
        this.updateStatus('online');
    }

    formatValue(value) {
        if (this.config.type === 'temp') {
            return value.toFixed(1);
        } else if (this.config.type === 'ph') {
            return value.toFixed(2);
        } else if (this.config.type === 'tds') {
            return Math.round(value);
        } else if (this.config.type === 'level') {
            return Math.round(value);
        }
        return value.toString();
    }

    addToChart(value, timestamp) {
        const timeLabel = timestamp.toLocaleTimeString();
        
        // Adicionar novo ponto
        this.chart.data.labels.push(timeLabel);
        this.chart.data.datasets[0].data.push(value);

        // Manter apenas os últimos 20 pontos
        if (this.chart.data.labels.length > 20) {
            this.chart.data.labels.shift();
            this.chart.data.datasets[0].data.shift();
        }

        // Atualizar gráfico
        this.chart.update('none');
    }

    updateStatus(status) {
        const statusElement = this.element.querySelector('.widget-status');
        statusElement.className = `widget-status ${status}`;
        
        // Atualizar cor do card baseado no status
        const card = this.element;
        card.classList.remove('status-online', 'status-offline', 'status-warning');
        card.classList.add(`status-${status}`);
    }

    simulateData() {
        let value;
        switch (this.config.type) {
            case 'temp':
                value = 25 + Math.random() * 5; // 25-30°C
                break;
            case 'ph':
                value = 7.5 + (Math.random() - 0.5) * 1; // 7.0-8.0
                break;
            case 'tds':
                value = 350 + Math.random() * 100; // 350-450 ppm
                break;
            case 'level':
                value = 80 + Math.random() * 20; // 80-100%
                break;
            default:
                value = Math.random() * 100;
        }
        this.updateValue(value);
        return value;
    }
}

// Gerenciador do Dashboard
class DashboardManager {
    constructor() {
        this.widgets = new Map();
        this.updateInterval = null;
    }

    initialize(containerId) {
        const container = document.getElementById(containerId);
        if (!container) {
            console.error(`Container ${containerId} não encontrado`);
            return;
        }

        // Configuração dos widgets
        const widgetConfigs = [
            { type: 'temp', title: 'Temperatura', unit: '°C' },
            { type: 'ph', title: 'pH', unit: '' },
            { type: 'tds', title: 'TDS', unit: 'ppm' },
            { type: 'level', title: 'Nível', unit: '%' }
        ];

        // Criar widgets
        widgetConfigs.forEach(config => {
            const widget = new DashboardWidget(config);
            const element = widget.create();
            container.appendChild(element);
            this.widgets.set(config.type, widget);
        });

        // Iniciar atualizações
        this.startUpdates();
    }

    startUpdates() {
        // Atualização inicial
        this.updateAllWidgets();

        // Atualizações periódicas
        this.updateInterval = setInterval(() => {
            this.updateAllWidgets();
        }, 5000);
    }

    async updateAllWidgets() {
        try {
            const response = await fetch('/api/sensors');
            const data = await response.json();

            // Atualizar cada widget com os dados correspondentes
            if (data.temp !== undefined) {
                this.widgets.get('temp')?.updateValue(data.temp);
            }
            if (data.ph !== undefined) {
                this.widgets.get('ph')?.updateValue(data.ph);
            }
            if (data.tds !== undefined) {
                this.widgets.get('tds')?.updateValue(data.tds);
            }
            if (data.level !== undefined) {
                this.widgets.get('level')?.updateValue(data.level);
            }

        } catch (error) {
            console.error('Erro ao atualizar widgets:', error);
            // Marcar todos como offline
            this.widgets.forEach(widget => {
                widget.updateStatus('offline');
            });
        }
    }

    simulateAllData() {
        this.widgets.forEach(widget => {
            widget.simulateData();
        });
    }

    destroy() {
        if (this.updateInterval) {
            clearInterval(this.updateInterval);
        }
        this.widgets.clear();
    }
}

// Instância global do dashboard
let dashboardManager = null;

// Função para inicializar o dashboard
function initializeDashboard(containerId = 'dashboardGrid') {
    if (dashboardManager) {
        dashboardManager.destroy();
    }
    
    dashboardManager = new DashboardManager();
    dashboardManager.initialize(containerId);
    
    return dashboardManager;
}

// Função para simular dados (útil para testes)
function simulateDashboardData() {
    if (dashboardManager) {
        dashboardManager.simulateAllData();
    }
}

// Exportar para uso global
window.initializeDashboard = initializeDashboard;
window.simulateDashboardData = simulateDashboardData; 