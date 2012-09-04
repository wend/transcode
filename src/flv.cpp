/**
 * @file
 * @author Allen <btian@scilearn.com>
 * @version 0.1
 * @section LICENSE
 *
 * Copyright 1997 - 2011 Scientific Learning Corporation.
 *
 * All rights reserved.  Redistribution or copying of this source code is prohibited.
 * The binary code may be distributed only under and pursuant to the terms of a
 * license agreement agreed to by Scientific Learning Corporation or one of its
 * authorized distributors.
 *
 * @section DESCRIPTION
 *
 * Flash Video file header creater
 *
 */
#include "flv.h"
#include <string.h>
//#include <sstream>

#define FLVHEADSIZE 9
#define TAGHEADSIZE 11
//#define METADATASIZE 9

//define the type of flv head
#define FLVHEADAUDIOTYPE 4
#define FLVHEADVIDEOTYPE 1
#define FLVHEADTYPEALL   5

//define the type of script data
#define AUDIODATARATE       0
#define AUDIOSAMPLERATE     16000.0
#define AUDIOSAMPLESIZE     16.0
#define STEREO              false
#define AUDIOCODECID        11.0//speex

//Hard code the metadata size now
#define METADATATAGSIZE     168
#define METADATATAGTIMEPOS  0

#define METASIGN            0x12
#define AUDIOSIGN           0x08
#define VIDEOSIGN           0x09

//#define AUDIOTAGSIGN        0xB2
#define PREVIEWTAGTYPESIZE  4


//typedef short int sampleType;

/** Convert little endian */
static inline int le_int(int i)
{
    int ui, ret;
    ui = i;
    ret =  ui>>24;
    ret |= (ui>>8)&0x0000ff00;
    ret |= (ui<<8)&0x00ff0000;
    ret |= (ui<<24);
    return ret;
}

/** Convert little endian */
static inline double le_double(double d)
{
    long long* du = (long long*)(&d);
    double dInit = 0;
    long long* dRet = (long long*)(&dInit);
    (*dRet) =  (*du)>>56;
    (*dRet) |= ((*du)>>40)&   0x000000000000ff00;
    (*dRet) |= ((*du)>>24)&   0x0000000000ff0000;
    (*dRet) |= ((*du)>>8)&    0x00000000ff000000;
    (*dRet) |= ((*du)<<8)&    0x000000ff00000000ULL;
    (*dRet) |= ((*du)<<24)&   0x0000ff0000000000ULL;
    (*dRet) |= ((*du)<<40)&   0x00ff000000000000ULL;
    (*dRet) |= ((*du)<<56);

    return *((double*)dRet);
}

/** Convert int to char[3](like int:24)  */
static inline bool _itoChar3(int i, char* outChar)
{
    int ui, ret;
    ret =0;
    ui = i;
    ret =  (ui>>24)&0x000000ff;
    outChar[2]  = (char)((ui&0x000000ff));
    outChar[1]  = (char)((ui>>8)&0x000000ff);
    outChar[0]  = (char)((ui>>16)&0x000000ff);
    //ret |= (ui<<8)&0x00ff0000;
    //ret |= (ui<<24);
    return true;
}

/** Convert int to char[3](like int:24)  */
static inline int _char3toi(char* inChar)
{
    int i = 0;

    i=(unsigned char)(inChar[2]);
    i=i|(unsigned char)(inChar[1])<<8;
    i=i|(unsigned char)(inChar[0])<<16;

    return i;
}

_flvHead::_flvHead()
{
    memset(this,0,sizeof(_flvHead));
    mark[0]     = 'F';
    mark[1]     = 'L';
    mark[2]     = 'V';
    version     = 1;
    headSize    = le_int(9);
}

_tagHead::_tagHead()
{
    //Should not use *this pointer here
    //however since all the members were
    //clearly, we can temporarily use this
    //to make things simple
    memset(this,0,sizeof(_tagHead));
    //I'm quite sure we do not need this expend flag now
    timeExpendFlag  = 0;
    streamId[0]        = 0;
    streamId[0]        = 0;
    streamId[0]        = 0;
}

//When ust int24 to set we mast make sure
//the input is not over bound
void _tagHead::setDataSize_24(int input)
{
    _itoChar3(input,dataSize);
}

void _tagHead::setTimePos_24(int input)
{
    _itoChar3(input,timePos);
}

void _tagHead::setSign(char inputSign)
{
    sign = inputSign;
}



_metaDataAMF1::_metaDataAMF1()
{
    type = 0x02;
    lengthOfMarkHigher = 0x00;
    lengthOfMark = 0x0A;
    mark[0] = 'o';
    mark[1] = 'n';
    mark[2] = 'M';
    mark[3] = 'e';
    mark[4] = 't';
    mark[5] = 'a';
    mark[6] = 'D';
    mark[7] = 'a';
    mark[8] = 't';
    mark[9] = 'a';
}

ParamDuration::ParamDuration()
{
    nameLengthHeight    = 0x00;
    nameLength          = 0x08;
    name[0]     = 'd';
    name[1]     = 'u';
    name[2]     = 'r';
    name[3]     = 'a';
    name[4]     = 't';
    name[5]     = 'i';
    name[6]     = 'o';
    name[7]     = 'n';
    valueType           = 0x00;
}

ParamAudiodatarate::ParamAudiodatarate()
{
    nameLengthHeight    = 0x00;
    nameLength          = 0x0D;
    valueType           = 0x00;
    name[0]     = 'a';
    name[1]     = 'u';
    name[2]     = 'd';
    name[3]     = 'i';
    name[4]     = 'o';
    name[5]     = 'd';
    name[6]     = 'a';
    name[7]     = 't';
    name[8]     = 'a';
    name[9]     = 'r';
    name[10]    = 'a';
    name[11]    = 't';
    name[12]    = 'e';
    audiodatarate       = le_double(AUDIODATARATE);
}

ParamAudiosamplerate::ParamAudiosamplerate()
{
    nameLengthHeight    = 0x00;
    nameLength          = 0x0F;
    valueType           = 0x00;
    name[0]     = 'a';
    name[1]     = 'u';
    name[2]     = 'd';
    name[3]     = 'i';
    name[4]     = 'o';
    name[5]     = 's';
    name[6]     = 'a';
    name[7]     = 'm';
    name[8]     = 'p';
    name[9]     = 'l';
    name[10]    = 'e';
    name[11]    = 'r';
    name[12]    = 'a';
    name[13]    = 't';
    name[14]    = 'e';
    audiosamplerate     = le_double(AUDIOSAMPLERATE);
}

ParamAudiosamplesize::ParamAudiosamplesize()
{
    nameLengthHeight    = 0x00;
    nameLength          = 0x0F;
    valueType           = 0x00;
    name[0]     = 'a';
    name[1]     = 'u';
    name[2]     = 'd';
    name[3]     = 'i';
    name[4]     = 'o';
    name[5]     = 's';
    name[6]     = 'a';
    name[7]     = 'm';
    name[8]     = 'p';
    name[9]     = 'l';
    name[10]    = 'e';
    name[11]    = 's';
    name[12]    = 'i';
    name[13]    = 'z';
    name[14]    = 'e';
    audiosamplesize     = le_double(AUDIOSAMPLESIZE);
}

ParamStereo::ParamStereo()
{
    nameLengthHeight    = 0x00;
    nameLength          = 0x06;
    valueType           = 0x01;
    name[0]     = 's';
    name[1]     = 't';
    name[2]     = 'e';
    name[3]     = 'r';
    name[4]     = 'e';
    name[5]     = 'o';
    stero               = STEREO;
}

ParamAudiocodecid::ParamAudiocodecid()
{
    nameLengthHeight    = 0x00;
    nameLength          = 0x0C;
    valueType           = 0x00;
    name[0]     = 'a';
    name[1]     = 'u';
    name[2]     = 'd';
    name[3]     = 'i';
    name[4]     = 'o';
    name[5]     = 'c';
    name[6]     = 'o';
    name[7]     = 'd';
    name[8]     = 'e';
    name[9]     = 'c';
    name[10]    = 'i';
    name[11]    = 'd';
    audiocodecid     = le_double(AUDIOCODECID);
}

ParamFilesize::ParamFilesize()
{
    nameLengthHeight    = 0x00;
    nameLength          = 0x08;
    valueType           = 0x00;
    name[0]     = 'f';
    name[1]     = 'i';
    name[2]     = 'l';
    name[3]     = 'e';
    name[4]     = 's';
    name[5]     = 'i';
    name[6]     = 'z';
    name[7]     = 'e';
}

_metaDataAMF2::_metaDataAMF2()
{
    type            = 0x08;
    arrayNumber     = le_int(7);

    endFlag[0]  = 0x00;
    endFlag[1]  = 0x00;
    endFlag[2]  = 0x09;
}

void MetaTag::init(double duration,double fileSize)
{
    metaTagHead.setDataSize_24(METADATATAGSIZE);
    metaTagHead.setTimePos_24(METADATATAGTIMEPOS);
    metaTagHead.setSign(METASIGN);

    metaData.amf2.duration.duration  = le_double(duration);
    metaData.amf2.fileSize.filesize  = le_double(fileSize);
}

int MetaTag::getSize()
{
    return METADATATAGSIZE + TAGHEADSIZE;
}



AudioData::AudioData()
{
    sign = 0xB2;
    data = NULL;
    dataSize    = 0;
}

AudioData::~AudioData()
{
    delete []data;
    data = NULL;
}

void AudioData::setData(char* input,int size)
{
    data = new char[size];
    memcpy(data,input,size);
    dataSize = size;
}

int AudioData::getSize()
{
    return dataSize +1/*sing*/;
}

void AudioTag::init(int datasize,int timePos, char* data)
{
    audioData.setData(data,datasize);
    audioTagHead.setDataSize_24(audioData.getSize());
    audioTagHead.setTimePos_24(timePos);
    audioTagHead.setSign(AUDIOSIGN);
}


int AudioTag::getSize()
{
    int ret = TAGHEADSIZE + audioData.getSize();
    return ret;
}

Flv::Flv()
{
    flvHead = new FLVHEADFORMAT;
    flvHead->sign = 0x04;
    metaTag = new MetaTag;
    audioList = new AudioList();
}

Flv::~Flv()
{
    if(flvHead)
    {
        delete flvHead;
        flvHead = 0;
    }

    if(metaTag)
    {
        delete metaTag;
        metaTag = 0;
    }

    if(audioList)
    {
        AudioListIter iter;
        for(iter = audioList->begin();iter != audioList->end();iter++)
        {
            //if(*iter)
            {
                delete *iter;
                //*iter = 0;
            }
        }
    }
	audioList->clear();
	delete audioList;
	audioList = NULL;
}

void Flv::init(double duration)
{
    metaTag->init(duration,getSize());
}

//Make sure the audio has been initialized
void Flv::addAudio(AudioTag* audio)
{
    audioList->push_back(audio);
    //update the data
    metaTag->metaData.amf2.duration.duration = le_double((double)(_char3toi(audio->audioTagHead.timePos))/1000.0);
    metaTag->metaData.amf2.fileSize.filesize += le_double((double)(audio->audioData.getSize()+ TAGHEADSIZE));
}

void Flv::reset()
{
    if(audioList)
    {
        AudioListIter iter;
        for(iter = audioList->begin();iter != audioList->end();iter++)
        {
            //if(*iter)
            {
                delete *iter;
                //*iter = 0;
            }
        }
        audioList->clear();

        metaTag->metaData.amf2.duration.duration = 0;
        metaTag->metaData.amf2.fileSize.filesize = 0;
    }
}

//Returen the size of this flv
int Flv::getSize()
{
    int size = metaTag->getSize() + FLVHEADSIZE;
    size += PREVIEWTAGTYPESIZE  * 2;//
    if(audioList)
    {
        AudioListIter iter;
        for(iter = audioList->begin();iter != audioList->end();iter++)
        {
            size += (*iter)->getSize();
            size += PREVIEWTAGTYPESIZE;
        }
    }
    return size;
}

void Flv::dump(char* outBuff, int size)
{
    int offset = 0;
    char* buffIter = outBuff;
    //Add flv head to buff
    char* curItor = (char*)flvHead;
    offset += FLVHEADSIZE;
    if(offset < size)
    {
        memcpy(buffIter,curItor,FLVHEADSIZE);
        buffIter += FLVHEADSIZE;
    }
    else
    {
        return;
    }

    //Previous Tag Size#0
    offset += 4;
    if(offset < size)
    {
        int PreviousSize = 0;
        int le =  le_int(PreviousSize);
        char* input = (char*)(&le);
        memcpy(buffIter,input,4);
        buffIter += 4;
    }
    else
    {
        return;
    }

    //Add meta Tag to buff
    curItor = (char*)metaTag;
    offset += METADATATAGSIZE + TAGHEADSIZE;
    if(offset < size)
    {
        memcpy(buffIter,curItor,METADATATAGSIZE + TAGHEADSIZE);
        buffIter += METADATATAGSIZE + TAGHEADSIZE;
    }
    else
    {
        return;
    }

    //Previous Tag Size
    offset += 4;
    if(offset < size)
    {
        int PreviousSize = METADATATAGSIZE + TAGHEADSIZE;
        int le =  le_int(PreviousSize);
        char* input = (char*)(&le);
        memcpy(buffIter,input,4);
        buffIter += 4;
    }
    else
    {
        return;
    }

    if(audioList)
    {
        AudioListIter iter;
        for(iter = audioList->begin();iter != audioList->end();iter++)
        {
            if(*iter)
            {
                AudioTag* curTag = *iter;
                //dump audio head
                curItor=(char*)(&(curTag->audioTagHead));
                memcpy(buffIter,curItor,TAGHEADSIZE);
                offset += TAGHEADSIZE;
                if(offset < size)
                {
                    buffIter += TAGHEADSIZE;
                }
                else
                {
                    return;
                }

                //dump audio data
                offset++;
                if(offset < size)
                {
                    *buffIter = curTag->audioData.sign;
                    buffIter++;
                }
                else
                {
                    return;
                }

                offset += curTag->audioData.dataSize;
                if(offset < size)
                {
                    memcpy(buffIter,curTag->audioData.data,curTag->audioData.dataSize);
                    buffIter += curTag->audioData.dataSize;
                }
                else
                {
                    return;
                }

                //Previous Tag Size#0
                offset += 4;
                if(offset <= size)
                {
                    int PreviousSize = curTag->audioData.getSize() + TAGHEADSIZE;
                    int le =  le_int(PreviousSize);
                    char* input = (char*)(&le);
                    memcpy(buffIter,input,4);
                    buffIter += 4;
                }
                else
                {
                    return;
                }
            }
        }
    }
}


//void createFlv(const char* inputBuff, const wchar_t* outPutFlvName)
//{
//    //The input is raw data
//    std::fstream fout;
//    fout.open(outPutFlvName,ios::binary|ios::in);
//    //do some test
//    char test1[3] = {0x00,0x01,0x23};
//    int ab = _char3toi(test1);
//
//    //Example
//    Flv* test = new Flv;
//    test->init(20);
//
//    //When we try to add audio
//    for(int i = 0; i< 26;i++)
//    {
//        char* data = new char[70];
//        memset(data,0,70);
//        AudioTag* newAudio = new AudioTag;
//        newAudio->init(70,20*i,data);
//        test->addAudio(newAudio);
//    }
//
//
//    char* out = new char[test->getSize()];
//    test->dump(out,test->getSize());
//
//
//}
//
//int _tmain(int argc, _TCHAR* argv[])
//{
//    //The default arg is the exe loaction
//    //If we input four args
//    //the second indicate be the input is raw file
//    //or wav file
//    //If there were only three args
//    //the second arg is the raw input(by default)
//    //The last arg will be the output file name
//    const wchar_t* inputFileName = NULL;
//    const wchar_t* outputFileName = NULL;
//    wchar_t* test = NULL;
//
//    if (argc == 3)
//    {
//        inputFileName   = argv[1];
//        outputFileName  = argv[2];
//    }
//    std::fstream fin;
//    fin.open(inputFileName,ios::binary|ios::out|ios::_Nocreate);
//    fin.seekg(0, ios::end);      //to the end
//    streampos ps = fin.tellg();  //get size of file
//    int fileSize = ps.seekpos();
//
//    //Read the src buff
//    char* inputBuff = new char[fileSize];
//    fin.seekg(0, ios::beg);
//    fin.read(inputBuff,fileSize);
//    //get the raw data now
//
//    createFlv(inputBuff,outputFileName);
//
//    delete inputBuff;
//    fin.close();
//    return 0;
//}

