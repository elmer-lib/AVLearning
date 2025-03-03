//
// Created by 周豪豪 on 2025/3/3.
//

#include "AVJpegEncode.h"
// https://github.com/LuaDist/libjpeg/blob/master/example.c

AVResult AVJpegEncode::writeJpegFile(const char *filePath, int quality, int imageWidth, int imageHeight, uint8_t* imageData) {
    /* This struct contains the JPEG compression parameters and pointers to
     * working space (which is allocated as needed by the JPEG library).
     * It is possible to have several such structures, representing multiple
     * compression/decompression processes, in existence at once.  We refer
     * to any one struct (and its associated working data) as a "JPEG object".
    */
    struct jpeg_compress_struct cinfo;
    /* This struct represents a JPEG error handler.  It is declared separately
     * because applications often want to supply a specialized error handler
     * (see the second half of this file for an example).  But here we just
     * take the easy way out and use the standard error handler, which will
     * print a message on stderr and call exit() if compression fails.
     * Note that this struct must live as long as the main JPEG parameter
     * struct, to avoid dangling-pointer problems.
     */
    struct jpeg_error_mgr jerr;

    FILE* outfile;		        /* target file */
    JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
    int row_stride;		        /* physical row width in image buffer */

    cinfo.err = jpeg_std_error(&jerr); // We have to set up the error handler first, in case the initialization step fails
    // 1. allocate and initialize JPEG compression object
    jpeg_create_compress(&cinfo); // initialize the JPEG compression object
    // 2. specify data destination Note: steps 2 and 3 can be done in either order
    if ((outfile = fopen(filePath, "wb")) == NULL) {
        LOGE("writeJpegFile open file error for %s", filePath);
        return AV_ERROR;
    }
    jpeg_stdio_dest(&cinfo, outfile);
    // 3. set parameters for compression
    cinfo.image_width = imageWidth;         // image width and height, in pixels
    cinfo.image_height = imageHeight;
    cinfo.input_components = 4;             // of color components per pixel
    cinfo.in_color_space = JCS_EXT_RGBA;    // colorSpace of input image
    jpeg_set_defaults(&cinfo);              // set default compression parameters. You must set at least cinfo.in_color_space before calling this, since the defaults depend on the source color space.
    jpeg_set_quality(&cinfo, quality, TRUE);    // limit to baseline-JPEG values
    // 4. Start compressor
    jpeg_start_compress(&cinfo, TRUE);
    // 5. while (scan lines remain to be written) jpeg_write_scanlines(...)
    row_stride = imageWidth * 4;
    while (cinfo.next_scanline < cinfo.image_height) {
        /* jpeg_write_scanlines expects an array of pointers to scanlines.
         * Here the array is only one element long, but you could pass
         * more than one scanline at a time if that's more convenient.
        */
        row_pointer[0] = &imageData[cinfo.next_scanline * row_stride];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    // 6. Finish compression
    jpeg_finish_compress(&cinfo);
    // 7. release resource
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);
    return AV_OK;
}

struct redefined_jpeg_error_mgr {
    struct jpeg_error_mgr pub;  // "public" fields
    jmp_buf setjmp_buffer;      // for return to caller
};

AVResult AVJpegEncode::readJpegFile(const char *filePath, int& outImageWidth, int outImageHeight, uint8_t* outImageData) {
    /* This struct contains the JPEG decompression parameters and pointers to
     * working space (which is allocated as needed by the JPEG library).
     */
    struct jpeg_decompress_struct cinfo;
    /* We use our private extension JPEG error handler.
     * Note that this struct must live as long as the main JPEG parameter
     * struct, to avoid dangling-pointer problems.
     */
    struct redefined_jpeg_error_mgr jerr;
    FILE* infile;		        /* target file */
    JSAMPARRAY buffer;		    /* Output row buffer */
    int row_stride;		        /* physical row width in output buffer */

    if ((infile = fopen(filePath, "rb")) == NULL) {
        LOGE("readJpegFile open file error for %s", filePath);
        return AV_ERROR;
    }

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = [](j_common_ptr cinfo){
        // cinfo->err really points to a my_error_mgr struct, so coerce pointer
        redefined_jpeg_error_mgr* errorMgr = (redefined_jpeg_error_mgr*)cinfo->err;
        // Always display the message.
        (*cinfo->err->output_message) (cinfo);
        // Return control to the setjmp point
        longjmp(errorMgr->setjmp_buffer, 1);
    };
    // Establish the setjmp return context for my_error_exit to use
    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        return AV_ERROR;
    }

    // 1. allocate and initialize JPEG decompression object
    jpeg_create_decompress(&cinfo);
    // 2. specify data source
    jpeg_stdio_src(&cinfo, infile);
    // 3. read file parameters with jpeg_read_header()
    jpeg_read_header(&cinfo, TRUE);
    // 4. set parameters for decompression
    /* In this example, we don't need to change any of the defaults set by
     * jpeg_read_header(), so we do nothing here.
    */
    // 5. Start decompressor
    jpeg_start_decompress(&cinfo);
    /* We may need to do some setup of our own at this point before reading
     * the data.  After jpeg_start_decompress() we have the correct scaled
     * output image dimensions available, as well as the output colormap
     * if we asked for color quantization.
     * In this example, we need to make an output work buffer of the right size.
    */
    row_stride = cinfo.output_width * cinfo.output_components;
    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);
    // 6. while (scan lines remain to be read) jpeg_read_scanlines(...);
    while (cinfo.output_scanline < cinfo.output_height) {
        /* jpeg_read_scanlines expects an array of pointers to scanlines.
         * Here the array is only one element long, but you could ask for
         * more than one scanline at a time if that's more convenient.
        */
        jpeg_read_scanlines(&cinfo, buffer, 1);
        // Assume put_scanline_someplace wants a pointer and sample count
        // put_scanline_someplace(buffer[0], row_stride);
    }
    // 7. Finish decompression
    jpeg_finish_decompress(&cinfo);
    // 8. release resource
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return AV_OK;
}
