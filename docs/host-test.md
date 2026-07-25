# Host hardware test (Arch laptop)

Practical recipe for building `88x2bu.ko` against this machine’s running kernel and exercising an RTL8812BU / RTL8822BU **USB** dongle **without** touching the laptop’s built-in Wi-Fi.

Normative L4 checklist (scan / associate / ping / unload criteria): [`smoke-test.md`](smoke-test.md). Arch toolchain gotchas: [`rust-migration/dev-environment.md`](rust-migration/dev-environment.md#arch-linux).

## Safety

| Item | Notes |
|------|--------|
| Built-in radio | Typically Intel (`iwlwifi`, e.g. `wlp0s20f3`). Unrelated to this module. |
| What `88x2bu` binds | Realtek USB IDs only. It does not unload or replace `iwlwifi`. |
| Prefer temporary load | Use `insmod` / `rmmod` while iterating. Avoid DKMS until you want it permanent. |
| Residual risk | A buggy out-of-tree module can still Oops the whole kernel; it will not “take over” the Intel card. |

Identify interfaces before and after:

```bash
ip -br link
lsmod | grep -E 'iwlwifi|rtw88|88x2bu'
```

Keep using the built-in iface for normal networking; only operate on the **new** USB iface below.

## 1. Build against the running kernel

Stock Arch `linux` / `linux-headers` are usually **GCC-built** with `CONFIG_RUST=y`. Do **not** pass `LLVM=1`. Prefer pacman `rustc` over rustup.

```bash
sudo pacman -S --needed linux-headers base-devel bc rust

export PATH="/usr/bin:$PATH"          # Arch rustc before ~/.cargo/bin
export LIBCLANG_PATH=/usr/lib
rustc --version                       # expect "(Arch Linux rust …)"

cd /path/to/rtl88x2bu-linux-driver
make clean
make KDIR=/lib/modules/$(uname -r)/build -j"$(nproc)"
nm 88x2bu.ko | grep -E 'rtw_rust_kbuild_probe|rtw_rust_scaffold_init|aes_ctr_encrypt'
```

## 2. Plug in the USB dongle

```bash
lsusb | grep -i realtek
dmesg | tail -n 30
```

If in-tree `rtw88_*` claims the stick first:

```bash
lsmod | grep -E 'rtw88|88x2bu'
echo "blacklist rtw88_8822bu" | sudo tee /etc/modprobe.d/rtw8822bu.conf
sudo modprobe -r rtw88_8822bu rtw88_usb rtw88_core 2>/dev/null || true
# Unplug / replug the dongle if it stayed bound
```

This blacklist targets Realtek USB only — not Intel Wi-Fi.

## 3. Load the module

```bash
export PATH="/usr/bin:$PATH"
sudo insmod ./88x2bu.ko
dmesg | tail -n 40
ip -br link
```

Expect a **new** interface (often `wlan0`), distinct from the built-in `wlp*`. Note its name:

```bash
IFACE=wlan0   # adjust to whatever appeared
```

In NetworkManager, connect / scan on `"$IFACE"` only; leave the built-in connection profile alone.

## 4. Quick smoke (USB iface only)

```bash
sudo ip link set "$IFACE" up
sudo iw dev "$IFACE" scan | head -n 80
```

For associate / DHCP / ping, follow [`smoke-test.md`](smoke-test.md) steps 4–5 against `"$IFACE"`.

## 5. Unload

```bash
sudo ip link set "$IFACE" down
sudo rmmod 88x2bu
dmesg | tail -n 30
lsmod | grep 88x2bu || echo "88x2bu unloaded"
```

Pass = clean unload, no WARN/Oops, built-in Wi-Fi still on `iwlwifi`.

## Common failures

| Symptom | Fix |
|---------|-----|
| Clang errors: `-mindirect-branch=…`, `-mpreferred-stack-boundary=3` | Dropped `LLVM=1`; rebuild with GCC as above. |
| `E0514` incompatible `core` / rustc | `export PATH="/usr/bin:$PATH"` so pacman `rustc` matches header `.rmeta`. |
| `/bin/sh: bc: command not found` | `sudo pacman -S bc` |
| Stick appears but no new iface / `rtw88_*` loaded | Blacklist `rtw88_8822bu`, unload, replug, `insmod` again. |
| Built-in Wi-Fi gone after test | Unrelated to bind; check `rfkill` / `ip link` / NetworkManager — `rmmod 88x2bu` should not remove `iwlwifi`. |
