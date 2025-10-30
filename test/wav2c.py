#!/usr/bin/env python3
# 
# Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
# Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
# For more details go to https://github.com/vova-tymosh/RCC
# 
# The above copyright notice shall be included in all
# copies or substantial portions of the Software.
# 
import sys
import wave
import time
import os
from infra import SerialComm

def upload_wav_to_device(wav_file_path):
    """
    Validates and uploads a WAV file to the device.

    Args:
        wav_file_path (str): Path to the input WAV file.
    """
    # Validate WAV file
    with wave.open(wav_file_path, 'rb') as wf:
        n_channels = wf.getnchannels()
        samp_width = wf.getsampwidth()
        frame_rate = wf.getframerate()
        n_frames = wf.getnframes()

        print(f"WAV file info:")
        print(f"  Channels: {n_channels}")
        print(f"  Sample width: {samp_width * 8}-bit")
        print(f"  Sample rate: {frame_rate} Hz")
        print(f"  Frames: {n_frames}")

        # Validate format
        if samp_width != 2:
            print(f"ERROR: File must be 16-bit PCM, got {samp_width * 8}-bit")
            sys.exit(1)

        if frame_rate != 16000:
            print(f"ERROR: Sample rate must be 16000 Hz, got {frame_rate} Hz")
            sys.exit(1)

        print("✓ WAV file format is valid")

        # Read all audio frames
        frames = wf.readframes(n_frames)
        total_size = len(frames)
        print(f"Total data size: {total_size} bytes")

    # Extract filename without path
    filename = os.path.basename(wav_file_path)
    filename = filename.split('.')
    filename = filename[0]
    device_path = f"/sounds/{filename}"

    # Open serial connection
    print("\nConnecting to device...")
    s = SerialComm.openPort()
    if s is None:
        print('ERROR: No serial port found')
        sys.exit(1)

    print(f"Connected to: {s.description}")

    # Upload data in chunks
    chunk_size = 256  # Match CHUNK_SIZE from device
    offset = 0
    chunks_sent = 0

    print(f"\nUploading to {device_path}...")

    while offset < total_size:
        chunk = frames[offset:offset + chunk_size]
        chunk_len = len(chunk)

        # Send append command
        s.write(f'A{device_path}:{chunk_len}')
        time.sleep(0.05)  # Small delay for device to process

        # Send data
        s.write(chunk)
        
        # Wait for confirmation
        if not s.read(f'Append bytes: {chunk_len}'):
            print(f"\nERROR: Failed to upload chunk at offset {offset}")
            sys.exit(1)

        offset += chunk_len
        chunks_sent += 1

        # Progress indicator
        progress = (offset / total_size) * 100
        print(f"\rProgress: {progress:.1f}% ({offset}/{total_size} bytes)", end='', flush=True)

    print(f"\n✓ Upload complete: {chunks_sent} chunks sent")

    # Verify file size
    s.write(f'Z{device_path}')
    if s.read(f'Size of {device_path}: {total_size}'):
        print(f"✓ File size verified: {total_size} bytes")
    else:
        print("WARNING: Could not verify file size")

    del s
    print("\nDone!")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: wav2c.py <wav_file>")
        print("Example: wav2c.py horn.wav")
        sys.exit(1)

    wav_file = sys.argv[1]

    if not os.path.exists(wav_file):
        print(f"ERROR: File not found: {wav_file}")
        sys.exit(1)

    upload_wav_to_device(wav_file)