
#include "MyLASLib.hpp"

// void MyLASLib::readNextChunk() {
//     chunkIndex = 0;
//     fread(&chunk, header.point_data_record_length, 1, CHUNK_SIZE);
// }

// void MyLASLib::init() {
//     fread(&header, sizeof(LASHeader), 1, fp);
//     fseek(fp, header.offset_to_point_data, SEEK_SET);
//     readNextChunk();
// }

// bool MyLASLib::readPoint(void) {
//     if (chunkIndex == CHUNK_SIZE) {
//         readNextChunk();
//     }
//     if (chunkIndex == header.number_of_point_records) {
//         return false;
//     }
//     point = &chunk[_chunkIndex];
//     return true;
// }

// bool MyLASLib::insideRectangle(const double min_x, const double max_x, const double min_y, const double max_y) {
//     if (getPntX() >= min_x &&
//         getPntX() <  max_x &&
//         getPntY() >= min_y &&
//         getPntY() <  max_y)
//     {
//         return true;
//     }
//     return false;
// }

// void MyLASLib::fuck() {
//     fseek(fp, header.offset_to_point_data, SEEK_SET);
//     readNextChunk();
// }

MyLASLib::MyLASLib() {
    fp = fopen(path, "rb");
    if (!fp) {
        exit(0);
    }
    init();
}