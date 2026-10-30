from pathlib import Path
import hashlib,shutil,plistlib,subprocess,json
b=Path(__file__).resolve().parent;old=b/'Tahoe-Graphics-Runtime';a=b/'Audio';p=b/'Tahoe-Combined-Runtime'
def sha(f):return hashlib.sha256(f.read_bytes()).hexdigest()
if p.exists():raise SystemExit('Combined staging already exists; inspect it before rebuilding.')
shutil.copytree(old,p,ignore=shutil.ignore_patterns('reports'))
(p/'Audio/Profiles').mkdir(parents=True)
shutil.copy2(a/'Profiles/base.plist',p/'Audio/Profiles/base.plist')
base=p/'Audio/Profiles/base.plist'
for mode in ['accelerated','recovery']:
 subprocess.run([str(p/'tahoe-runtime-check'),'--config',str(base),str(p/'Profiles'/(mode+'.plist')),mode,sha(base)],check=True)
 subprocess.run([str(p/'ocvalidate'),str(p/'Profiles'/(mode+'.plist'))],stdout=subprocess.DEVNULL,check=True)
 shutil.copy2(p/'Profiles'/(mode+'.plist'),p/'Audio/Profiles'/(mode+'.plist'))
entries=[]
for mode,source in [('base',b/'Candidate/EFI/OC/config.plist'),('accelerated',old/'Profiles/accelerated.plist'),('recovery',old/'Profiles/recovery.plist')]:
 entries += [(sha(source),mode),(sha(p/'Audio/Profiles'/(mode+'.plist')),mode)]
(p/'Audio/PROFILE-MAP.txt').write_text(''.join(h+' '+mode+'\n' for h,mode in sorted(entries)))
(p/'ALLOWED-CONFIG-SHA256.txt').write_text(''.join(h+'\n' for h,mode in sorted(entries)))
shutil.copytree(a/'SimpleHDA.kext',p/'EFI-Kexts/SimpleHDA.kext')
for name in ['PROVENANCE.json','KERNEL-AUDIT.json','NOTICE.md','README.en.md']:shutil.copy2(a/name,p/'Audio'/name)
shutil.copytree(a/'Licenses',p/'Audio/Licenses')
# Keep the optional HAL package out of automatic installation: standard IOAudio is the initial path.
alc=b/'Candidate/EFI/OC/Kexts/AppleALC.kext/Contents/MacOS/AppleALC'
(p/'Audio/AppleALC-SHA256.txt').write_text(sha(alc)+'\n')
s=(p/'install-windowserver.sh').read_text()
s=s.replace('# Native Tahoe transaction.', '# Combined native Tahoe graphics and ALC295 audio transaction.')
s=s.replace("echo 'Checking the existing Dell identity and qualified Tahoe graphics families...'", "echo 'Checking the existing Dell identity and qualified Tahoe graphics/audio package...'")
needle='BKC=/System/Library/KernelCollections/BootKernelExtensions.kc'
s=s.replace(needle,'''# SimpleHDA restores the stock HDA stack through EFI. Refuse competing root-installed donors.
for conflict in /Library/Extensions/AppleHDA.kext /Library/Extensions/VoodooHDA.kext /Library/Extensions/HDAUniversal.kext;do
 [ ! -e "$conflict" ] || { echo "Competing audio restoration found: $conflict. Remove it using its original uninstaller first.";exit 2; }
done
SIMPLEHDA="$EFIMOUNT/EFI/OC/Kexts/SimpleHDA.kext"
[ "$(hash "$EFIMOUNT/EFI/OC/Kexts/AppleALC.kext/Contents/MacOS/AppleALC")" = "$(cat "$HERE/Audio/AppleALC-SHA256.txt")" ] || { echo 'AppleALC differs from the confirmed Dell build; refusing an unqualified replacement.';exit 2; }
codesign --verify --strict "$HERE/EFI-Kexts/SimpleHDA.kext"
AUDIO_MODE=$(/usr/bin/awk -v h="$current" '$1==h {print $2}' "$HERE/Audio/PROFILE-MAP.txt")
case "$AUDIO_MODE" in base|accelerated|recovery) ;; *) echo 'No matching audio-preserving configuration profile.';exit 2;;esac
AUDIO_CFG="$HERE/Audio/Profiles/$AUDIO_MODE.plist"
'''+needle)
s=s.replace('"$AUX" "$CFG" )','"$AUX" "$CFG" "$SIMPLEHDA" )')
s=s.replace('if [ "$path" = "$CFG" ] && ! /usr/sbin/diskutil info "$EFIMOUNT" >/dev/null 2>&1;then', 'if { [ "$path" = "$CFG" ] || [ "$path" = "$SIMPLEHDA" ]; } && ! /usr/sbin/diskutil info "$EFIMOUNT" >/dev/null 2>&1;then')
s=s.replace('"$HERE/tahoe-runtime-check" --config "$CFG" "$WORK/graphics-config.plist" accelerated "$current"', '"$HERE/tahoe-runtime-check" --config "$AUDIO_CFG" "$WORK/graphics-config.plist" accelerated "$(hash "$AUDIO_CFG")"\n[ "$(hash "$WORK/graphics-config.plist")" = "$(hash "$HERE/Profiles/accelerated.plist")" ] || { echo \'Combined profile differs from the qualified configuration.\';exit 2; }')
s=s.replace('case "$path" in "$AUX"|"$CFG"|/Library/Preferences/FeatureFlags/Domain/CoreMedia.plist)', 'case "$path" in "$AUX"|"$CFG"|"$SIMPLEHDA"|/Library/Preferences/FeatureFlags/Domain/CoreMedia.plist)')
s=s.replace('# Apply the qualified config only after the new permanent-path collection validates.', '''# Commit the audio kext before enabling its EFI entry. The common transaction backs it up and rolls it back.
rm -rf "$SIMPLEHDA"
ditto --norsrc --noextattr "$HERE/EFI-Kexts/SimpleHDA.kext" "$SIMPLEHDA"
codesign --verify --strict "$SIMPLEHDA"
cmp -s "$HERE/EFI-Kexts/SimpleHDA.kext/Contents/MacOS/SimpleHDA" "$SIMPLEHDA/Contents/MacOS/SimpleHDA"
# Apply the qualified combined config only after the new permanent-path collection validates.''')
s=s.replace('state=installed-and-prelinked\\nphysical-acceleration-verified=no', 'state=installed-and-prelinked\\naudio=SimpleHDA1.0.0-AppleALC-layout33-staged-for-next-boot\\nphysical-audio-verified=no\\nphysical-acceleration-verified=no')
s=s.replace('Tahoe graphics installed and prelinked. Backup:', 'Tahoe graphics installed/prelinked and ALC295 audio staged in EFI. Backup:')
s=s.replace("echo 'Allow the graphics extensions", "echo 'Audio takes effect after reboot; no System-volume audio patch or automatic HAL installation was used.'\necho 'Allow the graphics extensions")
(p/'install-windowserver.sh').write_text(s)
s=(p/'rollback-windowserver.sh').read_text()
s=s.replace('/Volumes/*/EFI/OC/config.plist)', '/Volumes/*/EFI/OC/config.plist|/Volumes/*/EFI/OC/Kexts/SimpleHDA.kext)')
s=s.replace('mount="${path%/EFI/OC/config.plist}"', 'case "$path" in */config.plist) mount="${path%/EFI/OC/config.plist}";; *) mount="${path%/EFI/OC/Kexts/SimpleHDA.kext}";;esac')
s=s.replace('[ -f "$path" ] && [ "$uuid"', '[ "$uuid"')
s=s.replace('Prior graphics files, feature flag, USB profile and cache files restored.', 'Prior graphics/audio files, feature flag, USB profile and cache files restored.')
(p/'rollback-windowserver.sh').write_text(s)
status=json.loads((p/'STATUS.json').read_text());status.update(candidate='Tahoe graphics0.3.20 + ALC295 audio1.0.0',combinedInstaller=True,audioLayout=33,audioUSBInjection='SimpleHDA1.0.0 after AppleALC; Darwin25 only',AMFIBypassPreserved=True,audioVerifiedOnDell=False,USBPublished=False,installerTransactionCasesPassed=0);(p/'STATUS.json').write_text(json.dumps(status,indent=2)+'\n')
(p/'README.md').write_text((p/'README.md').read_text()+'''\n## Combined audio installation\n\nThe same install-windowserver.sh now stages SimpleHDA1.0.0 into the prepared USB EFI and enables AppleALC on Darwin25, preserving the confirmed ALC295 layout33/controller mapping and existing AMFI bypass. Lilu loads before AppleALC, followed by SimpleHDA. The normal accelerated and explicit recovery profiles both retain audio. Existing and newly generated config hashes are accepted for the transition; unrelated edits are refused.\n\nAudio and graphics share the existing file/EFI/cache backup and rollback transaction. The audio kext is copied and signature-checked before the combined configuration is committed. No System-volume AppleHDA patch or automatic optional HAL package is installed. Competing root-installed AppleHDA/VoodooHDA/HDAUniversal restores are refused. Audio support is prepared for the next normal Tahoe boot, not claimed verified by installation or enabled in Recovery.\n\nThis combined update is staged on Corsair until the USB returns from the Dell. Run only on installed Tahoe, using the unchanged command above.\n''')
for f in p.rglob('*.sh'):subprocess.run(['/bin/bash','-n',str(f)],check=True)
print('Combined installer and rollback created; AMFI, input, network and ALC295 properties preserved.')
