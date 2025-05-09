#!/bin/env python3

# =========================================================================
# Copyright (C) 2025 Pavol Federl (pfederl@ucalgary.ca)
# All Rights Reserved. Do not distribute this file.
# =========================================================================
# DO NOT EDIT THIS FILE. DO NOT SUBMIT THIS FILE FOR GRADING.
#
#
# ==========================================================================
# very inefficient (O(N^2)) implementation of the fatsim assignment
# ==========================================================================

import sys, os, time

def fmt_seconds(s):
  h = int(s) // 3600
  m = (int(s) % 3600) // 60
  s = s - h * 3600 - m * 60
  return f"{h:02d}:{m:02d}:{s:05.2f}"

def trace(b, fat):
  length = 1
  while True:
    if fat[b] == -1:
      return b, length
    if length >= len(fat):
      return None, 0
    length += 1
    b = fat[b]

def fat_check(fat):
  start_time = time.time()
  last_report_time = time.time() + 2
  maxl, maxb = 0, []
  for b in range(len(fat)):
    curr_time = time.time()
    if curr_time - last_report_time > 1:
      last_report_time = curr_time
      elapsed = curr_time - start_time
      eta = (len(fat) - b) * (elapsed / (b+1e-6))
      etastr = fmt_seconds(eta)
      done = f"{100 * b / len(fat):3.0f}"
      report = f"Processed {done}%, ETA = {etastr}" + (" " * 40)
      print(report, end="\n", flush=True)
    tb, blen = trace(b, fat)
    if tb is None: continue
    if blen > maxl:
      maxl, maxb = blen, [b]
    elif blen == maxl:
      maxb.append(b)
    # print(f"{maxl=} {maxb=}")
  return sorted(maxb)

def main(argv):
  fat = [int(e) for e in sys.stdin.read().split()]
  print(f"FAT has {len(fat)} entries.")
  for b in fat:
    if b < -1: raise Exception("too negative")
  if max(fat) > len(fat)-1:
    raise Exception("some entries too big")
  start_time = time.time()

  result = fat_check(fat)
  line = "blocks:"
  for n in result:
    line += f" {n}"
    if len(line) > 80:
      print(line)
      line = " "
  if len(line) > 1:
    print(line)

  elapsed = time.time() - start_time
  print(f"elapsed time: {elapsed:.2f}s")
  

if __name__ == "__main__":
    main(sys.argv)
