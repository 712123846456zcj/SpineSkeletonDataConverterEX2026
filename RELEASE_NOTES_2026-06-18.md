# Release Notes (2026-06-18)

## Included binaries
- `SpineSkeletonDataConverter.exe`
- `SpineAtlasDowngrade.exe`

## Highlights
- Added fallback detection for non-standard Spine JSON files whose version metadata is not present near the file header.
- Added compatibility hash generation for JSON output when `skeleton.hash` is missing.
- Added initial support for upgrading Spine `2.1.x` JSON input to Spine `3.8` JSON output.

## Notes
- Current Spine `2.1.x` support is limited to **JSON input -> 3.8 JSON output**.
- After converting to 3.8 JSON, you can continue using the existing 3.8 -> newer-version conversion flow.
