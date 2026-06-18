#!/usr/bin/env python3
import json
import subprocess
import sys
from pathlib import Path

if len(sys.argv) != 2:
    print('usage: regression_418_s27_hash.py <converter-exe>')
    sys.exit(2)

exe = Path(sys.argv[1])
repo = Path(__file__).resolve().parent.parent
input_file = repo / '418.json'
output_file = repo / '418.to38.hash-check.json'

if not input_file.exists():
    print('SKIP: 418.json sample not present')
    sys.exit(0)

if output_file.exists():
    output_file.unlink()

result = subprocess.run(
    [str(exe), str(input_file), str(output_file), '-v', '3.8.75'],
    capture_output=True,
    text=True,
)

print('RETURNCODE', result.returncode)
print('STDOUT_START')
print(result.stdout)
print('STDOUT_END')
print('STDERR_START')
print(result.stderr)
print('STDERR_END')

if result.returncode != 0:
    print('UNEXPECTED FAILURE: 2.1 -> 3.8 conversion should succeed for 418.json')
    sys.exit(1)

out_obj = json.loads(output_file.read_text(encoding='utf-8'))
out_hash = out_obj.get('skeleton', {}).get('hash')
if not isinstance(out_hash, str) or not out_hash:
    print('EXPECTED RED: 2.1 -> 3.8 output is missing skeleton.hash for 418.json')
    sys.exit(1)

print('PASS: 2.1 -> 3.8 output contains skeleton.hash for 418.json')
