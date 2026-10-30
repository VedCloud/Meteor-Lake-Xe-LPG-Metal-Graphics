# SPDX-License-Identifier: MIT
"""Serialize observations of this owned QEMU instance, including stop/cont pairs."""
from contextlib import contextmanager
import fcntl,socket
@contextmanager
def owned_monitor(run):
 with (run/'HMP-READ.lock').open('a') as guard:
  fcntl.flock(guard,fcntl.LOCK_EX)
  with socket.socket(socket.AF_UNIX) as sock:
   yield sock
