import sys
import wave
import numpy as np

def wav_to_c_array(wav_file_path, output_c_file_path, array_name="audio_data"):
    """
    Converts a WAV file to a C array of PCM data.

    Args:
        wav_file_path (str): Path to the input WAV file.
        output_c_file_path (str): Path to save the C array file.
        array_name (str): Name of the C array in the output file.
    """
    with wave.open(wav_file_path, 'rb') as wf:
        n_channels = wf.getnchannels()
        samp_width = wf.getsampwidth()
        frame_rate = wf.getframerate()
        n_frames = wf.getnframes()

        # Read all audio frames
        frames = wf.readframes(n_frames)

        # Convert bytes to numpy array of appropriate type
        if samp_width == 1:  # 8-bit unsigned
            dtype = np.uint8
        elif samp_width == 2:  # 16-bit signed
            dtype = np.int16
        elif samp_width == 3:  # 24-bit signed (often padded to 4 bytes)
            # Special handling for 24-bit, as numpy doesn't have a direct 24-bit type
            # You might need to read as bytes and manually combine, or use a library like soundfile
            print("Warning: 24-bit WAV files might require more complex handling.")
            return
        elif samp_width == 4:  # 32-bit signed
            dtype = np.int32
        else:
            print(f"Unsupported sample width: {samp_width} bytes.")
            return

        audio_data = np.frombuffer(frames, dtype=dtype)

        # Write to C array file
        with open(output_c_file_path, 'w') as f:
            f.write(f"// Audio data from '{wav_file_path}'\n")
            f.write(f"// Channels: {n_channels}, Sample Rate: {frame_rate} Hz, Sample Width: {samp_width * 8}-bit\n")
            f.write(f"const {dtype.__name__}_t {array_name}[] = {{\n")
            
            # Format data for C array, e.g., 16 values per line
            for i, sample in enumerate(audio_data):
                f.write(str(sample))
                if i < len(audio_data) - 1:
                    f.write(", ")
                if (i + 1) % 16 == 0 and i < len(audio_data) - 1:
                    f.write("\n")
            f.write("\n};\n")
            # f.write(f"const size_t {array_name}_len = sizeof({array_name}) / sizeof({array_name}[0]);\n")



fromfile = sys.argv[1]
tofile = sys.argv[2]
wav_to_c_array(fromfile, tofile)