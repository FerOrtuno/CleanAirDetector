# Changelog

Todos los cambios notables en este proyecto serán documentados en este archivo.

El formato se basa en [Keep a Changelog](https://keepachangelog.com/en/1.0.0/), y este proyecto adhiere a [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.1.0] - 2026-01-03

### Añadido

- **Compensación Térmica**: Implementación de algoritmo de corrección polinomial utilizando lecturas de sensor DHT22 (Temp/Hum) para estabilizar lecturas del MQ-135 ante cambios ambientales.
- **Nuevos Niveles de Calidad**: Se ha refinado la escala de calidad de aire a 4 estados: `OPTIMA (<600)`, `BUENA (<1000)`, `REGULAR (<1500)`, `MALA (>1500)`.
- **Botón de Recalibración**: Soporte para botón físico (Pin 2) que fuerza una recalibración manual sin reiniciar el dispositivo.
- **Configuración Centralizada**: Archivo `config.h` para gestión fácil de pines, constantes y umbrales.
- **Dependencia DHT**: Integración de la librería DHT para lecturas ambientales.

### Cambiado

- **Arquitectura de Software**: Migración completa de estructura lineal a **Máquina de Estados Finitos** no bloqueante (`STATE_BOOT`, `WARMUP`, `CALIBRATE`, `RUN`, `ERROR`).
- **Estrategia de Calibración**:
  - Antes: Calibración continua errónea durante el calentamiento.
  - Ahora: Precalentamiento pasivo de 15 minutos ("Blind Time") seguido de una calibración de un solo punto ("One-Point Inverse Calibration") al objetivo de **427.20 ppm**.
- **Interfaz de Usuario**:
  - Contador regresivo visual durante el calentamiento.
  - Visualización de Temperatura y estimación de PPM en pantalla principal.

### Corregido

- Eliminada la deriva de lectura causada por calibrar el sensor mientras su resistencia interna cambiaba por temperatura (Warm-up drift).
- Corregida la fórmula de PPM para usar un `TARGET_OUTDOOR_PPM` fijo en lugar de ratios de aire limpio genéricos.

## [1.0.0] - Versión Inicial (Legacy)

- Lectura básica de MQ-135.
- Display LCD.
- Lógica simple basada en voltajes sin compensación.
