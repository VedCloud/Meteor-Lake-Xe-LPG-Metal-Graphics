from pathlib import Path
import tempfile,shutil,subprocess,json,concurrent.futures,hashlib
root=Path(__file__).resolve().parent.parent;source=root/'research/freetype-build/freetype-2.14.3';out=root/'desktop-build/freetype-static';out.mkdir(exist_ok=True)
with tempfile.TemporaryDirectory(prefix='latitude-freetype-') as tmp:
 stage=Path(tmp);shutil.copytree(source,stage/'source')
 config=stage/'source/include/freetype/config/ftoption.h';text=config.read_text();assert '/* #define FT_CONFIG_OPTION_SUBPIXEL_RENDERING */' in text;config.write_text(text.replace('/* #define FT_CONFIG_OPTION_SUBPIXEL_RENDERING */','#define FT_CONFIG_OPTION_SUBPIXEL_RENDERING'))
 def build(arch):
  build=stage/arch;build.mkdir();env=dict(__import__('os').environ);env.update(CC='clang -arch '+arch,CFLAGS='-O2 -mmacosx-version-min=15.0',LDFLAGS='-arch '+arch+' -mmacosx-version-min=15.0')
  with (out/(arch+'.log')).open('w') as log:
   subprocess.run([str(stage/'source/configure'),'--host='+('aarch64' if arch=='arm64' else 'x86_64')+'-apple-darwin','--without-zlib','--without-bzip2','--without-png','--without-harfbuzz','--without-brotli','--enable-static','--disable-shared'],cwd=build,env=env,stdout=log,stderr=subprocess.STDOUT,check=True)
   subprocess.run(['make','-j4'],cwd=build,env=env,stdout=log,stderr=subprocess.STDOUT,check=True)
  dest=out/arch;dest.mkdir(exist_ok=True);shutil.copyfile(build/'.libs/libfreetype.a',dest/'libfreetype.a')
 with concurrent.futures.ThreadPoolExecutor(max_workers=2) as pool:list(pool.map(build,['x86_64','arm64']))
 shutil.copytree(stage/'source/include',out/'include',dirs_exist_ok=True)
 for name in ['FTL.TXT','GPLv2.TXT']:shutil.copyfile(stage/'source/docs'/name,out/name)
 shutil.copyfile(stage/'source/LICENSE.TXT',out/'LICENSE.TXT')
(out/'SOURCE.json').write_text(json.dumps({'version':'2.14.3','source':'https://download.savannah.gnu.org/releases/freetype/freetype-2.14.3.tar.xz','tarSHA256':hashlib.sha256((root/'research/freetype-2.14.3.tar.xz').read_bytes()).hexdigest(),'architectures':['x86_64','arm64'],'externalCompressionAndShapingLibraries':False,'FT_CONFIG_OPTION_SUBPIXEL_RENDERING':True},indent=2)+'\n')
print('Built private FreeType static libraries for Intel target and ARM host-reference tests. No host installation.')
