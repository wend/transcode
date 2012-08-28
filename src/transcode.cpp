// transcode.cpp : Defines the entry point for the console application.
//
//#include "stdafx.h"
#ifdef __cplusplus
extern "C" {
#endif
#include <libavcodec/avcodec.h>
__declspec(dllexport) char* flv2mp3(char* src, int src_len, int* dst_len);
__declspec(dllexport) char* multiflv2mp3(char* src, int* src_len, int src_size, int* dst_len);
__declspec(dllexport) char* raw2flv(char* src, int src_len, int* dst_len);
#ifdef __cplusplus
}
#endif


#include <string>
#include "flv.h"

#define BUFFER_SIZE 1024*10
#define FRAME_SIZE 640
#define IN_SAMPLE_RATE 16000
#define IN_BIT_RATE 32000
#define IN_CHANNELS 1
#define OUT_SAMPLE_RATE 32000
#define OUT_BIT_RATE 48000
#define OUT_CHANNELS 1

// the flv should be only audio
static std::string decode_flv(std::string inputStr)
{
    uint8_t *pSrc = (uint8_t *)inputStr.c_str();
    int src_len = inputStr.size();
    if (src_len < 192 || strncmp((char*)pSrc, "FLV", 3) != 0 || pSrc[8] != 9)
    {
        fprintf(stderr, "not a flv file\n");
        return "";
    }
    AVCodec *pInCodec = NULL;
    AVCodecContext *pInCodecContext= NULL;
    AVPacket avpkt;
    av_init_packet(&avpkt);

    // find the speex audio decoder 
    pInCodec = avcodec_find_decoder(AV_CODEC_ID_SPEEX);
    if (!pInCodec) 
    {
        fprintf(stderr, "decodec not found\n");
        return "";
    }
    pInCodecContext = avcodec_alloc_context3(pInCodec);

    pInCodecContext->channels = IN_CHANNELS;
    pInCodecContext->bit_rate = IN_BIT_RATE;
    pInCodecContext->sample_rate = IN_SAMPLE_RATE;
    pInCodecContext->sample_fmt = AV_SAMPLE_FMT_S16;
    pInCodecContext->frame_size = 320;

    // open it 
    if (avcodec_open2(pInCodecContext, pInCodec, NULL) < 0) 
    {
        av_free(pInCodecContext);
        fprintf(stderr, "could not open codec\n");
        return "";
    }

    uint8_t *pDst = new uint8_t[BUFFER_SIZE];
    if (pDst == NULL)
    {
        fprintf(stderr, "out of memory\n");
        return "";
    }
    // read FLV file header;
    pSrc += 192;
    int decoded_data_size = 0;
    std::string str;
    int i = 0;
    while (pSrc < ((uint8_t*)inputStr.c_str() + src_len - 16)) 
    {
        // previous tag size
        pSrc += 4;
        // sign
        pSrc++;
        // get the tag audio size;
        int packet_size = 0x0|(int)(*pSrc)<<24|(int)(*(pSrc+1))<<16|*(pSrc+2);
        pSrc += 11;
        avpkt.data = pSrc;
        avpkt.size = packet_size - 1;

        int data_size = AVCODEC_MAX_AUDIO_FRAME_SIZE * 2;
        int size = avpkt.size;
        int decoded = 0;
        while(size > 0) 
        {
            int len = avcodec_decode_audio3(pInCodecContext, (int16_t *) pDst, &data_size, &avpkt);
            if (len >0)
            {
                str.append((char*)pDst, data_size);
                //pDst += data_size;
                //decoded_data_size += data_size;
            }
            size -= len;
            decoded += len;
        }
        // a packet is decoded
        pSrc += packet_size - 1;
    }
    delete[] pDst;
    avcodec_close(pInCodecContext);
    av_free(pInCodecContext);

    return str;
}

static std::string resample(std::string inputStr)
{
    std::string str;
    struct AVResampleContext* ctx = av_resample_init(OUT_SAMPLE_RATE, IN_SAMPLE_RATE, 16, 10, 1, 1.0);
    int consumed = 0;
    int i = 0;
    int out_size = 0;
    int src_len = inputStr.size();
    uint8_t *pDst = new uint8_t[BUFFER_SIZE];
    uint8_t *src = (uint8_t*)inputStr.c_str();
    for (i = 0; i < src_len/FRAME_SIZE; i++)
    {
        out_size = av_resample(ctx, (int16_t*)pDst, (int16_t*)(src + i * FRAME_SIZE), &consumed, FRAME_SIZE, BUFFER_SIZE, 0);
        str.append((char*)pDst, out_size);
    }
    av_resample_close(ctx);
    delete[] pDst;
    return str;
}

static std::string encode_flv(std::string inputStr)
{
    AVCodec *pOutCodec;
    AVCodecContext *pOutCodecContext= NULL;
    int frame_size, i = 0, outbuf_size=0;
    /* find the MP2 encoder */
    pOutCodec = avcodec_find_encoder(AV_CODEC_ID_SPEEX);
    if (!pOutCodec) 
    {
        fprintf(stderr, "codec not found\n");
        return "";
    }

    pOutCodecContext = avcodec_alloc_context3(pOutCodec);
    /* put sample parameters */
    pOutCodecContext->bit_rate = 32000;
    pOutCodecContext->sample_rate = 16000;
    pOutCodecContext->channels = OUT_CHANNELS;
    pOutCodecContext->sample_fmt = AV_SAMPLE_FMT_S16;

    /* open it */
    if (avcodec_open2(pOutCodecContext, pOutCodec, NULL) < 0) 
    {
        av_free(pOutCodecContext);
        fprintf(stderr, "could not open codec\n");
        return "";
    }
    frame_size = pOutCodecContext->frame_size;
    short *samples = new short[frame_size*pOutCodecContext->channels];
    short *pSrc = (short*)inputStr.c_str();
    int src_len = inputStr.size();
    uint8_t * pDst = new uint8_t[BUFFER_SIZE];
	int frame_index = 0;
    int out_size = 0;
    std::string str;
    Flv* flv = new Flv();
    flv->init(0);
    for(frame_index = 0; frame_index < (src_len/(frame_size*2) + 1); frame_index++)
    {
        memset((char*)samples, 0, frame_size*2*pOutCodecContext->channels);
        for(i = 0; i < frame_size && (i + frame_index*frame_size) < src_len/2; i++) 
        {
            samples[i] = pSrc[i + frame_index*frame_size];
            //samples[2*i] = pSrc[i + frame_index*frame_size];
            //samples[2*i+1] = samples[2*i];
        }
        out_size = avcodec_encode_audio(pOutCodecContext, pDst, BUFFER_SIZE, samples);
        AudioTag* newAudio = new AudioTag;
        newAudio->init(out_size, 20 * (frame_index), (char*)pDst);
        flv->addAudio(newAudio);
        //str.append((char*)pDst, out_size);
    }
    unsigned int flvbuffer_size = flv->getSize(); // the buffer size
    char *flvbuff = new char[flvbuffer_size];
    flv->dump(flvbuff, flvbuffer_size); 
    str.append(flvbuff, flvbuffer_size);

    delete flvbuff;
    delete flv;
    delete[] samples;
    delete[] pDst;
    avcodec_close(pOutCodecContext);
    av_free(pOutCodecContext);
    return str;
}

static std::string encode_mp3(std::string inputStr)
{
    AVCodec *pOutCodec;
    AVCodecContext *pOutCodecContext= NULL;
    int frame_size, i = 0, outbuf_size=0;
    /* find the MP2 encoder */
    pOutCodec = avcodec_find_encoder(AV_CODEC_ID_MP2);
    if (!pOutCodec) 
    {
        fprintf(stderr, "codec not found\n");
        return "";
    }

    pOutCodecContext = avcodec_alloc_context3(pOutCodec);
    /* put sample parameters */
    pOutCodecContext->bit_rate = OUT_BIT_RATE;
    pOutCodecContext->sample_rate = OUT_SAMPLE_RATE;
    pOutCodecContext->channels = OUT_CHANNELS;
    pOutCodecContext->sample_fmt = AV_SAMPLE_FMT_S16;

    /* open it */
    if (avcodec_open2(pOutCodecContext, pOutCodec, NULL) < 0) 
    {
        av_free(pOutCodecContext);
        fprintf(stderr, "could not open codec\n");
        return "";
    }
    frame_size = pOutCodecContext->frame_size;
    short *samples = new short[frame_size*pOutCodecContext->channels];
    short *pSrc = (short*)inputStr.c_str();
    int src_len = inputStr.size();
    uint8_t * pDst = new uint8_t[BUFFER_SIZE];
	int frame_index = 0;
    int out_size = 0;
    std::string str;
    for(frame_index = 0; frame_index < (src_len/(frame_size*2) + 1); frame_index++)
    {
        memset((char*)samples, 0, frame_size*2*pOutCodecContext->channels);
        for(i = 0; i < frame_size && (i + frame_index*frame_size) < src_len/2; i++) 
        {
            samples[i] = pSrc[i + frame_index*frame_size];
            //samples[2*i] = pSrc[i + frame_index*frame_size];
            //samples[2*i+1] = samples[2*i];
        }
        out_size = avcodec_encode_audio(pOutCodecContext, pDst, BUFFER_SIZE, samples);
        str.append((char*)pDst, out_size);
    }

    delete[] samples;
    delete[] pDst;
    avcodec_close(pOutCodecContext);
    av_free(pOutCodecContext);
    return str;
}
/*
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
}*/
char* raw2flv(char* src, int src_len, int* dst_len)
{
    avcodec_register_all();
    char *dst = NULL;
    std::string inputStr;
    inputStr.append((char*)src, src_len);
    std::string outputStr = encode_flv(inputStr);
    *dst_len = outputStr.size();
    if(*dst_len == 0)
        return NULL;

    dst = new char[*dst_len];
    memcpy(dst, outputStr.c_str(), *dst_len);

    return dst;
}

char* flv2mp3(char* src, int src_len, int* dst_len)
{
    avcodec_register_all();
    char *dst = NULL;
    std::string inputStr;
    inputStr.append((char*)src, src_len);
    std::string sampleStr = decode_flv(inputStr);
    if (sampleStr.size() == 0)
        return dst;
    //write_file("d:\\test\\test.raw", sampleStr.c_str(), sampleStr.size());

    std::string resampleStr = resample(sampleStr);
    if (resampleStr.size() == 0)
        return dst;
    //write_file("d:\\test\\resampe.raw", resampleStr.c_str(), resampleStr.size());

    std::string mp3Str = encode_mp3(resampleStr);
    //write_file("d:\\test\\test.mp3", (uint8_t*)mp3Str.c_str(), mp3Str.size());
    *dst_len = mp3Str.size();
    if (*dst_len == 0 )
        return dst;
    
    dst = new char[*dst_len];
    memcpy(dst, mp3Str.c_str(), *dst_len);
    return dst;
}

char* multiflv2mp3(char* src, int* src_len, int src_size, int* dst_len)
{
    avcodec_register_all();

    char *dst = NULL;
    char *pSrc = src;
    int index = 0;
    std::string totalResampleStr;
    for(index = 0; index < src_size; index++)
    {
        std::string inputStr;
        inputStr.append(pSrc, src_len[index]);
        std::string sampleStr = decode_flv(inputStr);
        std::string resampleStr = resample(sampleStr);
        totalResampleStr.append(resampleStr.c_str(), resampleStr.size());
        pSrc += src_len[index];
    }

    std::string mp3Str = encode_mp3(totalResampleStr);
    *dst_len = mp3Str.size();
    if (*dst_len == 0 )
        return dst;
    
    dst = new char[*dst_len];
    memcpy(dst, mp3Str.c_str(), *dst_len);
    return dst;
}

/*int main()
{
    uint8_t *buf;
    int file_size = open_inputfile("d:\\test\\20.flv", &buf);
    int len;
    char *p = flv2mp3((char*)buf, file_size, &len);
    free(p);
    delete buf;
    return  0;
}
*/