from pathlib import Path
import os,runpy
root=Path(__file__).resolve().parent
os.environ['NVMTL_PLUGIN_SOURCE']=str(root/'Graphics-source/plugin')
runpy.run_path(str(root/'Graphics-A8-0.3.21/build.py'),run_name='__main__')
