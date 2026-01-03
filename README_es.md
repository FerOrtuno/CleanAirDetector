# CleanAirDetector Firmware

Firmware inteligente para monitores de calidad de aire basados en sensores MOS (MQ-135), diseñado para corregir la deriva térmica y proporcionar lecturas estables mediante compensación ambiental.

## Características Principales

- **Arquitectura de Estados Finitos**: Gestión robusta del ciclo de vida del sensor (Arranque -> Precalentamiento -> Calibración -> Operación).
- **Precalentamiento Controlado**: Ciclo obligatorio de 15 minutos con cuenta regresiva en pantalla para estabilizar el elemento calefactor del sensor.
- **Calibración "Outdoor Reference"**: Algoritmo de calibración inversa de un punto que fija la línea base en **427.20 ppm** (concentración global aproximada de CO2) al finalizar el precalentamiento en aire exterior.
- **Compensación Térmica**: Integración con sensor **DHT22** para ajustar dinámicamente la lectura del MQ-135 según la temperatura y humedad, evitando falsos positivos al mover el dispositivo de exteriores fríos a interiores cálidos.
- **Indicador de Calidad Cualitativa**: Sistema de 4 niveles para una interpretación fácil y segura del aire:
  - **OPTIMA (EXT)**: < 600 ppm (Aire puro/exterior)
  - **BUENA (INT)**: 600 - 1000 ppm (Interior saludable)
  - **REGULAR**: 1000 - 1500 ppm (Ventilación recomendada)
  - **MALA**: > 1500 ppm (Ventilación necesaria)

## Requisitos de Hardware

- **Microcontrolador**: Arduino Uno/Nano o compatible.
- **Sensor de Gas**: Módulo MQ-135 (Pin A0).
  - *Nota*: Verifique la resistencia de carga (RL) de su módulo. Configurable en `config.h` (Defecto: 10k).
- **Sensor T/H**: DHT22 o DHT11 (Pin Digital 4).
- **Pantalla**: LCD 1602 con interfaz I2C (Dirección 0x27).
- **Botón (Opcional)**: Pulsador momentáneo para recalibración manual (Pin Digital 2 a GND).

## Instalación y Configuración

1. **Librerías Necesarias**:
   - `LiquidCrystal_I2C`
   - `DHT sensor library` (Adafruit)

2. **Conexiones**:

   | Componente | Pin Arduino |
   |------------|-------------|
   | MQ-135 AO  | A2          |
   | DHT22 Data | D2          |
   | LCD SDA    | A4 (SDA)    |
   | LCD SCL    | A5 (SCL)    |
   | Botón      | D4 (a GND)  |

3. **Configuración**:
   Edite el archivo `config.h` para ajustar parámetros como:
   - `WARMUP_MINUTES`: Tiempo de precalentamiento.
   - `RL_VALUE`: Resistencia de carga de su placa MQ.
   - Umbrales de calidad de aire.

## Uso

1. Encienda el dispositivo al aire libre o en una ventana abierta ("Aire Limpio").
2. Espere a que termine el ciclo de **PRECALENTAMIENTO** (15 min).
3. El sistema mostrará **"Calibrado!"** y pasará a modo operación.
4. Ya puede ingresar el dispositivo al interior.
5. Si necesita recalibrar, mantenga el sensor en aire limpio y pulse el botón de recalibración (o reinicie).

## Licencia

Este proyecto es de código abierto.
