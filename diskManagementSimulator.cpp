/*ex7_final_proj.2021 - disk management simulator
 * Aviv eldad - 311551477
 * The program simulate disk management, by index allocation method.
 * The disk divided for blocks in block_size size, and for each file the program define a management block,
 * and write its information to blocks on the disk
 * The program manages the information about the files in the directory*/
#include <iostream>

#include <string.h>

#include <vector>

#include <assert.h>

#include <fcntl.h>

#include <math.h>

#include <map>

#define DISK_SIM_FILE "DISK_SIM_FILE.txt"
#define DISK_SIZE 256

using namespace std;

// ============================================================================
//The class present a file in the system
class FsFile {
  int file_size;
  int block_in_use;
  int index_block;
  int block_size;

  public:
    FsFile(int _block_size) {

      file_size = 0;

      block_in_use = 0;

      block_size = _block_size;

      index_block = -1;

      file_size = 0;

    }

  int getfile_size() {

    return file_size;

  }

  int getBlockInUse() {
    return block_in_use;
  }

  int getIndexBlock() {
    return index_block;
  }

  void set_index_block(int available_block) {
    index_block = available_block;
  }

  void setBlockInUse(int _block_in_use) {
    block_in_use += _block_in_use;
  }

  void setFileSize(int size) {
    file_size += size;
  }

};

// ============================================================================
//The class present a file descriptor in the system
class FileDescriptor {

  string file_name;
  FsFile * fs_file;
  bool inUse;

  public:
    FileDescriptor(string FileName, FsFile * fsi) {

      file_name = FileName;

      fs_file = fsi;

      inUse = true;

    }

  string getFileName() {

    return file_name;

  }

  bool getInUse() {
    return inUse;
  }

  void setInUse(bool _inUse) {
    inUse = _inUse;
  }

  FsFile * getFs_file() {
    return fs_file;
  }
};

// ============================================================================
//The class present the disk
class fsDisk {
  FILE * sim_disk_fd;

  bool is_formated;

  int BitVectorSize;

  int * BitVector;

  int direct_enteris;

  int block_size;

  int maxSize;

  int freeBlocks;

  vector < FileDescriptor * > MainDir;

  FileDescriptor ** OpenFileDescriptors;

  public:

    //constructor for fsDisk class
    fsDisk() {

      sim_disk_fd = fopen(DISK_SIM_FILE, "r+");
      assert(sim_disk_fd);
      for (int i = 0; i < DISK_SIZE; i++) {
        int ret_val = fseek(sim_disk_fd, i, SEEK_SET);
        ret_val = fwrite("\0", 1, 1, sim_disk_fd);
        assert(ret_val == 1);
      }
      fflush(sim_disk_fd);

      direct_enteris = 0;
      block_size = 0;
      is_formated = false;
      freeBlocks = 0;
      maxSize = 0;
    }

  // The function prints all the existing files on the disk, if they are in use,  and the disk content
  void listAll() {
      int i = 0;
      for (; i < (int) MainDir.size();) {
        cout << "index: " << i << ": FileName: " << MainDir[i] -> getFileName() << " , isInUse: " <<
          MainDir[i] -> getInUse() << endl;
        i++;
      }
      char bufy;

      cout << "Disk content: '";

      for (i = 0; i < DISK_SIZE; i++) {

        int ret_val = fseek(sim_disk_fd, i, SEEK_SET);

        ret_val = fread( & bufy, 1, 1, sim_disk_fd);

        cout << bufy;
      }
      cout << "'" << endl;
    }

    //destructor for fsDisk class
    ~fsDisk() {
      if (is_formated) {
        for (int i = 0; i < BitVectorSize; i++) {
          OpenFileDescriptors[i] = nullptr;
        }
        delete[] OpenFileDescriptors;

        for (size_t i = 0; i < MainDir.size(); i++) {
          delete MainDir[i] -> getFs_file();
          delete MainDir[i];
        }

        delete[] BitVector;
        fclose(sim_disk_fd);
      }
    }
  // The function format the disk - allocated all the data structures for the disk management and update the  fields
  void fsFormat(int blockSize) {
    if (is_formated) {
      for (int i = 0; i < BitVectorSize; i++) {
        OpenFileDescriptors[i] = nullptr;
      }
      delete[] OpenFileDescriptors;

      for (size_t i = 0; i < MainDir.size(); i++) {
        delete MainDir[i] -> getFs_file();
        delete MainDir[i];
      }
      MainDir.clear();
      delete[] BitVector;
      for (int i = 0; i < DISK_SIZE; i++) {

        int ret_val = fseek(sim_disk_fd, i, SEEK_SET);

        ret_val = fwrite("\0", 1, 1, sim_disk_fd);

        assert(ret_val == 1);

      }
      fflush(sim_disk_fd);
    }
    this -> block_size = blockSize;
    BitVectorSize = DISK_SIZE / block_size;
    this -> freeBlocks = BitVectorSize;
    this -> maxSize = block_size * block_size;
    BitVector = new int[BitVectorSize];
    for (int j = 0; j < BitVectorSize; j++) {
      BitVector[j] = 0;
    }
    OpenFileDescriptors = new FileDescriptor * [BitVectorSize];
    for (int i = 0; i < BitVectorSize; i++) {
      OpenFileDescriptors[i] = nullptr;
    }
    this -> is_formated = true;
  }

  // The function create a new file on the disk and opens it. The function return the index of the file in the ofd vector
  int CreateFile(string fileName) {
    if (!is_formated) {
      return -1;
    }
    int location = 0;
    for (; location < BitVectorSize; location++) {
      if (OpenFileDescriptors[location] == nullptr) {
        break;
      }
    }
    if (location == BitVectorSize) {
      return -1;
    }
    FsFile * nf = new FsFile(block_size);
    FileDescriptor * nfd = new FileDescriptor(fileName, nf);
    MainDir.push_back(nfd);
    OpenFileDescriptors[location] = nfd;
    return location;
  }

  // The function open an existing file, in case the file is not already open
  int OpenFile(string fileName) {
    if (!is_formated) {
      return -1;
    }
    int location, i;
    for (i = 0; i < (int) MainDir.size(); i++) {
      if (fileName == MainDir[i] -> getFileName()) {
        break;
      }
    }
    //file doesn't exist
    if (i == (int) MainDir.size()) {
      return -1;
    }
    //file already open
    if (MainDir[i] -> getInUse()) {
      return -1;
    }
    for (location = 0; location < BitVectorSize; location++) {
      if (OpenFileDescriptors[location] == nullptr) {
        break;
      }
    }
    if (location == BitVectorSize) {
      return -1;
    }
    MainDir[i] -> setInUse(true);
    OpenFileDescriptors[location] = MainDir[i];

    return location;
  }

  // The function close an open file, in case the file is not close
  string CloseFile(int fd) {
    if ((!is_formated) || (fd >= BitVectorSize) || (OpenFileDescriptors[fd] == nullptr)) {
      return "-1";
    }
    string file_name = OpenFileDescriptors[fd] -> getFileName();
    OpenFileDescriptors[fd] -> setInUse(false);
    OpenFileDescriptors[fd] = nullptr;
    return file_name;
  }

  // The function convert a number from its decimal presentation to char presentation
  char decToBinary(int n, char & c) {
    int binaryNum[8];
    int i = 0;
    while (n > 0) {
      binaryNum[i] = n % 2;
      n = n / 2;
      i++;
    }
    for (int j = i - 1; j >= 0; j--) {
      if (binaryNum[j] == 1)
        c = c | 1 u << j;
    }
    return c;
  }

  //The function read from the disk the block we want to useS
  int getBlock(int index) {
    char c;
    int block;
    if (fseek(sim_disk_fd, index, SEEK_SET) != 0) {
      return -1;
    }
    if (fread( & c, sizeof(char), 1, sim_disk_fd) != 1) {
      return -1;
    }
    block = (int) c;
    if (block < 0) {
      block += 256;
    }
    return block;
  }

  // The function write file information to the last block of a file in the disk
  int writeToLastBlock(int fd, int last_block, int chars_in_use, char * buf, int len, int written) {
    last_block -= 1;
    last_block = getBlock(last_block);
    if (last_block == -1) {
      return -1;
    }
    last_block = (last_block * block_size) + (chars_in_use % block_size);

    for (int i = 0; i < len; i++) {
      if (fseek(sim_disk_fd, last_block + i, SEEK_SET) != 0) {
        return -1;
      }
      if (fwrite(buf + i + written, sizeof(char), 1, sim_disk_fd) != 1) {
        return -1;
      }
    }
    OpenFileDescriptors[fd] -> getFs_file() -> setFileSize(len);
    return len;
  }

  // The function write file information to a new block in the disk

  int writeToNewBlock(int fd, int last_block, char * buf, int len, int written) {
    int location = 0;
    while (BitVector[location] != 0) {
      location++;
    }
    BitVector[location] = 1;
    freeBlocks--;

    //write to management block
    char new_block = 0;
    decToBinary(location, new_block);
    if (fseek(sim_disk_fd, last_block, SEEK_SET) != 0) {
      return -1;
    }
    if (fwrite( & new_block, sizeof(char), 1, sim_disk_fd) != 1) {
      return -1;
    }
    OpenFileDescriptors[fd] -> getFs_file() -> setBlockInUse(1);

    //write to disk
    for (int i = 0; i < len; i++) {
      if (fseek(sim_disk_fd, (location * block_size) + i, SEEK_SET) != 0) {
        return -1;
      }
      if (fwrite(buf + i + written, sizeof(char), 1, sim_disk_fd) != 1) {
        return -1;
      }
    }
    OpenFileDescriptors[fd] -> getFs_file() -> setFileSize(len);
    return len;
  }

  // The function write file information to the disk
  int WriteToFile(int fd, char * buf, int len) {
    if ((!is_formated) || (fd >= BitVectorSize) || (OpenFileDescriptors[fd] == nullptr)) {
      return -1;
    }
    int chars_in_use = OpenFileDescriptors[fd] -> getFs_file() -> getfile_size();
    int available_chars = block_size - (chars_in_use % block_size);
    int blocks_to_add = len / block_size;
    int remainder_chars = len % block_size;
    if (remainder_chars != 0) {
      blocks_to_add++;
    }
    if (remainder_chars > available_chars) {
      blocks_to_add--;
    }
    if ((freeBlocks == 0) || freeBlocks - blocks_to_add < 0) {
      return -1;
    }
    if (OpenFileDescriptors[fd] -> getFs_file() -> getBlockInUse() == 0) {
      if (freeBlocks - (blocks_to_add + 1) < 0) {
        return -1;
      }
      int location = 0;
      while (BitVector[location] != 0) {
        location++;
      }
      OpenFileDescriptors[fd] -> getFs_file() -> set_index_block(location);
      BitVector[location] = 1;
      freeBlocks--;
    }

    //check for place in file
    if ((maxSize - chars_in_use) < len) {
      return -1;
    }
    int last_block = (OpenFileDescriptors[fd] -> getFs_file() -> getIndexBlock() * block_size) +
      OpenFileDescriptors[fd] -> getFs_file() -> getBlockInUse();

    if (len <= available_chars) {
      if (chars_in_use % block_size != 0) {
        return writeToLastBlock(fd, last_block, chars_in_use, buf, len, 0);
      } else {
        if (freeBlocks == 0) {
          return -1;
        }
        return writeToNewBlock(fd, last_block, buf, len, 0);
      }
    }
    int written = 0;

    if (len == 4) {
      remainder_chars = 4;
    }

    if (available_chars != block_size) { //there is space in last block
      if (writeToLastBlock(fd, last_block, chars_in_use, buf, available_chars, written) == -1) {
        return -1;
      } else {
        remainder_chars = len - available_chars;
        written += available_chars;
      }
    }
    while (written < len) {
      if (len - written >= block_size) {
        if (writeToNewBlock(fd, last_block, buf, block_size, written) == -1) {
          return -1;
        } else {
          written += block_size;
          last_block++;
        }
      } else {
        if (remainder_chars != 0 && written != 0) {
          remainder_chars %= block_size;
          if (writeToNewBlock(fd, last_block, buf, remainder_chars, written) == -1) {
            return -1;
          } else {
            written += remainder_chars;
          }
        }
      }
    }
    return len;
  }

  // The function delete file from the disk and clear its information as well
  int DelFile(string FileName) {
    if (!is_formated) {
      return 0;
    }
    int fd = 0;
    size_t location;
    for (location = 0; location < (int) MainDir.size(); location++) {
      if (FileName == MainDir[location] -> getFileName()) {
        break;
      }
    }
    //file doesn't exist
    if (location == MainDir.size()) {
      return 0;
    }
    if (MainDir[location] -> getFs_file() -> getfile_size() != 0) {
      int block_to_erase, erased = 0, index = 0, remainder = MainDir[location] -> getFs_file() -> getfile_size();
      while (erased < MainDir[location] -> getFs_file() -> getfile_size()) {
        block_to_erase = getBlock((MainDir[location] -> getFs_file() -> getIndexBlock() * block_size) + index);
        if (block_to_erase == -1) {
          return 0;
        }
        if (remainder < block_size) {
          for (int j = 0; j < remainder; j++) {
            if (fseek(sim_disk_fd, block_to_erase * block_size + j, SEEK_SET) < 0) {
              return 0;
            }
            if (fwrite("\0", sizeof(char), 1, sim_disk_fd) != 1) {
              return 0;
            }
          }
          erased += remainder;
          index++;
        } else {
          for (int j = 0; j < block_size; j++) {
            if (fseek(sim_disk_fd, block_to_erase * block_size + j, SEEK_SET) != 0) {
              return 0;
            }
            if (fwrite("\0", sizeof(char), 1, sim_disk_fd) != 1) {
              return 0;
            }
          }
          erased += block_size;
          remainder -= block_size;
          index++;
        }
        BitVector[block_to_erase] = 0;
        freeBlocks++;
      }
      //delete management block
      index--;
      while (index >= 0) {
        block_to_erase = (MainDir[location] -> getFs_file() -> getIndexBlock() * block_size) + index;
        if (fseek(sim_disk_fd, block_to_erase, SEEK_SET) != 0) {
          return 0;
        }
        if (fwrite("\0", sizeof(char), 1, sim_disk_fd) != 1) {
          return 0;
        }
        index--;
      }
      BitVector[MainDir[location] -> getFs_file() -> getIndexBlock()] = 0;
      freeBlocks++;
    }
    //if the file is open
    if (MainDir[location] -> getInUse()) {
      for (; fd < BitVectorSize; fd++) {
        if (OpenFileDescriptors[fd] != nullptr) {
          if (OpenFileDescriptors[fd] -> getFileName() == FileName) {
            break;
          }
        }
      }
      OpenFileDescriptors[fd] = nullptr;
      MainDir[location] -> setInUse(false);
    }

    delete MainDir[location] -> getFs_file();
    delete MainDir[location];
    MainDir.erase(MainDir.begin() + location);

    return 1;
  }

  // The function read file information from the disk
  int ReadFromFile(int fd, char * buf, int len) {
    if ((!is_formated) || (fd >= BitVectorSize) || (OpenFileDescriptors[fd] == nullptr)) {
      return -1;
    }
    int chars_in_use = OpenFileDescriptors[fd] -> getFs_file() -> getfile_size();

    if (len > chars_in_use) {
      return -1;
    }
    int block_to_read = 0, is_read = 0, block_num, remainder = len;
    buf[0] = '\0';
    while (is_read < len) {
      block_num = getBlock((OpenFileDescriptors[fd] -> getFs_file() -> getIndexBlock() * block_size) + block_to_read);
      if (block_num == -1) {
        return -1;
      }
      if (remainder < block_size) {
        for (int i = 0; i < remainder; i++) {
          if (fseek(sim_disk_fd, block_num * block_size + i, SEEK_SET) < 0) {
            return -1;
          }
          if (fread( & buf[i + is_read], sizeof(char), 1, sim_disk_fd) != 1) {
            return -1;
          }
        }
        is_read += remainder;
        block_to_read++;
      } else {
        for (int i = 0; i < block_size; i++) {
          if (fseek(sim_disk_fd, block_num * block_size + i, SEEK_SET) < 0) {
            return -1;
          }
          if (fread( & buf[i + is_read], sizeof(char), 1, sim_disk_fd) != 1) {
            return -1;
          }
        }
        is_read += block_size;
        remainder -= block_size;
        block_to_read++;
      }
    }
    buf[len] = '\0';
    return is_read;
  }

};

// ============================================================================
int main() {
  int blockSize, written;
  string fileName;
  char str_to_write[DISK_SIZE];
  char str_to_read[DISK_SIZE];
  int size_to_read;
  int _fd;

  fsDisk * fs = new fsDisk();
  int cmd_;
  while (1) {
    cin >> cmd_;
    switch (cmd_) {
    case 0: // exit
      delete fs;
      exit(0);
      break;

    case 1: // list-file
      fs -> listAll();
      break;

    case 2: // format
      cin >> blockSize;
      fs -> fsFormat(blockSize);
      break;

    case 3: // creat-file
      cin >> fileName;
      _fd = fs -> CreateFile(fileName);
      cout << "CreateFile: " << fileName << " with File Descriptor #: " << _fd << endl;
      break;

    case 4: // open-file
      cin >> fileName;
      _fd = fs -> OpenFile(fileName);
      cout << "OpenFile: " << fileName << " with File Descriptor #: " << _fd << endl;
      break;

    case 5: // close-file
      cin >> _fd;
      fileName = fs -> CloseFile(_fd);
      cout << "CloseFile: " << fileName << " with File Descriptor #: " << _fd << endl;
      break;

    case 6: // write-file
      cin >> _fd;
      cin >> str_to_write;
      written = fs -> WriteToFile(_fd, str_to_write, strlen(str_to_write));
      cout << "Writed: " << written << " Char's into File Descriptor#: " << _fd << endl;
      break;

    case 7: // read-file
      cin >> _fd;
      cin >> size_to_read;
      fs -> ReadFromFile(_fd, str_to_read, size_to_read);
      cout << "ReadFromFile: " << str_to_read << endl;
      break;

    case 8: // delete file
      cin >> fileName;
      _fd = fs -> DelFile(fileName);
      cout << "DeletedFile: " << fileName << " with File Descriptor #: " << _fd << endl;
      break;
    default:
      break;
    }
  }

}
