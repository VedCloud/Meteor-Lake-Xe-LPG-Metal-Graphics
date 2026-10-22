from pathlib import Path
import runpy
# The retired push/pop cache no longer exists. Exercise the live allocator and
# completion-gated retirement rather than an obsolete cache implementation.
r = Path(__file__).resolve().parent
runpy.run_path(str(r / 'test-pool-lifetime.py'), run_name='__main__')
