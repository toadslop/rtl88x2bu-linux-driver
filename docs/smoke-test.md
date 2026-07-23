# Hardware smoke test (L4)

STA bring-up checklist for a real RTL8822BU (or compatible) USB adapter. Use this at **wave milestones** and Phase 1 exit — not as the default gate for every PR. Offline gates: [`rust-migration/test-plan.md`](rust-migration/test-plan.md).

Requires a built `88x2bu.ko` (for migration work: pinned Rust-enabled `KDIR` + `LLVM=1`; see [`rust-migration.md`](rust-migration.md)).

## 1. Blacklist in-tree rtw88 (if needed)

```bash
lsmod | grep -E 'rtw88|88x2bu'
# If rtw88_* is loaded / preferred:
echo "blacklist rtw88_8822bu" | sudo tee /etc/modprobe.d/rtw8822bu.conf
sudo reboot   # or: sudo modprobe -r rtw88_8822bu rtw88_usb rtw88_core  (order may vary)
```

## 2. Insert module

From the driver tree (or install path):

```bash
sudo insmod ./88x2bu.ko
# or: sudo modprobe 88x2bu
dmesg | tail -n 50
ip link show
```

Expect an interface (often `wlan0`). Note the name for later steps.

## 3. Scan

```bash
IFACE=wlan0   # adjust
sudo ip link set "$IFACE" up
sudo iw dev "$IFACE" scan | head -n 80
# or: sudo iwlist "$IFACE" scan
```

## 4. Associate

Prefer IFACE-agnostic modern userspace (`NetworkManager`, or `wpa_supplicant` + `dhclient`/`dhcpcd` against `"$IFACE"`).

```bash
# Example: wpa_supplicant + DHCP (edit SSID/PSK in a local conf first)
sudo wpa_supplicant -B -i "$IFACE" -c ./wpa_supplicant.conf
sudo dhclient "$IFACE"    # or: NetworkManager / dhcpcd
```

Optional legacy helpers in this repo (`runwpa`, `wlan0dhcp`) hardcode `wlan0` and older `wpa_supplicant`/ifcfg patterns — use only if you know them:

```bash
sudo ./runwpa
sudo ./wlan0dhcp
```

Confirm association:

```bash
iw dev "$IFACE" link
# expect Connected to … / SSID …
```

## 5. Ping

```bash
ping -c 4 1.1.1.1
# or a known LAN gateway
```

## 6. Remove module

```bash
sudo ip link set "$IFACE" down
sudo rmmod 88x2bu
# or: sudo modprobe -r 88x2bu
dmesg | tail -n 50
```

Expect a clean unload: no WARN/Oops, no stuck references (`lsmod` should not show `88x2bu`).

## Pass / fail

| Step | Pass |
|------|------|
| Blacklist / load | `88x2bu` loaded; iface present |
| Scan | BSS list from the radio |
| Associate | Link up; IP if using DHCP |
| Ping | Echo replies |
| `rmmod` | Clean unload; no kernel WARN/Oops |

Failures here block the **wave epic** (or Phase 1 exit), not every small PR, if L0–L2 were green — but they must be investigated before opening the next wave’s children.
