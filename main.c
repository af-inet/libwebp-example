#include <stdio.h>
#include <stdlib.h>
#include <webp/encode.h>
#include <webp/mux.h>

// TODO: free memory

int write_file(const char *filename, const uint8_t *data, size_t data_size)
{
	FILE *out = fopen(filename, "wb");
	size_t ok;
	if (out == NULL)
	{
		fprintf(stderr, "Error! Cannot open output file '%s'\n", filename);
		return 0;
	}
	ok = fwrite(data, data_size, 1, out);
	if (ok != 1)
	{
		fprintf(stderr, "unexpected fwrite() = %zu on file '%s' \n", ok, filename);
	}
	fclose(out);
	return ok;
}

void fill(uint8_t *data, int width, int height, int r, int g, int b, int a)
{
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			data[(y * width * 4) + (x * 4) + 0] = r;
			data[(y * width * 4) + (x * 4) + 1] = g;
			data[(y * width * 4) + (x * 4) + 2] = b;
			data[(y * width * 4) + (x * 4) + 3] = a;
		}
	}
}

int main(int argc, char *argv[])
{
	WebPPicture frame;
	WebPAnimEncoder *enc = NULL;
	WebPAnimEncoderOptions enc_options;
	WebPConfig config;
	WebPData webp_data;
	const int width = 64;
	const int height = 64;
	// 1 second frame duration
	const int frame_duration = 1000;
	const int nframes = 6;
	int frame_timestamp = 0;
	// encode a new frame
	uint8_t frame_rgba[width * height * 4];

	WebPDataInit(&webp_data);

	if (!WebPConfigInit(&config) || !WebPAnimEncoderOptionsInit(&enc_options) ||
		!WebPPictureInit(&frame))
	{
		fprintf(stderr, "Error! Version mismatch!\n");
		return 1;
	}

	// initialize frame
	frame.width = width;
	frame.height = height;
	if (!WebPPictureAlloc(&frame))
	{
		fprintf(stderr, "WebPPictureAlloc failed.\n");
		return 1;
	}

	// initialize encoder
	enc = WebPAnimEncoderNew(frame.width, frame.height,
							 &enc_options);
	if (enc == NULL)
	{
		fprintf(stderr,
				"Error! Could not create encoder object. Possibly due to "
				"a memory error.\n");
		return 1;
	}

	for (int n = 0; n < nframes; n++)
	{
		switch (n % 3)
		{
		case 0:
			fill(frame_rgba, width, height, 255, 0, 0, 255);
			break;
		case 1:
			fill(frame_rgba, width, height, 0, 255, 0, 128);
			break;
		case 2:
			fill(frame_rgba, width, height, 0, 0, 255, 255);
			break;
		}
		WebPPictureImportRGBA(&frame, frame_rgba, width); // *rgb buffer should have a size of at least height * rgb_stride.
		if (!WebPAnimEncoderAdd(enc, &frame, frame_timestamp, &config))
		{
			fprintf(stderr, "Error while adding frame #%d: %s\n", n,
					WebPAnimEncoderGetError(enc));
			return 1;
		}
		frame_timestamp += frame_duration;
	}

	// export the webp data
	if (!WebPAnimEncoderAssemble(enc, &webp_data))
	{
		fprintf(stderr, "%s\n", WebPAnimEncoderGetError(enc));
		return 1;
	}

	write_file("out.webp", webp_data.bytes, webp_data.size);

	return 0;
}
