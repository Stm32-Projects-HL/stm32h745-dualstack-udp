# STM32H745I-DISCO Dual-Stack UDP Example

A dual‑stack (IPv4 & IPv6) UDP networking example for the **STM32H745** microcontroller using STM32 HAL and NetXDuo.

---

## 📁 Repository Structure

```
stm32h745-dualstack-udp/
├── Core/                     # Main application source (M7 Core)
├── Middleware/               # LwIP or NetXDuo network stack
├── Drivers/                  # HAL drivers
├── Tools/                    
├── .gitignore
├── Makefile / project files
└── README.md
```

---

## 🧠 Overview

This project targets the **STM32H745** dual-core microcontroller (Cortex-M7 + Cortex-M4). It demonstrates:

✔ Ethernet initialization  
✔ IPv4 & IPv6 dual-stack support  
✔ UDP echo or send/receive  
✔ Host PC testing support

---

## 📌 Hardware Requirements

- **NUCLEO‑H745ZI-Q** or similar with Ethernet PHY
- Ethernet cable
- PC on the same subnet

---

## 🛠️ Requirements

- **STM32CubeIDE**
- **STM32CubeProgrammer**
- Ethernet-connected host PC

---

## ⚙️ Configuration

- **IPv4**: 192.168.1.111
- **IPv6**: fe80::1234:5678:9abc:def0
- **UDP Port**: 6000

---

## 🚀 Build Instructions

1. Open in STM32CubeIDE
2. Check correct MCU: STM32H745
3. Enable middleware (NetXDuo)
4. Build project

---

## 🔁 Flashing Instructions

1. Open STM32CubeProgrammer
2. Connect via ST-Link
3. Flash compiled `.elf` or `.bin`
4. Reset board

---

---

## 🧠 How It Works

1. Configure Ethernet + PHY
2. Set static IP or use DHCP (if enabled)
3. Bind UDP port using stack
4. Receive, echo, or process UDP packets (Using PacketSender to test)

---

## 🧩 Tips

- Match PC subnet with MCU
- Ensure Ethernet clocks are configured
- For IPv6, use link-local with correct interface
- Use Wireshark to inspect packets

---

## 📚 References

- STM32H743 NETXDUO examples on GitHub
- ST application note AN5557 (dual-core architecture)

---
