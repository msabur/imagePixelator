/*
 * Converts a regular PPM image into a pixelated version
 * Input: a .ppm image
 * Output: a ppm image "output.ppm"
 */

#include "libs/argparse.h"
#include "libs/ppm_io.h"
#include <cstring>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>

static const char *const usage[] = {
	"pixelator [options] ppmfile",
	NULL,
};

#define SQUARE(x) (x * x)
#define ABORT(...)                                                             \
	do {                                                                         \
		printf(__VA_ARGS__);                                                       \
		exit(1);                                                                   \
	} while (0)

int main(int argc, const char *argv[]) {
	const char *infile = NULL, *outfile = "output.ppm";
	unsigned pixelWidth = 30;

	struct argparse_option options[] = {
		OPT_HELP(),
		OPT_GROUP("Optional parameters"),
		OPT_STRING('o', "output-file", &outfile,
				"name of output file (default is output.ppm)"),
		OPT_INTEGER('p', "pixel-width", &pixelWidth,
				"how wide to make each pixel (default is 30)"),
		OPT_END(),
	};

	struct argparse argparse;
	argparse_init(&argparse, options, usage, 0);
	argparse_describe(&argparse, "\nPixelate a PPM image.", "\n");
	argc = argparse_parse(&argparse, argc, argv);

	if (argc > 0) {
		infile = argv[0];
	} else {
		ABORT("Error: no input file given\n");
	}

	PPM img;
	if (img.read(infile) != 0) {
		ABORT("Opening file failed, exiting\n");
	}

	int height = img.getH(), width = img.getW();
	uint8_t *pImage = img.getImageHandler();
	uint8_t data[height * width * 3];
	std::memcpy(data, pImage, sizeof(data) / sizeof(*data));

	for (int i = pixelWidth; i < height; i += pixelWidth) {
		for (int j = pixelWidth; j < width; j += pixelWidth) {

			// calculate average color of each block
			unsigned sum[3] = {0};
			for (int x = i - pixelWidth; x < i; x++) {
				for (int y = j - pixelWidth; y < j; y++) {
					unsigned idx = (x * width + y) * 3;
					sum[0] += data[idx];
					sum[1] += data[idx + 1];
					sum[2] += data[idx + 2];
				}
			}
			unsigned average[3] = {sum[0] / SQUARE(pixelWidth),
				sum[1] / SQUARE(pixelWidth),
				sum[2] / SQUARE(pixelWidth)};

			// set the average color inside the whole box
			for (int x = i - pixelWidth; x < i; x++) {
				for (int y = j - pixelWidth; y < j; y++) {
					unsigned idx = (x * width + y) * 3;
					data[idx] = average[0];
					data[idx + 1] = average[1];
					data[idx + 2] = average[2];
				}
			}
		}
	}

	img.load(data, img.getH(), img.getW(), img.getMax(), img.getMagic());
	img.write(outfile);
}
