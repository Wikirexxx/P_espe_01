## 🚀  STM32F103C8T6 - Prueba de manejo de GPIO y creación de proyecto sin IDE y Extreme-BMP usando Makefile y archivos para CMSIS
Firmware para generar un parpadeo en un led conectado al pin PC13, usando un **STM32F103C8T6**.  
El código está escrito en **C**, utilizando archivos de cabecera CMSIS (**Extreme Bare metal programmig**). Además el codigo está compilado con **arm-none-eabi-gcc**, utilizando `Makefile` para la compilación, los comandos utilizados para la generación del `Makefile` se encuentran en el apartado `Compilación y generación de *.hex`

---

## 📂 Estructura del Proyecto
📁 Root<br>
&emsp;│── 📁build<br>
&emsp;&emsp;&emsp;&emsp;│── 📁output<br>
&emsp;│── 📁docs<br>
&emsp;│── 📁Inc<br>
&emsp;│── 📁src<br>
&emsp;│── 📁startup<br>
📁 C:\Users\franc\Documents\Embebidos\Extreme_BMP\chip_headers_STM32F103x\CMSIS\Device\ST\STM32F1xx\Include <br>
📁 C:\Users\franc\Documents\Embebidos\Extreme_BMP\chip_headers_STM32F103x\CMSIS\Include <br>

## 🔧 **Requisitos**
- **Microcontrolador:** STM32F103C8T6  
- **Compilador:** `arm-none-eabi-gcc`  
- **Flasheo:** `st-flash` o `STM32CubeProgrammer`  
- **Herramientas opcionales:** `Ozone` para depuracion
- **Terminal bash:** Para ejecución de comandos
- **MSYS:** Para ejecución de `Makefile`
---

## 🔨 **Compilación y generación de *.hex

```bash
$ make
```

## 🗑️ Borrar archivo / ❌ Eliminar archivos de salida y *.hex 

```bash
$ make clean
```

## Rutas
Se agregan las rutas de los archivos CMSIS
* C:\Users\franc\Documents\Embebidos\Extreme_BMP\chip_headers_STM32F103x\CMSIS\Device\ST\STM32F1xx\Include
* C:\Users\franc\Documents\Embebidos\Extreme_BMP\chip_headers_STM32F103x\CMSIS\Include
