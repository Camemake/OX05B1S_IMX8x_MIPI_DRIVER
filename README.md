# OX05B1S i.MX8 4-Lane MIPI CSI-2 Driver

**Status:** Template — replace the placeholder register list with the official one, build, and stream **2592 × 1944 @ 60 fps** on any **NXP i.MX8** SoC that exposes four CSI-2 data-lanes.

---

## 1  Features

| Capability | Details |
|------------|---------|
| Resolution | 2592 × 1944 (5 MP) |
| Frame rate | 60 fps (4-lane, 1.05 Gbps per lane) |
| Bit depth  | RAW12 (RGB-IR Bayer + IR) |
| Interface  | MIPI CSI-2, 4 data-lanes + 1 clock |
| Platform   | NXP i.MX8M*/i.MX8MP/i.MX8QM, etc. |
| Controls   | Stream on/off, exposure, analogue & digital gain, flips |

> The driver is a **V4L2 sub-device** and therefore host-agnostic.  
> All i.MX8 specifics live in the device-tree snippet that connects the sensor to `mipi_csi_2`.

---

## 2  Repository Layout

```
OX05B1S_IMX8X_MIPI_DRIVER/
├── include/ox05b1s.h           # public structs & constants
├── src/
│   ├── ox05b1s.c               # V4L2 sub-device driver
│   └── ox05b1s_modes.c         # ← paste real register list here
├── dts/ox05b1s-imx8-snippet.dtsi  # DT fragment to merge
├── examples/capture_ox05b1s.c  # tiny V4L2 sample
├── Makefile                    # out-of-tree kernel build
└── README.md                   # this file
```

---

## 3  Quick-Start

```bash
# 0. Edit src/ox05b1s_modes.c — paste the real init sequence.
sudo apt install build-essential linux-headers-$(uname -r) git
git clone https://github.com/your-github/OX05B1S_IMX8X_MIPI_DRIVER.git
cd OX05B1S_IMX8X_MIPI_DRIVER

make -j$(nproc)       # builds ox05b1s.ko
sudo make install      # copies .ko & runs depmod

# Merge DT snippet, re-build DTB, reboot
cat dts/ox05b1s-imx8-snippet.dtsi >> <board>.dts
make dtbs
sudo cp <board>.dtb /boot/dtbs/<…>/
sudo reboot
```

After reboot:

```bash
v4l2-ctl --list-devices
ox05b1s 0-006c (platform:mipi_csi2):
        /dev/video2   # node index may differ
```

---

## 4  Capture Example

```bash
v4l2-ctl -d /dev/video2          -v width=2592,height=1944,pixelformat=RG12          --stream-mmap --stream-count=120
```

---

## 5  Device-Tree Notes

* **I²C bus** – change `&i2c3` to your board’s bus.  
* **Reset GPIO** – update pin & polarity.  
* **Clock** – supply a 24 MHz `xvclk` (root or external).  
* **`link-frequencies`** – per-lane bit-rate in Hz = `PCLK × bits / 4`.

---

## 6  Extending

* Duplicate `struct ox05b1s_mode` for more resolutions/FPS.  
* Add HDR, PDAF, or OTP reading in `ox05b1s.c`.  
* Export extra V4L2 controls with the control framework.

---

## 7  Troubleshooting

| Symptom | Remedy |
|---------|--------|
| `chip id mismatch` at probe | Wrong I²C address or power sequencing. |
| No `/dev/video*` node | DT link incorrect – check `dmesg | grep csi`. |
| `FIFO overflow` in CSI driver | Bad `link-frequencies` / timing. |
| Color tint | Wrong Bayer order (sensor default is **RGGB**). |

---

