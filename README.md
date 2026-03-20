# Firmware de identificación de motor DC con RLS/ARX en STM32F405RGT6

## Descripción general
Este proyecto implementa, sobre un **STM32F405RGT6**, un sistema de adquisición, procesamiento y visualización para la **caracterización dinámica de un motor DC con encoder incremental**. El firmware está escrito en **C**, con enfoque **bare-metal** y uso de **CMSIS**, sin HAL.

El sistema genera una señal PWM para accionar el motor, mide la velocidad a partir del encoder incremental, estima en tiempo real un modelo tipo **ARX** mediante un algoritmo de **Mínimos Cuadrados Recursivos (RLS)** y presenta información en una pantalla **TFT ILI9341**.

## Objetivo del proyecto
Desarrollar una plataforma didáctica y funcional para:

- controlar un motor DC mediante PWM,
- medir velocidad usando encoder incremental,
- ejecutar identificación paramétrica en línea mediante RLS,
- visualizar variables del sistema y resultados estimados en una TFT,
- mostrar una arquitectura modular de firmware basada en capas **driver + interfaz + aplicación**.

## Características principales
- Programación **bare-metal** basada en registros.
- Uso de **CMSIS** para acceso al hardware del STM32F4.
- Arquitectura modular con separación entre:
  - **drivers de periféricos**,
  - **capa de interfaz**,
  - **lógica de aplicación**.
- Estimación de parámetros en línea con modelo **ARX de 5 regresores**.
- Visualización gráfica en pantalla TFT ILI9341.
- Medición periódica mediante interrupción de **TIM4**.

## Plataforma de hardware
- **Microcontrolador:** STM32F405RGT6
- **Motor:** Motor DC con encoder incremental
- **Pantalla:** TFT con controlador **ILI9341**
- **Comunicación serial:** USART3
- **Señal de control del motor:** TIM1 PWM
- **Backlight de TFT:** TIM3 PWM
- **Lectura del encoder:** TIM2 en modo encoder

## Arquitectura del firmware
El proyecto está organizado en tres niveles.

### 1. Drivers
Acceso directo a registros del microcontrolador.

- `system_clock_driver.*`
- `gpio_driver.*`
- `spi_driver.*`
- `pwm_driver.*`
- `quad_enc.*`
- `uart_driver.*`
- `timer_driver.*`
- `ILI9341_driver.*`
- `mmcr.*`

### 2. Interfaces
Encapsulan los drivers mediante estructuras con punteros a función para desacoplar la aplicación del detalle de implementación.

- `system_clock_if.*`
- `gpio_if.*`
- `spi_if.*`
- `pwm_if.*`
- `quad_enc_if.*`
- `uart_if.*`
- `timer_if.*`

### 3. Aplicación
Coordina la inicialización, el lazo principal y la rutina periódica de identificación.

- `main.c`

## Estructura del proyecto
```text
espe/
├── Inc/
│   ├── Chars.h
│   ├── ILI9341_driver.h
│   ├── gpio_driver.h
│   ├── mmcr.h
│   ├── pwm_driver.h
│   ├── quad_enc.h
│   ├── spi_driver.h
│   ├── system_clock_driver.h
│   ├── timer_driver.h
│   └── uart_driver.h
├── if_c/
│   ├── gpio_if.c
│   ├── pwm_if.c
│   ├── quad_enc_if.c
│   ├── spi_if.c
│   ├── system_clock_if.c
│   ├── timer_if.c
│   └── uart_if.c
├── if_h/
│   ├── gpio_if.h
│   ├── pwm_if.h
│   ├── quad_enc_if.h
│   ├── spi_if.h
│   ├── system_clock_if.h
│   ├── timer_if.h
│   └── uart_if.h
├── src/
│   ├── ILI9341_driver.c
│   ├── gpio_driver.c
│   ├── main.c
│   ├── mmcr.c
│   ├── pwm_driver.c
│   ├── quad_enc.c
│   ├── spi_driver.c
│   ├── system_clock_driver.c
│   ├── timer_driver.c
│   └── uart_driver.c
└── startup/
    ├── startup_stm32f405rgtx.s
    └── STM32F405RGTX_FLASH.ld
```

## Flujo general de operación
1. Se limpian banderas de fallo y se habilita la FPU.
2. Se configura el reloj del sistema.
3. Se inicializa la pantalla TFT.
4. Se inicializan encoder, PWM del motor, UART y GPIO auxiliares.
5. Se inicializan las matrices y variables del algoritmo RLS.
6. Se arranca `TIM4`, que actúa como base temporal del sistema.
7. En cada interrupción de `TIM4`:
   - se lee el encoder,
   - se calcula la velocidad del motor,
   - se actualizan historiales,
   - se construye el vector de regresores,
   - se calcula la predicción,
   - se evalúa el error,
   - se actualizan parámetros y covarianza cuando corresponde.
8. En el `while(1)` principal:
   - se fija el sentido del motor,
   - se aplica el duty PWM,
   - se actualiza la gráfica en la TFT a una tasa menor.

## Mapa de periféricos y pines usados en el código
| Función | Periférico STM32 | Pines |
|---|---|---|
| PWM motor | TIM1 CH1 | PA8 |
| Dirección del motor | GPIOA | PA2, PA3 |
| Encoder incremental | TIM2 encoder mode | PA0, PA1 |
| UART | USART3 | PB10, PB11 |
| SPI hacia TFT | SPI1 | PA5, PA7 |
| Control TFT | GPIOB | PB6, PB7, PB8 |
| Backlight TFT | TIM3 CH3 | PB0 |
| Timer de muestreo | TIM4 | Interno |
| LED RGB | GPIOB | PB12, PB13, PB14 |

## Descripción de módulos

### `system_clock_driver.*`
Configura el reloj principal del microcontrolador y ofrece retardos por software.

### `gpio_driver.*`
Maneja pines de propósito general para:
- control de TFT,
- sentido del motor,
- LED RGB.

### `spi_driver.*`
Implementa el enlace SPI1 usado para la pantalla TFT ILI9341.

### `pwm_driver.*`
Genera:
- PWM del motor en `TIM1/PA8`,
- PWM del backlight de la TFT en `TIM3/PB0`.

### `quad_enc.*`
Configura `TIM2` en modo encoder y permite leer posición acumulada o velocidad en ticks por segundo.

### `uart_driver.*`
Inicializa USART3 a **115200 bps** y ofrece rutinas de transmisión y recepción por polling.

### `timer_driver.*`
Inicializa `TIM4` como base periódica para la adquisición y actualización del algoritmo.

### `ILI9341_driver.*`
Inicializa la TFT, envía comandos y datos, dibuja primitivas y actualiza la interfaz gráfica.

### `Chars.h`
Define la fuente bitmap utilizada para desplegar texto en pantalla.

### `mmcr.*`
Contiene las estructuras y operaciones matemáticas del algoritmo RLS/ARX:
- producto punto,
- construcción del vector de regresores,
- multiplicación matriz-vector,
- producto exterior,
- transposición,
- simetrización,
- saturación de covarianza,
- cálculo de promedios e historiales.

### `main.c`
Integra todos los módulos, define variables globales del motor y del RLS, y coordina la secuencia completa del sistema.

## Modelo y algoritmo implementado
El proyecto usa un esquema de identificación en línea con un vector de regresores de cinco elementos:

```text
z = [u(k), y(k-1), u(k-1), y(k-2), u(k-2)]
```

donde:
- `u(k)` es la entrada normalizada al motor,
- `y(k)` es la salida medida del sistema,
- `ye(k)` es la salida estimada,
- `e(k) = y(k) - ye(k)` es el error a priori.

El algoritmo actualiza:
- la ganancia de adaptación `K`,
- el vector de parámetros estimados `Pe`,
- la matriz de covarianza `C`.

Además, incluye medidas prácticas para mejorar estabilidad numérica:
- simetrización de la covarianza,
- acotamiento de la diagonal de `C`,
- validación por error promedio antes de adaptar.

## Variables globales principales
### Motor
La estructura `v_motor_t` almacena:
- velocidad estimada en rpm,
- posición por conteo del encoder,
- duty PWM,
- historial de velocidad.

### RLS
La estructura `variables_RLS_t` almacena:
- parámetros estimados,
- ganancias,
- vector de regresores,
- historiales de error,
- matriz de covarianza,
- matrices auxiliares para la actualización.

## Conversión de velocidad implementada
En la rutina de interrupción se calcula la velocidad a partir del conteo del encoder. La conversión actual del código asume:
- **44 pulsos por vuelta** en el motor,
- una ventana de muestreo de **10 ms**,
- relación de engranes de **4.4:1**.

La expresión usada es:

```text
rpm = pulsos / 44 * 100 * 60 / 4.4
```

## Pantalla TFT
La pantalla se utiliza para:
- mostrar una plantilla gráfica fija,
- dibujar ejes y zonas de visualización,
- representar la velocidad medida y la salida estimada.

El flujo de inicialización incluye:
- configuración de GPIO de control,
- inicialización de PWM para backlight,
- configuración de SPI1,
- reset del display,
- secuencia de arranque del ILI9341,
- carga de la plantilla gráfica.

## Dependencias
Este proyecto depende de:
- cabeceras CMSIS para **STM32F405xx**,
- archivo de arranque `startup_stm32f405rgtx.s`,
- linker script `STM32F405RGTX_FLASH.ld`,
- compilador ARM compatible con Cortex-M4F.

## Consideraciones de compilación
El proyecto no incluye aquí los archivos del entorno de construcción, por lo que puede integrarse en distintos flujos, por ejemplo:
- GCC ARM,
- STM32CubeIDE,
- VS Code con toolchain ARM,
- Makefile personalizado.

Es importante que la configuración de compilación contemple:
- objetivo **Cortex-M4**,
- **FPU** habilitada,
- definición de `STM32F405xx`,
- inclusión de CMSIS y de los directorios `Inc`, `if_h`, `src` e inicio.

## Observaciones técnicas
- La lógica está pensada con enfoque didáctico y modular.
- Se favorece el acceso directo a registros para que el estudiante comprenda la configuración real del periférico.
- La capa de interfaz permite reutilizar la misma aplicación con una implementación distinta del hardware si se desea conservar la firma.
- El sistema está orientado a prácticas de instrumentación, control e identificación de sistemas embebidos.

## Posibles mejoras futuras
- transmisión de variables por UART para depuración y registro en PC,
- almacenamiento de datos experimentales,
- ampliación del modelo identificado,
- separación más estricta entre ISR y procesamiento pesado,
- incorporación de filtros digitales adicionales,
- ajuste dinámico de parámetros de adaptación.

## Uso académico sugerido
Este proyecto es adecuado para cursos o prácticas de:
- microcontroladores bare-metal,
- programación con CMSIS,
- electrónica de control,
- identificación de sistemas,
- sistemas embebidos en tiempo real,
- integración de periféricos en STM32.

## Estado del proyecto
El firmware integra exitosamente:
- generación de PWM,
- lectura de encoder,
- visualización en TFT,
- temporización periódica,
- estimación RLS/ARX en tiempo real.

Está diseñado como base de trabajo para experimentación, enseñanza y refinamiento de estrategias de identificación sobre una planta real.
