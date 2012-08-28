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

#pragma once

#ifndef _FLV_H_
#define _FLV_H_

//Use this convertor to change the input raw data
//to flv file 
#include <vector>

//---------------Create the flv head----------
//--------------------------------------------
//{
//    //The head will be like this
//    //The mark, hard code

//    //The version always be 0x01 now

//    //The sign. 0x04 for audio 0x01 for video
//    //and 0x05 for both

//    //The six to nine byte indicate the size
//    //of head, in version 1 this shall always
//    //be 9

/*
FLV Head
    -------------------------
    |  Mark:    FLV         |
    |  Version: 1.0         |
    |  Sign:    audio 0x04  |
    |           video 0x01  |
    |           both  0x05  |
    |  HeadSize 9           |
    -------------------------
    */
//}
//__attribute__((packed))

#pragma pack(1)
typedef struct _flvHead 
{
    char    mark[3];
    char    version;
    char    sign;
    int     headSize;

    _flvHead();
} FLVHEADFORMAT;

//The audio tag head is 11 byte
//First byte is tag type: audio(0x08), video(0x09),
//and script data(0x12)
//The second to four is UI24 type and define the 
//size of tag data
//The five to seven is UI24 type and define the 
//time pos of this tag(ms), if in the first tag shall be 0
//The expend of time pos
//The 9-11 byte is the stream id,0
/*
FLV Head
    -------------------------
    | Tag type: audio  0x08 |
    |           video  0x09 |
    |           script 0x12 |
    | Tag size: UI24        |
    | Time pos: UI24        |
    | Time expend:  0       |
    | Stream id:    0       |
    -------------------------
    total size: 11 byte
*/

//We should define the tagsize and time pos outside
typedef struct _tagHead 
{
    char    sign;
    char    dataSize[3];
    char    timePos[3];
    char    timeExpendFlag;
    char    streamId[3];

    _tagHead();

    //When ust int24 to set we mast make sure
    //the input is not over bound
    void setDataSize_24(int input);

    void setTimePos_24(int input);

    void setSign(char inputSign);

} FLVTAGHEADFORMAT;

//No need to set any param in this amf
typedef struct _metaDataAMF1
{
    char        type;   //0x02 for string
    char        lengthOfMarkHigher;//always 0x00
    char        lengthOfMark; //mark length always be 0x0A
    char        mark[10]; //makr "onMetaData"

    _metaDataAMF1();
} FLVMETADATAAMF1FORMAT;

struct  ParamDuration
{
    //We should use a short int to storie
    //this but since we do not have a 
    //very long name so we can use just like 
    //this
    char    nameLengthHeight;
    char    nameLength;
    char    name[8];
    char    valueType; //0x00 for double
    double  duration;

    ParamDuration();
};

struct  ParamAudiodatarate
{
    char    nameLengthHeight;
    char    nameLength;
    char    name[13];
    char    valueType; //0x00 for double
    double  audiodatarate;

    ParamAudiodatarate();
};

struct  ParamAudiosamplerate
{
    char    nameLengthHeight;
    char    nameLength; 
    char    name[15];
    char    valueType; //0x00 for double
    double  audiosamplerate;

    ParamAudiosamplerate();
};

struct  ParamAudiosamplesize
{
    char    nameLengthHeight;
    char    nameLength; 
    char    name[15];
    char    valueType; //0x00 for double
    double  audiosamplesize;

    ParamAudiosamplesize();
};

struct  ParamStereo
{
    char    nameLengthHeight;
    char    nameLength; 
    char    name[6];
    char    valueType; //0x00 for double
    bool    stero;

    ParamStereo();
};

struct  ParamAudiocodecid
{
    char    nameLengthHeight;
    char    nameLength; 
    char    name[12];
    char    valueType; //0x00 for double
    double  audiocodecid;

    ParamAudiocodecid();
};

struct  ParamFilesize
{
    char    nameLengthHeight;
    char    nameLength; 
    char    name[8];
    char    valueType; //0x00 for double
    double  filesize;

    ParamFilesize();
};

typedef struct _metaDataAMF2
{
    char        type;   //0x08 for array
    int         arrayNumber; //We assume we have 7 params here 

    //The params here
    ParamDuration           duration;
    ParamAudiodatarate      audioDataRate;
    ParamAudiosamplerate    audioSmapleRate;
    ParamAudiosamplesize    audioSampleSize;
    ParamStereo             stereo;
    ParamAudiocodecid       audioCodecId;
    ParamFilesize           fileSize;

    char                endFlag[3];

    _metaDataAMF2();
} FLVMETADATAAMF2FORMAT;

typedef struct _metaDataStruct 
{
    FLVMETADATAAMF1FORMAT       amf1;
    FLVMETADATAAMF2FORMAT       amf2;
} FLVMETADATAFORMAT;

#pragma  pack()

struct MetaTag
{
    FLVTAGHEADFORMAT    metaTagHead;
    FLVMETADATAFORMAT   metaData;

    void init(double duration,double fileSize);

    int getSize();
};

struct AudioData
{
    char    sign;
    char*   data;
    int     dataSize;

    AudioData();
    ~AudioData();

    void setData(char* input,int size);
    int getSize();

};

struct AudioTag
{
    FLVTAGHEADFORMAT    audioTagHead;
    AudioData           audioData;

    void init(int datasize,int timePos, char* data);
    int getSize();
};


/*----------------
FILE Header
-------------------------
|                       |
|  Previous Tag Size#0  |
|                       |
|  Tag #1(MetaDataTag)  |
|                       |
|  Previous Tag Size#1  |
|                       |
|  Tag #2(First audio)  |   File Body
|                       |
|  Previous Tag Size#2  |
|   ...                 |
|   ...                 |
|                       |
|   Tag #N(Last audio)  |
|                       |
|  Previous Tag Size#N  |
|                       |
-------------------------
*///------------------------------------------
typedef std::vector<AudioTag*> AudioList;
typedef std::vector<AudioTag*>::iterator AudioListIter;


class Flv
{
public:
    Flv();
    ~Flv();

    void init(double duration);

    //Make sure the audio has been initialized
    void addAudio(AudioTag* audio);

    //Returen the size of this flv
    int getSize();
	void reset();
    //size is the buff size
    void dump(char* outBuff, int size);

protected:
private:
    FLVHEADFORMAT*       flvHead;
    MetaTag*                metaTag;
    //Store all the tags
    AudioList*              audioList;
};


#endif
