"""Crop the photographed Keyes modules and create README-ready WebP assets."""

from __future__ import annotations

import argparse
from pathlib import Path

import numpy as np
from PIL import Image, ImageOps


COMPONENTS_DIR = Path(__file__).resolve().parents[1] / "components"

PHOTOS = {
    "keyestudio-ks0037-vibration": "IMG_20261024_203426885_HDR.jpg",
    "keyestudio-ks0019-passive-buzzer": "IMG_20261024_203440803_HDR.jpg",
    "keyes-k855396-straw-hat-led": "IMG_20261024_203450304_HDR.jpg",
    "keyestudio-ks0033-thermistor": "IMG_20261024_203506743_HDR.jpg",
    "keyestudio-ks0024-knock": "IMG_20261024_203515354_HDR.jpg",
    "keyestudio-ks0028-photocell": "IMG_20261024_203524523_HDR.jpg",
    "keyestudio-ks0014-analog-rotation": "IMG_20261024_203538683_HDR.jpg",
    "keyestudio-ks0023-ds18b20": "IMG_20261024_203559430_BURST000_COVER.jpg",
    "keyestudio-ks0029-push-button": "IMG_20261024_203615490_HDR.jpg",
    "keyestudio-ks0038-reed-switch": "IMG_20261024_203624532_HDR.jpg",
    "keyestudio-ks0035-microphone": "IMG_20261024_203651498_HDR.jpg",
    "keyestudio-ks0025-tilt": "IMG_20261024_203705101_HDR.jpg",
    "keyestudio-ks0026-ir-receiver": "IMG_20261024_203723952_HDR.jpg",
    "keyestudio-ks0018-active-buzzer": "IMG_20261024_203734801_HDR.jpg",
    "keyestudio-ks0027-ir-transmitter": "IMG_20261024_203743461_HDR.jpg",
    "keyestudio-ks0022-lm35": "IMG_20261024_203804368_HDR.jpg",
    "keyestudio-ks0021-collision": "IMG_20261024_203811921_HDR.jpg",
    "keyestudio-ks0032-rgb-led": "IMG_20261024_203820913_HDR.jpg",
}

SECONDARY_PHOTOS = {
    "keyes-k855396-straw-hat-led": "IMG_20261024_203754423_HDR.jpg",
}


def crop_around_board(image: Image.Image) -> Image.Image:
    """Find the red PCB, then retain generous room for protruding components."""
    pixels = np.asarray(image.convert("RGB"))
    red = pixels[:, :, 0].astype(np.int16)
    green = pixels[:, :, 1].astype(np.int16)
    blue = pixels[:, :, 2].astype(np.int16)
    mask = (red > 125) & (red > green * 1.35) & (red > blue * 1.20)
    ys, xs = np.nonzero(mask)
    if len(xs) < 10_000:
        raise RuntimeError("Could not locate the red PCB")

    left, right = int(np.percentile(xs, 0.2)), int(np.percentile(xs, 99.8))
    top, bottom = int(np.percentile(ys, 0.2)), int(np.percentile(ys, 99.8))
    width, height = right - left, bottom - top
    pad_x, pad_y = int(width * 0.22), int(height * 0.18)
    box = (
        max(0, left - pad_x),
        max(0, top - pad_y),
        min(image.width, right + pad_x),
        min(image.height, bottom + pad_y),
    )
    return image.crop(box)


def fit(image: Image.Image, max_size: tuple[int, int]) -> Image.Image:
    result = image.copy()
    result.thumbnail(max_size, Image.Resampling.LANCZOS)
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("source_dir", type=Path, help="Directory containing the source JPEGs")
    args = parser.parse_args()

    for component, filename in PHOTOS.items():
        source = args.source_dir / filename
        destination = COMPONENTS_DIR / component
        with Image.open(source) as opened:
            image = ImageOps.exif_transpose(opened).convert("RGB")
            crop = crop_around_board(image)
            fit(crop, (1400, 1050)).save(
                destination / "image.webp", "WEBP", quality=90, method=6
            )
            fit(crop, (240, 180)).save(
                destination / "thumbnail.webp", "WEBP", quality=90, method=6
            )
        print(f"{component}: {filename}")

    for component, filename in SECONDARY_PHOTOS.items():
        source = args.source_dir / filename
        destination = COMPONENTS_DIR / component
        with Image.open(source) as opened:
            image = ImageOps.exif_transpose(opened).convert("RGB")
            crop = crop_around_board(image)
            fit(crop, (1400, 1050)).save(
                destination / "image-2.webp", "WEBP", quality=90, method=6
            )
            fit(crop, (240, 180)).save(
                destination / "thumbnail-2.webp", "WEBP", quality=90, method=6
            )
        print(f"{component} (secondary): {filename}")


if __name__ == "__main__":
    main()
