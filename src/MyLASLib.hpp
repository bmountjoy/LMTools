//
//  MyLASLib.h
//  
//
//  Created by HLRG on 2016-03-18.
//
//

#ifndef _MyLASLib_h
#define _MyLASLib_h

#include <stdlib.h>
#include <stdio.h>

#pragma pack(push, 1)
class LASPoint {
public:
    int X;
    int Y;
    int Z;
    unsigned short intensity;
    unsigned char return_number : 3;
    unsigned char number_of_returns : 3;
    unsigned char scan_direction_flag : 1;
    unsigned char edge_of_flight_line : 1;
    unsigned char classification : 5;
    unsigned char synthetic_flag : 1;
    unsigned char keypoint_flag  : 1;
    unsigned char withheld_flag  : 1;
    unsigned char scan_angle_rank;
    unsigned char user_data;
    unsigned short point_source_ID;
    double gps_time;
    unsigned short r;
    unsigned short g;
    unsigned short b;
};

class LASHeader {
public:
    char file_signature[4];                  // starts at byte   0
    unsigned short file_source_ID;                      // starts at byte   4
    unsigned short global_encoding;                     // starts at byte   6
    unsigned int project_ID_GUID_data_1;              // starts at byte   8
    unsigned short project_ID_GUID_data_2;              // starts at byte  12
    unsigned short project_ID_GUID_data_3;              // starts at byte  14
    unsigned char project_ID_GUID_data_4[8];            // starts at byte  16
    unsigned char version_major;                        // starts at byte  24
    unsigned char version_minor;                        // starts at byte  25
    char system_identifier[32];              // starts at byte  26
    char generating_software[32];            // starts at byte  58
    unsigned short file_creation_day;                   // starts at byte  90
    unsigned short file_creation_year;                  // starts at byte  92
    unsigned short header_size;                         // starts at byte  94
    unsigned int offset_to_point_data;                // starts at byte  96
    unsigned int number_of_variable_length_records;   // starts at byte 100
    unsigned char point_data_format;                    // starts at byte 104
    unsigned short point_data_record_length;            // starts at byte 105
    unsigned int number_of_point_records;             // starts at byte 107
    unsigned int number_of_points_by_return[5];       // starts at byte 111
    double x_scale;
    double y_scale;
    double z_scale;
    double x_offset;
    double y_offset;
    double z_offset;
    double max_x;
    double min_x;
    double max_y;
    double min_y;
    double max_z;
    double min_z;
    
    // LAS 1.3 only
    unsigned long long start_of_waveform_data_packet_record;
};
#pragma pack(pop)

// Assume # of variable length headers is 0
class MyLASLib {
public:
    LASHeader header;
    LASPoint *point;

private:
    static const unsigned long long CHUNK_SIZE = 1000;
    unsigned long long chunkIndex;
    unsigned long long chunkOffset;
    LASPoint chunk [CHUNK_SIZE];

    double _minx, _miny, _minz, _maxx, _maxy, _maxz;
    unsigned int _n_return_1, _n_return_2;

    FILE *fp;

    void readNextChunk() {
        if (chunkOffset != 0) {
            chunkIndex += CHUNK_SIZE;   
        }
        chunkOffset = 0;
        unsigned int nextChunk = ((header.number_of_point_records - chunkIndex) > CHUNK_SIZE)
            ? CHUNK_SIZE
            : header.number_of_point_records - chunkIndex;
        for (int i = 0; i < nextChunk; ++i) {
            fread(&chunk[i], header.point_data_record_length, 1, fp);
        }
    }

    void printHeader() {
        printf("header.offset_to_point_data: %d\n", header.offset_to_point_data);
        printf("header.number_of_point_records: %d\n", header.number_of_point_records);
        printf("header.header.point_data_format: %d\n", header.point_data_format);
        printf("header.point_data_record_length: %d==%d\n", header.point_data_record_length, sizeof(LASPoint));
        printf("size of header: %d\n", sizeof(LASHeader));
    }

    void initStats() {
        _minx = _miny = _minz = 9999999999.0;
        _maxx = _maxy = _maxz = 0.0;
        _n_return_1 = _n_return_2 = 0;
    }

    void updateStats() {
        _minx = (getPntX() < _minx) ? getPntX() : _minx;
        _miny = (getPntY() < _miny) ? getPntY() : _miny;
        _minz = (getPntZ() < _minz) ? getPntZ() : _minz;
        _maxx = (getPntX() > _maxx) ? getPntX() : _maxx;
        _maxy = (getPntY() > _maxy) ? getPntY() : _maxy;
        _maxz = (getPntZ() > _maxz) ? getPntZ() : _maxz;

        if (point->return_number == 1) {
            ++_n_return_1;
        }
        if (point->return_number == 2) {
            ++_n_return_2;
        }
    }

    void printStats() {
        printf("x: %lf -> %lf\n", _minx, _maxx);
        printf("y: %lf -> %lf\n", _miny, _maxy);
        printf("z: %lf -> %lf\n", _minz, _maxz);
        printf("1st returns: %d\n", _n_return_1);
        printf("2nd returns: %d\n", _n_return_2);
    }

    void init() {
        initStats();
        chunkIndex  = 0;
        chunkOffset = 0;
        fread(&header, sizeof(LASHeader), 1, fp);
        printHeader();
        fseek(fp, header.offset_to_point_data, SEEK_SET);
        readNextChunk();
    }

public:
    MyLASLib(const char *path) {
        fp = fopen(path, "rb");
        if (!fp) {
            exit(0);
        }
        init();
    }

    bool readPoint(void) {
        // Did we reach the end of the file?
        if ((chunkIndex + chunkOffset) == header.number_of_point_records) {
            printStats();
            return false;
        }
        if (chunkOffset == CHUNK_SIZE) {
            readNextChunk();
        }
        point = &chunk[chunkOffset++];
        if ((chunkIndex + chunkOffset) % 10000 == 0) {
            printf("reading point: %llu \n", chunkIndex + chunkOffset);
            printf("%lf,%lf,%lf\n", getPntX(), getPntY(), getPntZ());
            printf("%d,%d,%d\n", point->X, point->Y, point->Z);
        }

        updateStats();
        return true;
    }

    bool insideRectangle(const double min_x, const double max_x, const double min_y, const double max_y) {
        if (getPntX() >= min_x &&
            getPntX() <  max_x &&
            getPntY() >= min_y &&
            getPntY() <  max_y)
        {
            return true;
        }
        return false;
    }

    double getPntX() {
        return (header.x_scale * point->X) + header.x_offset;
    }

    double getPntY() {
        return (header.y_scale * point->Y) + header.y_offset;
    }

    double getPntZ() {
        return (header.z_scale * point->Z) + header.z_offset;
    }

    ~MyLASLib() {
        if (fp) {
            fclose(fp);
        }
    }
};

#endif
