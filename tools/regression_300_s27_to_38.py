#!/usr/bin/env python3
import json
import subprocess
import sys
from pathlib import Path

if len(sys.argv) != 2:
    print('usage: regression_300_s27_to_38.py <converter-exe>')
    sys.exit(2)

exe = Path(sys.argv[1])
repo = Path(__file__).resolve().parent.parent
input_file = repo / '300_s27.json'
output_file = repo / '300_s27.to38.test.json'

if not input_file.exists():
    print('SKIP: 300_s27.json sample not present')
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
    print('EXPECTED RED: 2.1 JSON input is not supported yet')
    sys.exit(1)

out_obj = json.loads(output_file.read_text(encoding='utf-8'))
assert out_obj['skeleton']['spine'].startswith('3.8')
assert 'bones' in out_obj and out_obj['bones']
assert 'animations' in out_obj and out_obj['animations']
assert any(b['name'] == 'root' for b in out_obj['bones'])
assert '300000' in out_obj['animations']
assert out_obj['skins'][0]['name'] == 'default'

bad = subprocess.run(
    [str(exe), str(input_file), str(output_file), '-v', '4.2.11'],
    capture_output=True,
    text=True,
)
assert bad.returncode != 0
assert '3.8 JSON only' in (bad.stdout + bad.stderr)

print('PASS: 2.1 JSON upgraded to 3.8 JSON and unsupported targets are rejected clearly')
