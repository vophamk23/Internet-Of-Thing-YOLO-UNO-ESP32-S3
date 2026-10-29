#pragma once
#include <Arduino.h>

// =============================================================
// HTML Dashboard Page (PROGMEM) - Official CoreIOT Exact Theme
// =============================================================
const char PAGE_MAIN[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="vi">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>CoreIOT Dashboard - ESP32</title>
  <style>
    :root {
      --core-blue: #0288d1;
      --core-blue-hover: #0277bd;
      --bg-page: #f4f6f9;
      --card-bg: #ffffff;
      --card-border: #e2e8f0;
      --text-main: #1e293b;
      --text-muted: #64748b;
      --text-unit: #64748b; /* Don vi giu mau trung tinh co dinh */
      --track-bg: #e2e8f0;
    }
    * { box-sizing: border-box; margin: 0; padding: 0; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif; }
    body {
      background-color: var(--bg-page);
      color: var(--text-main);
      min-height: 100vh;
      display: flex;
      flex-direction: column;
    }
    
    /* Official CoreIOT Top Navigation Bar */
    .topbar {
      background: #0288d1;
      color: #ffffff;
      padding: 0 20px;
      height: 56px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      box-shadow: 0 2px 8px rgba(2, 136, 209, 0.25);
      position: sticky;
      top: 0;
      z-index: 100;
    }
    /* Exact CoreIOT Logo from user reference */
    .brand {
      display: flex;
      align-items: center;
      gap: 10px;
      text-decoration: none;
    }
    .brand-svg {
      width: 28px;
      height: 28px;
      color: #ffffff;
      flex-shrink: 0;
    }
    .brand-title {
      font-size: 1.3rem;
      font-weight: 900;
      letter-spacing: 0.05em;
      color: #ffffff;
      text-transform: uppercase;
      font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
    }
    .top-actions {
      display: flex;
      align-items: center;
      gap: 10px;
    }
    .btn-settings-nav {
      display: inline-flex;
      align-items: center;
      gap: 6px;
      background: rgba(255, 255, 255, 0.16);
      border: 1px solid rgba(255, 255, 255, 0.3);
      color: #ffffff;
      padding: 7px 14px;
      border-radius: 8px;
      font-size: 0.82rem;
      font-weight: 600;
      text-decoration: none;
      transition: all 0.2s ease;
    }
    .btn-settings-nav:hover {
      background: rgba(255, 255, 255, 0.28);
      transform: translateY(-1px);
    }
    
    /* Main Content Area */
    .main-wrapper {
      width: 100%;
      max-width: 540px;
      margin: 20px auto 40px;
      padding: 0 14px;
      display: flex;
      flex-direction: column;
      gap: 16px;
    }
    
    /* System Status Bar */
    .status-strip {
      background: #ffffff;
      border: 1px solid var(--card-border);
      border-radius: 12px;
      padding: 10px 16px;
      display: flex;
      align-items: center;
      justify-content: space-between;
      font-size: 0.8rem;
      color: var(--text-muted);
      box-shadow: 0 1px 3px rgba(0, 0, 0, 0.03);
    }
    .status-left {
      display: flex;
      align-items: center;
      gap: 8px;
      font-weight: 600;
      color: var(--text-main);
    }
    .pulse-dot {
      width: 8px;
      height: 8px;
      background: #10b981;
      border-radius: 50%;
      animation: pulse 2s infinite;
    }
    @keyframes pulse {
      0% { box-shadow: 0 0 0 0 rgba(16, 185, 129, 0.7); }
      70% { box-shadow: 0 0 0 6px rgba(16, 185, 129, 0); }
      100% { box-shadow: 0 0 0 0 rgba(16, 185, 129, 0); }
    }
    .ap-tag {
      font-size: 0.75rem;
      background: #e0f2fe;
      color: #0369a1;
      padding: 3px 8px;
      border-radius: 6px;
      font-weight: 700;
    }

    /* Telemetry Grid */
    .telemetry-grid {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 14px;
    }
    @media (max-width: 440px) {
      .telemetry-grid { grid-template-columns: 1fr; }
    }

    /* Telemetry Card */
    .card {
      background: var(--card-bg);
      border: 1px solid var(--card-border);
      border-radius: 14px;
      padding: 18px 16px;
      box-shadow: 0 2px 10px rgba(0, 0, 0, 0.04);
      display: flex;
      flex-direction: column;
      justify-content: space-between;
      transition: all 0.2s ease;
    }
    .card:hover {
      box-shadow: 0 6px 18px rgba(0, 0, 0, 0.07);
      transform: translateY(-2px);
    }

    /* Metric Label - Made Bold & Clearer */
    .card-header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      margin-bottom: 12px;
    }
    .card-title {
      font-size: 0.92rem;
      font-weight: 800;
      color: #0f172a;
      letter-spacing: -0.01em;
      display: flex;
      align-items: center;
      gap: 6px;
    }
    .card-icon-wrap {
      width: 30px;
      height: 30px;
      border-radius: 8px;
      display: flex;
      align-items: center;
      justify-content: center;
    }
    .icon-temp { background: #fff7ed; color: #ea580c; border: 1px solid #fed7aa; }
    .icon-hum  { background: #f0fdf4; color: #0284c7; border: 1px solid #bae6fd; }

    /* Metric Numbers - Dynamically Coloured / Unit Stays Neutral */
    .metric-value-box {
      display: flex;
      align-items: baseline;
      gap: 4px;
      margin-bottom: 2px;
    }
    .metric-num {
      font-size: 2.35rem;
      font-weight: 800;
      letter-spacing: -0.02em;
      line-height: 1.1;
      font-variant-numeric: tabular-nums;
      color: #059669;
      transition: color 0.3s ease;
    }
    .metric-unit {
      font-size: 1.15rem;
      font-weight: 700;
      color: var(--text-unit); /* Giu co dinh mau xam chuan */
    }
    .metric-sub {
      font-size: 0.74rem;
      font-weight: 500;
      color: #94a3b8;
      margin-bottom: 12px;
    }

    /* Status Badge Chip */
    .status-badge {
      display: inline-flex;
      align-items: center;
      gap: 6px;
      padding: 5px 11px;
      border-radius: 20px;
      font-size: 0.78rem;
      font-weight: 700;
      margin-bottom: 12px;
      border: 1px solid transparent;
      transition: all 0.3s ease;
      width: fit-content;
    }
    .badge-ok {
      background: #ecfdf5;
      color: #059669;
      border-color: #a7f3d0;
    }
    .badge-caution {
      background: #fffbeb;
      color: #d97706;
      border-color: #fde68a;
    }
    .badge-warning {
      background: #fff7ed;
      color: #ea580c;
      border-color: #fed7aa;
    }
    .badge-danger {
      background: #fef2f2;
      color: #dc2626;
      border-color: #fecaca;
      animation: pulseDanger 1.2s infinite;
    }
    @keyframes pulseDanger {
      0%, 100% { opacity: 1; transform: scale(1); }
      50% { opacity: 0.7; transform: scale(0.98); }
    }

    /* Gauge Bar */
    .gauge-track {
      width: 100%;
      height: 7px;
      background: var(--track-bg);
      border-radius: 8px;
      overflow: hidden;
      margin-top: 4px;
    }
    .gauge-fill {
      height: 100%;
      width: 0%;
      border-radius: 8px;
      transition: width 0.5s ease, background 0.3s ease;
    }
    .gauge-labels {
      display: flex;
      justify-content: space-between;
      font-size: 0.68rem;
      font-weight: 600;
      color: #94a3b8;
      margin-top: 4px;
    }

    /* Actuators Control Card */
    .controls-card {
      background: var(--card-bg);
      border: 1px solid var(--card-border);
      border-radius: 14px;
      padding: 18px 20px;
      box-shadow: 0 2px 10px rgba(0, 0, 0, 0.04);
    }
    .section-title {
      font-size: 0.92rem;
      font-weight: 800;
      color: #0f172a;
      letter-spacing: -0.01em;
      margin-bottom: 14px;
      display: flex;
      align-items: center;
      gap: 6px;
    }
    .led-grid {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 12px;
    }
    @media (max-width: 440px) {
      .led-grid { grid-template-columns: 1fr; }
    }
    .led-box {
      background: #f8fafc;
      border: 1px solid var(--card-border);
      border-radius: 12px;
      padding: 12px 14px;
      display: flex;
      align-items: center;
      justify-content: space-between;
      transition: all 0.25s ease;
    }
    .led-box.active {
      background: #f0fdf4;
      border-color: #86efac;
      box-shadow: 0 2px 8px rgba(34, 197, 94, 0.15);
    }
    .led-info {
      display: flex;
      align-items: center;
      gap: 10px;
    }
    .bulb-icon {
      width: 34px;
      height: 34px;
      border-radius: 8px;
      background: #e2e8f0;
      color: #94a3b8;
      display: flex;
      align-items: center;
      justify-content: center;
      transition: all 0.25s ease;
    }
    .led-box.active .bulb-icon {
      background: #fef08a;
      color: #ca8a04;
      box-shadow: 0 2px 10px rgba(202, 138, 4, 0.35);
    }
    .led-name {
      font-size: 0.88rem;
      font-weight: 700;
      color: var(--text-main);
    }
    .led-status-text {
      font-size: 0.74rem;
      font-weight: 600;
      color: #94a3b8;
    }
    .led-box.active .led-status-text {
      color: #16a34a;
    }
    /* Switch Button */
    .switch-btn {
      position: relative;
      width: 48px;
      height: 26px;
      background: #cbd5e1;
      border-radius: 20px;
      cursor: pointer;
      border: none;
      outline: none;
      transition: background 0.25s ease;
      padding: 0;
    }
    .switch-btn::after {
      content: '';
      position: absolute;
      top: 3px;
      left: 3px;
      width: 20px;
      height: 20px;
      background: #ffffff;
      border-radius: 50%;
      transition: transform 0.25s cubic-bezier(0.34, 1.56, 0.64, 1);
      box-shadow: 0 1px 3px rgba(0, 0, 0, 0.2);
    }
    .switch-btn.active {
      background: var(--core-blue);
    }
    .switch-btn.active::after {
      transform: translateX(22px);
    }
  </style>
</head>
<body>
  <!-- CoreIOT Top Navigation Bar -->
  <header class="topbar">
    <div class="brand">
      <!-- Official CoreIOT Chip Wi-Fi Logo -->
      <svg class="brand-svg" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
        <rect x="5" y="5" width="14" height="14" rx="2.5"></rect>
        <line x1="9" y1="2" x2="9" y2="5"></line>
        <line x1="15" y1="2" x2="15" y2="5"></line>
        <line x1="9" y1="19" x2="9" y2="22"></line>
        <line x1="15" y1="19" x2="15" y2="22"></line>
        <line x1="2" y1="9" x2="5" y2="9"></line>
        <line x1="2" y1="15" x2="5" y2="15"></line>
        <line x1="19" y1="9" x2="22" y2="9"></line>
        <line x1="19" y1="15" x2="22" y2="15"></line>
        <!-- Wi-Fi Waves Inside Chip -->
        <path d="M8.5 11a4.8 4.8 0 0 1 7 0" stroke-width="1.8"></path>
        <path d="M10.2 13a2.4 2.4 0 0 1 3.6 0" stroke-width="1.8"></path>
        <circle cx="12" cy="15.2" r="0.9" fill="currentColor"></circle>
      </svg>
      <div class="brand-title">CORE IOT</div>
    </div>
    <div class="top-actions">
      <a href="/settings" class="btn-settings-nav">
        <svg width="15" height="15" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="3"></circle><path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 0 1 0 2.83 2 2 0 0 1-2.83 0l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-2 2 2 2 0 0 1-2-2v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 0 1-2.83 0 2 2 0 0 1 0-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1-2-2 2 2 0 0 1 2-2h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 0 1 0-2.83 2 2 0 0 1 2.83 0l.06.06a1.65 1.65 0 0 0 1.82.33H9a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 2-2 2 2 0 0 1 2 2v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 0 1 2.83 0 2 2 0 0 1 0 2.83l-.06.06a1.65 1.65 0 0 0-.33 1.82V9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 2 2 2 2 0 0 1-2 2h-.09a1.65 1.65 0 0 0-1.51 1z"></path></svg>
        <span>Cài đặt Wi-Fi</span>
      </a>
    </div>
  </header>

  <!-- Main Container -->
  <div class="main-wrapper">
    <!-- Live Status Strip -->
    <div class="status-strip">
      <div class="status-left">
        <div class="pulse-dot"></div>
        <span>Giám sát thời gian thực</span>
      </div>
      <span class="ap-tag">AP: 192.168.4.1</span>
    </div>

    <!-- Telemetry Cards -->
    <div class="telemetry-grid">
      <!-- Temperature Card -->
      <div class="card">
        <div>
          <div class="card-header">
            <span class="card-title">
              <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round"><path d="M14 14.76V3.5a2.5 2.5 0 0 0-5 0v11.26a4.5 4.5 0 1 0 5 0z"></path></svg>
              Nhiệt độ
            </span>
            <div class="card-icon-wrap icon-temp">
              <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><circle cx="12" cy="12" r="5"></circle><line x1="12" y1="1" x2="12" y2="3"></line><line x1="12" y1="21" x2="12" y2="23"></line></svg>
            </div>
          </div>
          
          <div class="metric-value-box">
            <span id="temp" class="metric-num">--.--</span>
            <span class="metric-unit">&deg;C</span>
          </div>
          <div class="metric-sub">Trung bình trượt 10 mẫu</div>
        </div>

        <div>
          <div id="temp_badge" class="status-badge badge-ok">
            <span>&bull;</span> <span id="temp_badge_text">Đang đo...</span>
          </div>
          <div class="gauge-track">
            <div id="temp_bar" class="gauge-fill" style="width: 30%; background: #059669;"></div>
          </div>
          <div class="gauge-labels">
            <span>20&deg;C</span>
            <span>50&deg;C</span>
          </div>
        </div>
      </div>

      <!-- Humidity Card -->
      <div class="card">
        <div>
          <div class="card-header">
            <span class="card-title">
              <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round"><path d="M12 2.69l5.66 5.66a8 8 0 1 1-11.31 0z"></path></svg>
              Độ ẩm
            </span>
            <div class="card-icon-wrap icon-hum">
              <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M12 2v20"></path><path d="m17 5-5-3-5 3"></path></svg>
            </div>
          </div>
          
          <div class="metric-value-box">
            <span id="hum" class="metric-num">--.--</span>
            <span class="metric-unit">%</span>
          </div>
          <div class="metric-sub">Dải trễ 1% Hysteresis</div>
        </div>

        <div>
          <div id="hum_badge" class="status-badge badge-ok">
            <span>&bull;</span> <span id="hum_badge_text">Đang đo...</span>
          </div>
          <div class="gauge-track">
            <div id="hum_bar" class="gauge-fill" style="width: 50%; background: #0284c7;"></div>
          </div>
          <div class="gauge-labels">
            <span>0%</span>
            <span>100%</span>
          </div>
        </div>
      </div>
    </div>

    <!-- Actuators Control Card -->
    <div class="controls-card">
      <div class="section-title">
        <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="2" y="6" width="20" height="12" rx="6"></rect><circle cx="8" cy="12" r="2"></circle></svg>
        Điều khiển thiết bị (Actuators)
      </div>
      <div class="led-grid">
        <!-- LED 1 -->
        <div id="boxLed1" class="led-box">
          <div class="led-info">
            <div class="bulb-icon">
              <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M9 18h6"></path><path d="M10 22h4"></path><path d="M15.09 14c.18-.98.65-1.74 1.41-2.5A4.65 4.65 0 0 0 18 8 6 6 0 0 0 6 8c0 1 .23 2.23 1.5 3.5A4.61 4.61 0 0 1 8.91 14"></path></svg>
            </div>
            <div>
              <div class="led-name">Đèn LED 1</div>
              <div id="txtLed1" class="led-status-text">ĐANG TẮT</div>
            </div>
          </div>
          <button id="btnLed1" class="switch-btn" onclick="toggleLed(1)" aria-label="Bật tắt LED 1"></button>
        </div>

        <!-- LED 2 -->
        <div id="boxLed2" class="led-box">
          <div class="led-info">
            <div class="bulb-icon">
              <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M9 18h6"></path><path d="M10 22h4"></path><path d="M15.09 14c.18-.98.65-1.74 1.41-2.5A4.65 4.65 0 0 0 18 8 6 6 0 0 0 6 8c0 1 .23 2.23 1.5 3.5A4.61 4.61 0 0 1 8.91 14"></path></svg>
            </div>
            <div>
              <div class="led-name">Đèn LED 2</div>
              <div id="txtLed2" class="led-status-text">ĐANG TẮT</div>
            </div>
          </div>
          <button id="btnLed2" class="switch-btn" onclick="toggleLed(2)" aria-label="Bật tắt LED 2"></button>
        </div>
      </div>
    </div>
  </div>

  <script>
    const T_LIMIT = { caution: 30, warning: 35, danger: 40 };

    function updateSensors() {
      fetch('/sensors')
        .then(response => response.json())
        .then(d => {
          let t = d.temp;
          let h = d.hum;
          let h_lvl = d.hum_level || 'ok';

          // ========================================================
          // 1. CẬP NHẬT NHIỆT ĐỘ: CHỈ ĐỔI MÀU SỐ ĐO, ĐƠN VỊ CỐ ĐỊNH
          // ========================================================
          let elTemp = document.getElementById('temp');
          let elTempBadge = document.getElementById('temp_badge');
          let elTempBadgeTxt = document.getElementById('temp_badge_text');
          let elTempBar = document.getElementById('temp_bar');

          elTemp.innerText = t.toFixed(2);
          let t_pct = Math.min(Math.max(((t - 20) / (50 - 20)) * 100, 5), 100);
          elTempBar.style.width = t_pct + '%';

          elTempBadge.className = 'status-badge ';
          if (t >= T_LIMIT.danger) {
            elTemp.style.color = '#dc2626';     // Đỏ nguy hiểm
            elTempBar.style.background = '#dc2626';
            elTempBadge.classList.add('badge-danger');
            elTempBadgeTxt.innerHTML = '&#9888; Nguy hiểm (&ge;40&deg;C)';
          } else if (t >= T_LIMIT.warning) {
            elTemp.style.color = '#ea580c';     // Cam cảnh báo
            elTempBar.style.background = '#ea580c';
            elTempBadge.classList.add('badge-warning');
            elTempBadgeTxt.innerHTML = '&#9888; Cảnh báo (&ge;35&deg;C)';
          } else if (t >= T_LIMIT.caution) {
            elTemp.style.color = '#d97706';     // Vàng chú ý
            elTempBar.style.background = '#d97706';
            elTempBadge.classList.add('badge-caution');
            elTempBadgeTxt.innerText = 'Chú ý (\u226530\u00b0C)';
          } else {
            elTemp.style.color = '#059669';     // Xanh lá bình thường
            elTempBar.style.background = '#059669';
            elTempBadge.classList.add('badge-ok');
            elTempBadgeTxt.innerText = 'Bình thường (<30\u00b0C)';
          }

          // ========================================================
          // 2. CẬP NHẬT ĐỘ ẨM: CHỈ ĐỔI MÀU SỐ ĐO, ĐƠN VỊ CỐ ĐỊNH
          // ========================================================
          let elHum = document.getElementById('hum');
          let elHumBadge = document.getElementById('hum_badge');
          let elHumBadgeTxt = document.getElementById('hum_badge_text');
          let elHumBar = document.getElementById('hum_bar');

          elHum.innerText = h.toFixed(2);
          let h_pct = Math.min(Math.max(h, 2), 100);
          elHumBar.style.width = h_pct + '%';

          elHumBadge.className = 'status-badge ';
          if (h_lvl === 'danger') {
            elHum.style.color = '#dc2626';     // Đỏ nguy hiểm
            elHumBar.style.background = '#dc2626';
            elHumBadge.classList.add('badge-danger');
            elHumBadgeTxt.innerHTML = '&#9888; Nguy hiểm (&ge;70%)';
          } else if (h_lvl === 'humid') {
            elHum.style.color = '#ea580c';     // Cam ẩm
            elHumBar.style.background = '#ea580c';
            elHumBadge.classList.add('badge-warning');
            elHumBadgeTxt.innerHTML = '&#9888; Cảnh báo (\u1ea8m &ge;60%)';
          } else if (h_lvl === 'dry') {
            elHum.style.color = '#d97706';     // Vàng khô
            elHumBar.style.background = '#d97706';
            elHumBadge.classList.add('badge-caution');
            elHumBadgeTxt.innerText = 'Chú ý (Khô <29%)';
          } else {
            elHum.style.color = '#0284c7';     // Xanh dương CoreIOT
            elHumBar.style.background = '#0284c7';
            elHumBadge.classList.add('badge-ok');
            elHumBadgeTxt.innerText = 'Bình thường (30% - 59%)';
          }
        })
        .catch(err => console.log('Sensors error:', err));
    }

    function toggleLed(id) {
      fetch('/toggle?led=' + id)
        .then(response => response.json())
        .then(d => {
          let is1 = (d.led1 === 'ON');
          let is2 = (d.led2 === 'ON');

          let box1 = document.getElementById('boxLed1');
          let btn1 = document.getElementById('btnLed1');
          let txt1 = document.getElementById('txtLed1');
          if (is1) {
            box1.classList.add('active');
            btn1.classList.add('active');
            txt1.innerText = 'ĐANG BẬT';
          } else {
            box1.classList.remove('active');
            btn1.classList.remove('active');
            txt1.innerText = 'ĐANG TẮT';
          }

          let box2 = document.getElementById('boxLed2');
          let btn2 = document.getElementById('btnLed2');
          let txt2 = document.getElementById('txtLed2');
          if (is2) {
            box2.classList.add('active');
            btn2.classList.add('active');
            txt2.innerText = 'ĐANG BẬT';
          } else {
            box2.classList.remove('active');
            btn2.classList.remove('active');
            txt2.innerText = 'ĐANG TẮT';
          }
        });
    }

    setInterval(updateSensors, 2000);
    updateSensors();
  </script>
</body>
</html>)rawliteral";

// =============================================================
// HTML Settings Page (PROGMEM) - CoreIOT Theme
// =============================================================
const char PAGE_SETTINGS[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="vi">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Cấu hình Wi-Fi - CoreIOT</title>
  <style>
    :root {
      --core-blue: #0288d1;
      --bg-page: #f4f6f9;
      --card-bg: #ffffff;
      --card-border: #e2e8f0;
      --text-main: #1e293b;
      --text-muted: #64748b;
    }
    * { box-sizing: border-box; margin: 0; padding: 0; font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif; }
    body {
      background-color: var(--bg-page);
      color: var(--text-main);
      min-height: 100vh;
      display: flex;
      flex-direction: column;
    }
    .topbar {
      background: #0288d1;
      color: #ffffff;
      padding: 0 20px;
      height: 56px;
      display: flex;
      align-items: center;
      gap: 10px;
      box-shadow: 0 2px 8px rgba(2, 136, 209, 0.25);
    }
    .brand-svg {
      width: 28px;
      height: 28px;
      color: #ffffff;
      flex-shrink: 0;
    }
    .brand-title {
      font-size: 1.3rem;
      font-weight: 900;
      letter-spacing: 0.05em;
      color: #ffffff;
      text-transform: uppercase;
    }
    .content-box {
      width: 100%;
      max-width: 440px;
      margin: 30px auto;
      padding: 0 16px;
    }
    .card {
      background: var(--card-bg);
      border: 1px solid var(--card-border);
      border-radius: 14px;
      padding: 26px 22px;
      box-shadow: 0 4px 16px rgba(0, 0, 0, 0.05);
    }
    h2 {
      font-size: 1.25rem;
      font-weight: 800;
      color: #0f172a;
      margin-bottom: 4px;
    }
    .header-desc {
      font-size: 0.8rem;
      color: var(--text-muted);
      margin-bottom: 20px;
    }
    .input-group {
      margin-bottom: 16px;
    }
    label {
      display: block;
      font-size: 0.78rem;
      font-weight: 700;
      letter-spacing: 0.03em;
      text-transform: uppercase;
      color: var(--text-muted);
      margin-bottom: 6px;
    }
    .input-box {
      position: relative;
    }
    input {
      width: 100%;
      padding: 12px 14px 12px 38px;
      background: #f8fafc;
      border: 1px solid #cbd5e1;
      border-radius: 10px;
      color: var(--text-main);
      font-size: 0.95rem;
      outline: none;
      transition: all 0.2s ease;
    }
    input:focus {
      border-color: #0288d1;
      background: #ffffff;
      box-shadow: 0 0 0 3px rgba(2, 136, 209, 0.15);
    }
    .input-icon {
      position: absolute;
      top: 50%;
      left: 12px;
      transform: translateY(-50%);
      color: #94a3b8;
    }
    .btn-row {
      display: grid;
      grid-template-columns: 1fr 1fr;
      gap: 12px;
      margin-top: 22px;
    }
    .btn-connect {
      background: #0288d1;
      color: #ffffff;
      border: none;
      padding: 12px 16px;
      border-radius: 10px;
      font-size: 0.9rem;
      font-weight: 700;
      cursor: pointer;
      box-shadow: 0 2px 8px rgba(2, 136, 209, 0.3);
      transition: all 0.2s ease;
    }
    .btn-connect:hover {
      background: #0277bd;
      transform: translateY(-1px);
    }
    .btn-connect:active { transform: scale(0.97); }
    .btn-back {
      background: #f8fafc;
      color: var(--text-muted);
      border: 1px solid var(--card-border);
      padding: 12px 16px;
      border-radius: 10px;
      font-size: 0.9rem;
      font-weight: 600;
      text-decoration: none;
      text-align: center;
      transition: all 0.2s ease;
    }
    .btn-back:hover {
      background: #e2e8f0;
      color: var(--text-main);
    }
    #status-area {
      margin-top: 16px;
      font-size: 0.85rem;
      font-weight: 600;
      text-align: center;
      min-height: 28px;
    }
    .spinner {
      display: inline-block;
      width: 15px;
      height: 15px;
      border: 2px solid #cbd5e1;
      border-radius: 50%;
      border-top-color: #0288d1;
      animation: spin 0.8s linear infinite;
      vertical-align: middle;
      margin-right: 6px;
    }
    @keyframes spin { to { transform: rotate(360deg); } }
  </style>
</head>
<body>
  <header class="topbar">
    <svg class="brand-svg" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
      <rect x="5" y="5" width="14" height="14" rx="2.5"></rect>
      <line x1="9" y1="2" x2="9" y2="5"></line>
      <line x1="15" y1="2" x2="15" y2="5"></line>
      <line x1="9" y1="19" x2="9" y2="22"></line>
      <line x1="15" y1="19" x2="15" y2="22"></line>
      <line x1="2" y1="9" x2="5" y2="9"></line>
      <line x1="2" y1="15" x2="5" y2="15"></line>
      <line x1="19" y1="9" x2="22" y2="9"></line>
      <line x1="19" y1="15" x2="22" y2="15"></line>
      <path d="M8.5 11a4.8 4.8 0 0 1 7 0" stroke-width="1.8"></path>
      <path d="M10.2 13a2.4 2.4 0 0 1 3.6 0" stroke-width="1.8"></path>
      <circle cx="12" cy="15.2" r="0.9" fill="currentColor"></circle>
    </svg>
    <div class="brand-title">CORE IOT</div>
  </header>

  <div class="content-box">
    <div class="card">
      <h2>Cấu hình Wi-Fi Router</h2>
      <div class="header-desc">Chuyển sang STA Mode để gửi dữ liệu lên CoreIOT Cloud</div>

      <div class="input-group">
        <label for="ssid">Tên Wi-Fi (SSID)</label>
        <div class="input-box">
          <span class="input-icon">
            <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M5 12.55a11 11 0 0 1 14.08 0"></path><path d="M1.42 9a16 16 0 0 1 21.16 0"></path><path d="M8.53 16.11a6 6 0 0 1 6.95 0"></path><line x1="12" y1="20" x2="12.01" y2="20"></line></svg>
          </span>
          <input type="text" id="ssid" placeholder="Nhập tên Wi-Fi phòng / nhà">
        </div>
      </div>

      <div class="input-group">
        <label for="pass">Mật khẩu Wi-Fi</label>
        <div class="input-box">
          <span class="input-icon">
            <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><rect x="3" y="11" width="18" height="11" rx="2" ry="2"></rect><path d="M7 11V7a5 5 0 0 1 10 0v4"></path></svg>
          </span>
          <input type="password" id="pass" placeholder="Mật khẩu (bỏ trống nếu không có)">
        </div>
      </div>

      <div class="btn-row">
        <button class="btn-connect" onclick="startConnect()">
          <span>Lưu &amp; Kết nối</span>
        </button>
        <a href="/" class="btn-back">
          <span>Quay lại</span>
        </a>
      </div>

      <div id="status-area"></div>
    </div>
  </div>

  <script>
    let pollInterval = null;

    function startConnect() {
      let ssid = document.getElementById('ssid').value.trim();
      let pass = document.getElementById('pass').value;

      if (!ssid) {
        document.getElementById('status-area').innerHTML = '<span style="color: #dc2626;">Vui lòng nhập SSID!</span>';
        return;
      }

      document.getElementById('status-area').innerHTML = '<span class="spinner"></span> Đang kết nối tới router... (tối đa 10s)';

      fetch('/connect?ssid=' + encodeURIComponent(ssid) + '&pass=' + encodeURIComponent(pass))
        .then(r => r.text())
        .then(txt => {
          if (pollInterval) clearInterval(pollInterval);
          pollInterval = setInterval(checkStatus, 1000);
        })
        .catch(err => {
          document.getElementById('status-area').innerHTML = '<span style="color: #dc2626;">Lỗi gửi yêu cầu!</span>';
        });
    }

    function checkStatus() {
      fetch('/status')
        .then(r => r.json())
        .then(data => {
          if (data.state === 'CONNECTED') {
            clearInterval(pollInterval);
            document.getElementById('status-area').innerHTML = 
              '<span style="color: #059669;">&#10004; Kết nối thành công!</span><br>' +
              '<span style="font-size: 0.8rem; color: #64748b;">STA IP: <b style="color: #0288d1;">' + data.ip + '</b></span><br>' +
              '<a href="http://' + data.ip + '" style="display: inline-block; margin-top: 8px; color: #0288d1; text-decoration: underline; font-weight: 700;">Chuyển sang Dashboard tại IP mới &rarr;</a>';
          } else if (data.state === 'FAILED') {
            clearInterval(pollInterval);
            document.getElementById('status-area').innerHTML = 
              '<span style="color: #dc2626;">&#10008; Kết nối thất bại (sai pass hoặc timeout)!</span><br>' +
              '<span style="font-size: 0.78rem; color: #64748b;">ESP32 đã giữ AP tại 192.168.4.1</span>';
          }
        })
        .catch(e => {
          console.log('Status polling...');
        });
    }
  </script>
</body>
</html>)rawliteral";
