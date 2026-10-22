# 0.2.0 hardware regression
User reports brightness no longer moves after installing 0.2.0. Do not call system slider/HUD fixed. No new Dell logs available; USB absent on development Mac at inspection. Possible API/display resolution/capability failure or Accessibility reauthorization; not established.

Preserved, signed 0.1.0 helper and its installer exist at iGPU-Port/backups/input-system-sync-20261021/Brightness. Reinstall via that directory's install-brightness-keys.sh as normal logged-in user to restore confirmed direct linear-brightness path. This does not resolve system UI synchronization.

Before restoring, invoke installed helper diagnose and manual minus (manual step needs no Accessibility permission) if user can provide output. Collector 0.2.0 captures diagnose and helper errors. Avoid another native-API-only candidate without target error evidence. Do not change framebuffer/graphics or keyboard/touchpad trial to fix this regression.
