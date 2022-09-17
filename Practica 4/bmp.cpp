#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

struct fileHeader{
  uint16_t ftype{0x4D42};
  uint32_t size{0};
  uint16_t resv1{0};
  uint16_t resv2{0};
  uint32_t offset{0};
};

struct infoHeader{
  uint32_t headerSize{0};
  uint32_t width{0};
  uint32_t height{0};
  uint16_t planos{1};
  uint16_t bpp{0};
  uint32_t compresion{0};
  uint32_t imgsize{0};
  uint32_t bpmx{0};
  uint32_t bpmy{0};
  uint32_t colores{0};
  uint32_t imxtcolores{0};
};

struct colorHeader{
  uint32_t red{0x00ff0000};
  uint32_t green{0x0000ff00};
  uint32_t blue{0x000000ff};
  uint32_t alpha{0xff000000};
  uint32_t spaceType{0x73524742};
  uint32_t unused[16]{0};
};

struct BMP{
private:
     // ...
 
     // Check if the pixel data is stored as BGRA and if the color space type is sRGB
     void check_cHead(colorHeader &cHead) {
         colorHeader expected_cHead;
         if(expected_cHead.red != cHead.red ||
             expected_cHead.blue != cHead.blue ||
             expected_cHead.green != cHead.green ||
             expected_cHead.alpha != cHead.alpha) {
             throw runtime_error("Unexpected color mask format! The program expects the pixel data to be in the BGRA format");
         }
         if(expected_cHead.spaceType != cHead.spaceType) {
             throw runtime_error("Unexpected color space type! The program expects sRGB values");
         }
     
  }
uint32_t row_stride{ 0 };
     void write_headers(std::ofstream &of) {
         of.write((const char*)&fHead, sizeof(fHead));
         of.write((const char*)&iHead, sizeof(iHead));
         if(iHead.bpp == 32) {
             of.write((const char*)&cHead, sizeof(cHead));
         }
     }
 
     void write_headers_and_data(std::ofstream &of) {
         write_headers(of);
         of.write((const char*)datos.data(), datos.size());
     }
 
     // Add 1 to the row_stride until it is divisible with align_stride
     uint32_t make_stride_aligned(uint32_t align_stride) {
         uint32_t new_stride = row_stride;
         while (new_stride % align_stride != 0) {
             new_stride++;
         }
         return new_stride;
     }
public:
  fileHeader fHead;
  infoHeader iHead;
  colorHeader cHead;

  vector<uint8_t> datos;

  BMP(string fname){
    read(fname);
  }
  void read(string fname){
    ifstream inp{fname, ios_base::binary};
      if (inp) {
             inp.read((char*)&fHead, sizeof(fHead));
             if(fHead.ftype != 0x4D42) {
                 throw runtime_error("Error! Unrecognized file format.");
             }
             inp.read((char*)&iHead, sizeof(iHead));
 
             // The colorHeader is used only for transparent images
             if(iHead.bpp == 32) {
                 // Check if the file has bit mask color information
                 if(iHead.headerSize >= (sizeof(infoHeader) + sizeof(colorHeader))) {
                     inp.read((char*)&cHead, sizeof(cHead));
                     // Check if the pixel data is stored as BGRA and if the color space type is sRGB
                     check_cHead(cHead);
                 } else {
                     cerr << "Warning! The file \"" << fname << "\" does not seem to contain bit mask information\n";
                     throw runtime_error("Error! Unrecognized file format.");
                 }
             }
 
             // Jump to the pixel data location
             inp.seekg(fHead.offset, inp.beg);
 
             // Adjust the header fields for output.
             // Some editors will put extra info in the image file, we only save the headers and the data.
             if(iHead.bpp == 32) {
                 iHead.headerSize = sizeof(infoHeader) + sizeof(colorHeader);
                 fHead.offset = sizeof(fileHeader) + sizeof(infoHeader) + sizeof(colorHeader);
             } else {
                 iHead.headerSize = sizeof(infoHeader);
                 fHead.offset = sizeof(fileHeader) + sizeof(infoHeader);
             }
             fHead.size = fHead.offset;
 
             if (iHead.height < 0) {
                 throw runtime_error("The program can treat only BMP images with the origin in the bottom left corner!");
             }
 
             datos.resize(iHead.width * iHead.height * iHead.bpp / 8);
 
             // Here we check if we need to take into account row padding
             if (iHead.width % 4 == 0) {
                 inp.read((char*)datos.data(), datos.size());
                 fHead.size += datos.size();
             }
             else {
                 row_stride = iHead.width * iHead.bpp / 8;
                 uint32_t new_stride = make_stride_aligned(4);
                 vector<uint8_t> padding_row(new_stride - row_stride);
 
                 for (int y = 0; y < iHead.height; ++y) {
                     inp.read((char*)(datos.data() + row_stride * y), row_stride);
                     inp.read((char*)padding_row.data(), padding_row.size());
                 }
                 fHead.size += datos.size() + iHead.height * padding_row.size();
             }
         }
         else {
             throw runtime_error("Unable to open the input image file.");
         }
     }
 
     // ...

  void write(string fname){
    ofstream of{fname,ios_base::binary};if (iHead.bpp == 32) {
          if(of){
                 write_headers_and_data(of);
             }
             else if (iHead.bpp == 24) {
                 if (iHead.width % 4 == 0) {
                     write_headers_and_data(of);
                 }
                 else {
                     uint32_t new_stride = make_stride_aligned(4);
                     vector<uint8_t> padding_row(new_stride - row_stride);
 
                     write_headers(of);
 
                     for (int y = 0; y < iHead.height; ++y) {
                         of.write((const char*)(datos.data() + row_stride * y), row_stride);
                         of.write((const char*)padding_row.data(), padding_row.size());
                     }
                 }
             }
             else {
                 throw runtime_error("The program can treat only 24 or 32 bits per pixel BMP files");
             }
         }
         else {
             throw runtime_error("Unable to open the output image file.");
         }
     }
};

int main() {
  BMP bmp("prueba.bmp");
  bmp.write("prueba2.bmp");
}
