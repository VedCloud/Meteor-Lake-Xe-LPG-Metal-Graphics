# Branding is an independent, backed-up cosmetic transaction after graphics commits.
# A branding failure is reported without rolling back the installed graphics runtime.
if [ "$MODE" = full ] && [ -f "$HERE/Branding/install-about-this-mac.sh" ];then
 echo 'Installing the bundled Dell About This Mac identity/icon...'
 set +e
 /bin/bash "$HERE/Branding/install-about-this-mac.sh" > >(tee "$REPORT/branding.txt") 2>&1
 branding_result=$?
 set -e
 printf 'branding-exit=%s\n' "$branding_result" >> "$REPORT/RESULT.txt"
 if [ "$branding_result" -ne 0 ];then
  echo "Graphics installed successfully; About This Mac branding failed with exit=$branding_result. See $REPORT/branding.txt."
  exit "$branding_result"
 fi
fi
