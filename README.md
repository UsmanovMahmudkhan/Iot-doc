# Smart Blind Shoes - Web Dashboard

<div align="center">

![Smart Blind Shoes](https://img.shields.io/badge/Project-Smart%20Blind%20Shoes-blue)
![Status](https://img.shields.io/badge/Status-Active-success)
![License](https://img.shields.io/badge/License-MIT-green)

**AI-Powered Navigation System for Visually Impaired Individuals**

[Live Demo](https://wokwi.com/projects/449468602181139457) • [Documentation](./docs/README.md) • [GitHub Repository](https://github.com/UsmanovMahmudkhan/Iot-doc)

</div>

---

## Table of Contents

- [Overview](#overview)
- [Problem Statement](#problem-statement)
- [Solution](#solution)
- [Features](#features)
- [System Architecture](#system-architecture)
- [Installation](#installation)
- [Usage](#usage)
- [Team Members](#team-members)
- [Technology Stack](#technology-stack)
- [Project Structure](#project-structure)
- [Contributing](#contributing)
- [License](#license)

## Overview

Smart Blind Shoes is an innovative assistive technology project designed to enhance mobility and independence for visually impaired individuals. The system combines hardware sensors, intelligent processing, and a modern web interface to provide real-time obstacle detection and navigation assistance.

### Key Highlights

- **Hands-free navigation** - No need to hold a cane
- **360° obstacle detection** - Four ultrasonic sensors cover all directions
- **Real-time web monitoring** - Live dashboard for data visualization
- **Haptic feedback** - Intuitive vibration patterns guide users
- **Emergency system** - Quick access to help with GPS location sharing

## Problem Statement

According to the World Health Organization, over **285 million people worldwide** are visually impaired. Traditional navigation aids like white canes have significant limitations:

- **Limited detection range** - Only detect obstacles at ground level
- **Manual operation required** - Must be held and swung constantly
- **No data tracking** - Cannot monitor usage patterns or provide analytics
- **No remote monitoring** - Caregivers cannot track user location or status

Our project addresses these challenges by providing a comprehensive, hands-free navigation solution with real-time monitoring capabilities.

## Solution

Smart Blind Shoes integrates:

1. **Hardware Layer**: Arduino-based system with ultrasonic sensors, vibration motors, and GPS
2. **Processing Layer**: Real-time sensor data processing and obstacle detection algorithms
3. **Web Interface**: Modern dashboard for visualization, monitoring, and control

### System Workflow

```
Sensor Data → Arduino Processing → Haptic Feedback
                    ↓
            Web Dashboard (Real-time)
```

## Features

### Core Functionality

- **Multi-directional obstacle detection** (Left, Center, Right, Downward)
- **Intelligent zone classification** (Clear, Warning, Alert, Critical)
- **Directional haptic feedback** (Left/Right vibration patterns)
- **Step counting and activity tracking**
- **Battery level monitoring**
- **GPS location tracking**
- **Emergency alert system**

### Web Dashboard Features

- **Real-time data visualization** with interactive charts
- **GPS location display** with coordinate tracking
- **Historical data graphs** showing distance trends
- **System controls** for mode and sensitivity adjustment
- **Responsive design** - works on all devices
- **Modern UI** - Apple/Stripe inspired design

## System Architecture

```
┌─────────────────┐
│  Ultrasonic     │
│  Sensors (x4)   │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│   Arduino Uno   │
│   Processing    │
└────────┬────────┘
         │
    ┌────┴────┐
    │         │
    ▼         ▼
┌────────┐ ┌──────────────┐
│ Haptic │ │ Serial/USB   │
│Feedback│ │ Communication│
└────────┘ └──────┬───────┘
                  │
                  ▼
         ┌────────────────┐
         │  Web Dashboard │
         │  (JavaScript)  │
         └────────────────┘
```

## Installation

### Prerequisites

- Modern web browser (Chrome, Firefox, Safari, Edge)
- Local web server (optional, for development)

### Quick Start

1. **Clone the repository**
   ```bash
   git clone https://github.com/UsmanovMahmudkhan/Iot-doc.git
   cd Iot-doc
   ```

2. **Open the project**
   - Simply open `index.html` in your web browser
   - Or use a local server:
     ```bash
     # Python
     python -m http.server 8000
     
     # Node.js
     npx http-server
     ```

3. **Access the dashboard**
   - Navigate to `http://localhost:8000` (if using server)
   - Or open `index.html` directly in browser

## Usage

### Starting the Dashboard

1. Open `index.html` in your web browser
2. Click **"Start Live Demo"** button or scroll to Dashboard section
3. The dashboard will automatically start simulating sensor data

### Dashboard Features

- **Real-time Sensor Data**: See obstacle distances update every 500ms
- **Interactive Charts**: View distance trends over time
- **System Controls**: Adjust operation mode and sensitivity
- **Emergency Button**: Trigger emergency alert (simulated)

### Connecting Real Hardware

To connect actual Arduino hardware:

1. Upload the Arduino sketch to your device
2. Ensure Serial communication is enabled
3. Modify `src/js/app.js` to read from Serial port instead of simulation
4. Use Web Serial API for browser-to-Arduino communication

## Team Members

| Name | Role | Responsibilities |
|------|------|------------------|
| **Mahmud Khonusmonov** | Hardware & Firmware | Arduino programming, sensor integration, haptic feedback system |
| **Team Member 2** | Web Development | Frontend design, real-time data visualization, API integration |
| **Team Member 3** | Backend & Infrastructure | Server setup, data processing, deployment |
| **Team Member 4** | Testing & Documentation | System testing, user documentation, presentation materials |

## Technology Stack

### Frontend
- **HTML5** - Structure
- **CSS3** - Styling (Apple/Stripe inspired design)
- **JavaScript (ES6+)** - Interactivity and data visualization
- **Chart.js** - Data visualization library

### Hardware
- **Arduino Uno** - Microcontroller
- **HC-SR04 Ultrasonic Sensors** - Obstacle detection
- **Vibration Motors** - Haptic feedback
- **GPS Module** - Location tracking

### Tools & Libraries
- **Chart.js** - For real-time charts
- **Google Fonts (Inter)** - Typography
- **Web Serial API** - Browser-Arduino communication (future)

## Project Structure

```
smart-blind-shoes/
├── index.html              # Main HTML file
├── README.md              # This file
├── docs/                  # Documentation
│   ├── README.md         # Detailed documentation
│   ├── API.md            # API reference
│   └── PRESENTATION.md   # Presentation guide
├── src/
│   ├── css/
│   │   └── style.css     # Main stylesheet
│   ├── js/
│   │   └── app.js        # Main JavaScript application
│   └── assets/            # Images, icons, etc.
├── presentation/          # Presentation materials
│   ├── slides/           # Presentation slides
│   └── demo-plan.md      # Demo execution plan
└── public/               # Public assets (if deploying)
```

## Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- World Health Organization for statistics on visual impairment
- Arduino community for hardware support
- Chart.js for excellent visualization library
- Apple and Stripe for design inspiration

## Contact

For questions, suggestions, or collaboration opportunities:

- **Project Repository**: [GitHub](https://github.com/UsmanovMahmudkhan/Iot-doc)
- **Live Simulation**: [Wokwi](https://wokwi.com/projects/449468602181139457)
- **Portfolio**: [View Portfolio](./portfolio.html)

---

<div align="center">

**Made with dedication for the visually impaired community**

Star this repo if you find it helpful!

</div>

