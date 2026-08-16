#!/usr/bin/env python3

"""
Converts an equirectangular (2:1) panorama into 6 cubemap face textures.

Usage:
    python Equirect2Cubemap.py input.png output_dir [face_size]

Memory requirements for each panorma:
  - 64k => ~60GiB
  - 32k => ~30GiB
  - 16k => ~8GiB
"""

import sys
import numpy as np
from PIL import Image

FACE_DEFS = {
    'PX': lambda u, v: (np.ones_like(u), -v, -u),
    'NX': lambda u, v: (-np.ones_like(u), -v, u),
    'PY': lambda u, v: (u, np.ones_like(u), v),
    'NY': lambda u, v: (u, -np.ones_like(u), -v),
    'PZ': lambda u, v: (u, -v, np.ones_like(u)),
    'NZ': lambda u, v: (-u, -v, -np.ones_like(u)),
    }


def sample_bilinear(img: np.ndarray, x: np.ndarray, y: np.ndarray) -> np.ndarray:
    h, w = img.shape[:2]

    x0 = np.floor(x).astype(np.int64)
    y0 = np.floor(y).astype(np.int64)
    x1 = x0 + 1
    y1 = y0 + 1

    wx = (x - x0)[..., None]
    wy = (y - y0)[..., None]

    x0m = x0 % w
    x1m = x1 % w
    y0c = np.clip(y0, 0, h - 1)
    y1c = np.clip(y1, 0, h - 1)

    Ia = img[y0c, x0m]
    Ib = img[y0c, x1m]
    Ic = img[y1c, x0m]
    Id = img[y1c, x1m]

    top = Ia * (1 - wx) + Ib * wx
    bottom = Ic * (1 - wx) + Id * wx
    return top * (1 - wy) + bottom * wy


def equirect_to_cubemap(pano: np.ndarray, face_size: int) -> dict[str, np.ndarray]:
    h, w = pano.shape[:2]

    idx = (np.arange(face_size) + 0.5) / face_size * 2 - 1
    u, v = np.meshgrid(idx, idx)

    faces = {}
    for name, dir_fn in FACE_DEFS.items():
        x, y, z = dir_fn(u, v)
        norm = np.sqrt(x * x + y * y + z * z)
        x, y, z = x / norm, y / norm, z / norm

        lon = np.arctan2(x, -z)
        lat = np.arcsin(np.clip(y, -1, 1))

        px = (lon / (2 * np.pi) + 0.5) * w - 0.5
        py = (0.5 - lat / np.pi) * h - 0.5

        faces[name] = sample_bilinear(pano, px, py)

    return faces


def main():
    if len(sys.argv) != 3 or len(sys.argv) > 4:
        print('Usage: python Equirect2Cubemap.py input.png output_dir [face_size]')
        sys.exit(1)

    input_path = sys.argv[1]
    output_dir = sys.argv[2]
    face_size = int(sys.argv[3]) if len(sys.argv) > 3 else None

    import os
    os.makedirs(output_dir, exist_ok=True)

    Image.MAX_IMAGE_PIXELS = 2_147_483_648
    img = Image.open(input_path).convert('RGB')
    w, h = img.size
    if w != 2 * h:
        print(f'Error: input image must be equirectangular (width = 2 * height), got {w}x{h}')
        sys.exit(1)

    if face_size is None:
        face_size = h // 2

    pano = np.asarray(img, dtype=np.float64) / 255.0
    faces = equirect_to_cubemap(pano, face_size)

    for name, face in faces.items():
        face_img = Image.fromarray(np.clip(face * 255, 0, 255).astype(np.uint8))
        out_path = os.path.join(output_dir, f'{name}.png')
        face_img.save(out_path)
        print(f'Saved {out_path}')


if __name__ == '__main__':
    main()
