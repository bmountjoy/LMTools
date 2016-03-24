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

// Assume # of variable length headers is 0
class MyLASLib {
public:
    LASHeader header;
    LASPoint *point;

private:
    static const unsigned long long CHUNK_SIZE = 100;
    unsigned long long chunkIndex;
    LASPoint chunk [CHUNK_SIZE];

    FILE *fp;

    void readNextChunk() {
        chunkIndex = 0;
        fread(&chunk, header.point_data_record_length, CHUNK_SIZE, fp);
    }

    void init() {
        fread(&header, sizeof(LASHeader), 1, fp);
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

    // void fuck();
    bool readPoint(void) {
        if (chunkIndex == CHUNK_SIZE) {
            readNextChunk();
        }
        if (chunkIndex == header.number_of_point_records) {
            return false;
        }
        point = &chunk[chunkIndex];
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
