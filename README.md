# 🚗 STM32 DC Motor Control Using L293D — Register-Level Coding

This is a **bare-metal STM32 DC motor control project** using a **register-level approach** (no HAL drivers).
You’ll control a DC motor’s **direction** and **speed** using an **L293D** motor driver and manually configured STM32 registers.

This is great if you want to learn **how microcontrollers really work** under the hood — not just call HAL wrappers.

---

## 📌 What This Project Does

You’ll learn how to:

* Directly configure **GPIO registers** to set motor direction
* Set up **PWM using STM32 timers** at the register level
* Drive a DC motor via the **L293D H-bridge driver**
* Understand the **signal flow** from MCU → driver → motor

This is the same control strategy as HAL versions, just without abstraction layers.([DeepBlue][1])

---

## 🧠 Why Register-Level Code

Using registers means:

✔ You **know what’s happening inside your MCU**
✔ Code runs faster with minimal overhead
✔ You learn how to control peripherals manually
✔ Great for interviews and deep embedded skills

No HAL, no Cube — just straight C and register access.

---

## 📦 Hardware Required

| Component                     | Purpose      |
| ----------------------------- | ------------ |
| STM32 MCU (e.g., STM32F446RE) | Controller   |
| L293D Motor Driver IC         | Motor driver |
| DC Motor                      | Actuator     |
| External Power Supply         | Motor power  |
| Wires & Breadboard            | Connections  |

Remember, DC motors **should not** be driven directly from MCU pins — that’s what L293D is for.([Last Minute Engineers][2])

---

## 🔌 Pin Connections (Example)

| MCU Pin              | Function    | L293D        |
| -------------------- | ----------- | ------------ |
| PA12                 | IN1         | Direction A  |
| PA11                 | IN2         | Direction B  |
| PA5                  | PWM         | EN1 (Enable) |
| GND                  | Ground      | Common GND   |
| Motor + External VCC | Motor power | Vcc2         |

⚠️ **Common ground** between MCU, L293D, and motor supply is crucial.

---

## 🧩 How It Works — Beginner View

### ➤ Direction Control

Here we use **GPIO register writes** instead of HAL.

To rotate forward:

```c
GPIOA->ODR |= (1 << 12);   // PA12 = 1
GPIOA->ODR &= ~(1 << 11);  // PA11 = 0
```

To reverse:

```c
GPIOA->ODR &= ~(1 << 12);  // PA12 = 0
GPIOA->ODR |= (1 << 11);   // PA11 = 1
```

To stop:

```c
GPIOA->ODR &= ~((1 << 12) | (1 << 11));
```

**Direction bits map to L293D control pins**, so changing them changes motor behavior.([Last Minute Engineers][2])

---

### ➤ PWM Speed Control (Register Style)

We manually configure the **Timer (TIM)** registers for PWM.

#### 🔧 Basic Setup Steps

1. Enable Timer clock
2. Configure PWM pin as alternate function
3. Set timer period (ARR)
4. Set compare value (CCR)
5. Enable PWM mode

Example:

```c
TIM1->PSC = 0;       // Prescaler
TIM1->ARR = 1000;    // Auto-reload (controls period)
TIM1->CCR1 = 500;    // Compare (50% duty)
TIM1->CCER |= TIM_CCER_CC1E; // Enable channel
TIM1->CR1 |= TIM_CR1_CEN;    // Start timer
```

This sets up PWM on **Timer1 Channel1** with a duty cycle of 50%.
As you change `CCR1`, the speed changes.

Same concept as HAL PWM, just *direct register access*.([HackMD][3])

---

## 📌 Code Flow (Register Logic)

Here’s what your main loop *actually does*:

```c
// Configure GPIO for direction pins
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
GPIOA->MODER |= (1 << (12*2)) | (1 << (11*2)); // PA12, PA11 digital out

// Configure PWM pin
GPIOA->AFR[0] |= (1 << (5*4));   // Set AltFunc on PA5
// Timer setup for PWM here...

// Run motor forward
GPIOA->ODR |= (1<<12);
GPIOA->ODR &= ~(1<<11);

// PWM 50%
TIM1->CCR1 = 500;

// delay loop
for (volatile int i = 0; i < 1000000; i++);

// Stop motor
GPIOA->ODR &= ~((1<<12)|(1<<11));
```

This is **pure bare metal**, no layers between you and the silicon.

---

## 🔁 Internal Signal Flow

```
MCU Registers → GPIO Bits → L293D Inputs → Motor
MCU Timer → PWM Output → L293D EN → Motor Speed
```

Direction control is digital logic; speed control is PWM. Either one can be independently changed.

---

## ⚠️ Common Beginner Gotchas

❗ No HAL delays — use safe delay loops or SysTick
❗ Pin alternate function selection must match timer channel
❗ Always ensure proper grounding
❗ High duty cycles mean high motor current

---

## 🎓 What You’ll Learn

* How GPIO registers work
* Configuring timers for PWM
* H-bridge motor control logic
* How motor direction & speed signals affect movement

---

## 🚀 Next Steps You Can Build

* Button-controlled direction/speed
* UART motor control
* Encoder feedback + PID
* Integrating sensors

---

## 📜 License

MIT License — you’re free to use and expand.

---
