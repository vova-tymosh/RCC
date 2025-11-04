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
import os

def convert_wav_to_sound(wav_file_path, output_dir):
    """
    Converts a WAV file to raw PCM data for filesystem.

    Args:
        wav_file_path (str): Path to the input WAV file.
        output_dir (str): Directory to save the raw PCM file.
    """
    # Validate WAV file
    with wave.open(wav_file_path, 'rb') as wf:
        n_channels = wf.getnchannels()
        samp_width = wf.getsampwidth()
        frame_rate = wf.getframerate()
        n_frames = wf.getnframes()

        # Validate format
        if samp_width != 2:
            print(f"ERROR: {wav_file_path} must be 16-bit PCM, got {samp_width * 8}-bit")
            sys.exit(1)

        if frame_rate != 16000:
            print(f"ERROR: {wav_file_path} sample rate must be 16000 Hz, got {frame_rate} Hz")
            sys.exit(1)

        # Read all audio frames (raw PCM data)
        frames = wf.readframes(n_frames)

    # Extract filename without extension
    filename = os.path.splitext(os.path.basename(wav_file_path))[0]
    output_path = os.path.join(output_dir, filename)

    # Write raw PCM data to file
    with open(output_path, 'wb') as f:
        f.write(frames)


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: wav2sound.py <wav_file> <output_dir>")
        print("Example: wav2sound.py horn.wav data/sounds")
        sys.exit(1)

    wav_file = sys.argv[1]
    output_dir = sys.argv[2]

    if not os.path.exists(wav_file):
        print(f"ERROR: File not found: {wav_file}")
        sys.exit(1)

    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    convert_wav_to_sound(wav_file, output_dir)
