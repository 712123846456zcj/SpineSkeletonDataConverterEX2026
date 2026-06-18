#!/usr/bin/env python3
import subprocess
import sys
from pathlib import Path

if len(sys.argv) != 2:
    print('usage: regression_300_s38.py <converter-exe>')
    sys.exit(2)

exe = Path(sys.argv[1])
repo = Path(__file__).resolve().parent.parent
input_file = repo / '300_s38.json'
out_file = repo / '300_s38.compat-test.json'

if not input_file.exists():
    print('SKIP: 300_s38.json sample not present')
    sys.exit(0)

if out_file.exists():
    out_file.unlink()

result = subprocess.run(
    [str(exe), str(input_file), str(out_file), '-v', '4.2.11'],
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
    print('UNEXPECTED FAILURE: 300_s38.json should now convert successfully')
    sys.exit(1)

combined = result.stdout + result.stderr
if 'version resolved from parsed JSON instead of file header' not in combined:
    print('UNEXPECTED RESULT: missing non-standard JSON compatibility warning')
    sys.exit(1)

print('PASS: 300_s38.json converts successfully with non-standard JSON warning')
