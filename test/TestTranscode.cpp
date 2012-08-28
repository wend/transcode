
#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
char* flv2mp3(char* src, int src_len, int* dst_len);
char* multiflv2mp3(char* src, int* src_len, int src_size, int* dst_len);
char* raw2flv(char* src, int src_len, int* dst_len);
#ifdef __cplusplus
}
#endif
#include <string>

static int open_inputfile(char *inputfilename, uint8_t **buf)
{
    FILE *f;
    f = fopen(inputfilename, "rb");
    if (!f) 
    {
        fprintf(stderr, "could not open %s\n", inputfilename);
        return -1;
    }
    fseek(f, 0L, SEEK_END);
    int file_size = ftell(f);
    rewind(f);
    *buf = new uint8_t[file_size];
    fread(*buf, 1, file_size, f);

    fclose(f);
    return file_size;
}

static int write_file(char *filename, uint8_t *buf, int len)
{
    FILE *f;
    f = fopen(filename, "wb");
    if (!f) 
    {
        fprintf(stderr, "could not open %s\n", filename);
        return -1;
    }
    fwrite(buf, 1, len, f);
    fclose(f);
    return 0;
}

int main()
{
    uint8_t *buf;
    //int file_size = open_inputfile("d:\\test\\20.flv", &buf);
    int file_size = open_inputfile("d:\\test\\test.raw", &buf);
    int len;
    //char *p = flv2mp3((char*)buf, file_size, &len);
    char* p = raw2flv((char*)buf, file_size, &len);
    write_file("d:\\test\\test.raw.flv", (uint8_t *)p, len);
    free(p);
    delete buf;

    file_size = open_inputfile("d:\\test\\test.raw.flv", &buf);
    p = flv2mp3((char*)buf, file_size, &len);
    write_file("d:\\test\\test.raw.mp3", (uint8_t *)p, len);
    free(p);
    delete buf;

    return  0;
}
