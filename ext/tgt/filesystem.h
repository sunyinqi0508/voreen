/**********************************************************************
 *                                                                    *
 * tgt - Tiny Graphics Toolbox                                        *
 *                                                                    *
 * Copyright (C) 2006-2008 Visualization and Computer Graphics Group, *
 * Department of Computer Science, University of Muenster, Germany.   *
 * <http://viscg.uni-muenster.de>                                     *
 *                                                                    *
 * This file is part of the tgt library. This library is free         *
 * software; you can redistribute it and/or modify it under the terms *
 * of the GNU Lesser General Public License version 2.1 as published  *
 * by the Free Software Foundation.                                   *
 *                                                                    *
 * This library is distributed in the hope that it will be useful,    *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the       *
 * GNU Lesser General Public License for more details.                *
 *                                                                    *
 * You should have received a copy of the GNU Lesser General Public   *
 * License in the file "LICENSE.txt" along with this library.         *
 * If not, see <http://www.gnu.org/licenses/>.                        *
 *                                                                    *
 **********************************************************************/

#ifndef TGT_FILESYSTEM_H
#define TGT_FILESYSTEM_H

#include "tgt/config.h"
#include "tgt/logmanager.h"

#include <map>
#include <string>
#include <fstream>

namespace tgt {

/**
 * Base class for input files.
 */
class File {
public:
    /**
     * Offset identifiers for seek(). Used exactly like
     * seekdir parameter in std::istream::seekg().
     */
    enum SeekDir {
        BEGIN,      ///< Offset is specified relatively to beginning of file
        CURRENT,    ///< Offset is specified relatively to current position in file
        END         ///< Offset is specified relatively to end of file
    };

    File();
    File(const std::string& name);
    virtual ~File();

    /// Closes the file.
    virtual void close() = 0;
    
    /// Returns the name of the file
    const std::string& getName(void) { return name_; }

    /// Returns the size of the file in bytes (0 if unknown)
    size_t size(void) const { return size_; }

    /// Read up to count bytes into buf. Returns the number of bytes read.
    virtual size_t read(void* buf, size_t count) = 0;
    /// Reads up to maxCount characters into buf. Stops at first occurence of delim.
    virtual size_t readLine(char* buf, size_t maxCount, char delim = '\n');
    /// Reads until first occurence of delim and returns a string
    virtual std::string getLine(char delim = '\n');
    /// Reads the whole file and puts it into a string.
    virtual std::string getAsString();
    /// Skips until after the first occurence of delim.
    virtual size_t skipLine(char delim = '\n');

    /// Skips count bytes (or to end of file, whatever comes first)
    virtual void skip(long count) = 0;
    /// Seeks to pos (relative to the begin of file)
    virtual void seek(size_t pos) = 0;
    /// Seeks to offset (relative to seekDir)
    virtual void seek(size_t offset, SeekDir seekDir) = 0;
    /// Returns the current reading position
    virtual size_t tell() = 0;

    /// Returns true if the End Of File is reached
    virtual bool eof() = 0;
    /// Returns true if the file is open.
    virtual bool isOpen() = 0;
    /// Check if the state of the file is good for i/o operations.
    virtual bool good() = 0;

protected:
    std::string name_;
    size_t size_;
};

/**
 * A file from the regular filesystem.
 */
class RegularFile : public File {
public:
    /// Open filename
    RegularFile(const std::string& filename);
    virtual ~RegularFile();

    virtual void close();

    virtual size_t read(void* buf, size_t count);

    virtual void skip(long count);
    virtual void seek(size_t pos);
    virtual void seek(size_t offset, File::SeekDir seekDir);
    virtual size_t tell();

    virtual bool eof();
    virtual bool isOpen();
    virtual bool good();

protected:
    std::ifstream file_;
	static const std::string loggerCat_;
};

/**
 * A virtual file, read from a chunk of memory
 */ 
class MemoryFile : public File {
public:
    /// Create memoryfile from data with given size and call it filename.
    /// If deleteData is true the memory file will delete the data upon destruction.
    MemoryFile(char* data, size_t size, const std::string& filename, bool deleteData = false);
    ~MemoryFile();

    virtual void close();

    virtual size_t read(void* buf, size_t count);

    virtual void skip(long count);
    virtual void seek(size_t pos);
    virtual void seek(size_t offset, File::SeekDir seekDir);
    virtual size_t tell();

    virtual bool eof();
    virtual bool isOpen();
    virtual bool good();

protected:
    char* data_;
    size_t pos_;
    bool deleteData_;
};

/**
 * A virtual file, part of a tar archive.
 */
class TarFile : public File {
public:
    /// Open the file at offset with size in the archive tarfilename and call it filename
    TarFile(const std::string& filename, const std::string& tarfilename, size_t offset, size_t size);
    ~TarFile();

    virtual void close();

    virtual size_t read(void* buf, size_t count);

    virtual void skip(long count);
    virtual void seek(size_t pos);
    virtual void seek(size_t offset, File::SeekDir seekDir);
    virtual size_t tell();

    virtual bool eof();
    virtual bool isOpen();
    virtual bool good();

protected:
    File* file_;
    size_t offset_;
	static const std::string loggerCat_;
};

//-----------------------------------------------------------------------------

/**
 * A FileFactory plugs into the virtual FS and creates File objects
 */
class FileFactory {
public:
    virtual ~FileFactory() {}
    /// Open the file filename from this factory
    virtual File* open(const std::string& filename) = 0;
    /// Returns all the filenames the factory can open.
    virtual std::vector<std::string> getFilenames() = 0;
};

/**
 * Creates instances of MemoryFile.
 * This factory always provides only one file.
 */
class MemoryFileFactory : public FileFactory {
public:
    /// Create factory which provides a file with name filename from the data in memory at data with size size
    MemoryFileFactory(const std::string& filename, char* data, size_t size);
    /// Create factory which provides a file with name filename from the data in the string data
    MemoryFileFactory(const std::string& filename, const std::string& data);
    virtual File* open(const std::string& filename);
    virtual std::vector<std::string> getFilenames();

protected:
    char* data_;
    size_t size_;
    std::string filename_;
};

/**
 * Reads content of a tar archive and creates TarFile objects for all files in it.
 */
class TarFileFactory : public FileFactory {
public:
    struct ArchivedFile {
        size_t size_;
        size_t offset_;
    };
    
    /**
     * Create Factory from tarfile filename.
     * Reads content of tarfile and save offsets and size for all contained files.
     * Files are added to the virtual FS relative to rootpath:
     * Name in VFS = rootpath + name in archive
     */
    TarFileFactory(const std::string& filename, const std::string& rootpath = "./");

    virtual File* open(const std::string& filename);
    virtual std::vector<std::string> getFilenames();

protected:
    std::string filename_;
    std::map<std::string, ArchivedFile> files_;
	static const std::string loggerCat_;
};

/**
 * Reads content of a zip archive and creates File objects for all files in it.
 * Needs TGT_HAS_ZLIB define to read from compressed zip files.
 * Supports uncompressed files and deflate compression.
 * Not supported: Encryption, multi volume archives, zip64 file format.
 */
class ZipFileFactory : public FileFactory {
public:
    struct ArchivedFile {
        size_t offset_;
    };
    
    /**
     * Create Factory from zipfile filename.
     * Reads content of zipfile and save offsets for all contained files.
     * Files are added to the virtual FS relative to rootpath:
     * Name in VFS = rootpath + name in archive
     */
    ZipFileFactory(const std::string& filename, const std::string& rootpath = "./");
    ~ZipFileFactory();

    /**
     * Reads file from zip to memory and creates+returns a Memoryfile
     */
    virtual File* open(const std::string& filename);
    virtual std::vector<std::string> getFilenames();

protected:
    static const std::string loggerCat_;

    File* file_;
    std::string filename_;
    std::map<std::string, ArchivedFile> files_;
};

//-----------------------------------------------------------------------------

/**
 * Provides transparent access to the filesystem overlayed with a virtual filesystem.
 */
class FileSystem {
public:
    FileSystem();
    ~FileSystem();

    /// Open a file, first checks for availability in virtual FS, then in regular FS.
    File* open(const std::string& filename);
    /// Checks wether a file with the specified filename exists.
    ///TODO: unimplemented
    bool exists(const std::string& filename);
    /// Adds a FileFactory and inserts all files provided by the factory to the virtual FS.
    /// All Factories are deleted upon destruction.
    void addFactory(FileFactory* ff);

    /// Creates a MemoryFileFactory and adds it (just for convenience)
    void addMemoryFile(const std::string& filename, char* data, size_t size);
    /// Creates a MemoryFileFactory and adds it (just for convenience)
    void addMemoryFile(const std::string& filename, const std::string& data);

    /// Creates a TarFileFactory and adds it (just for convenience)
    void addPackage(const std::string& filename, const std::string& rootpath = "./");

    //
    // Static methods for file system information
    // NOTE: These do not work with the virtual filesystem!
    //

    //
    // path information/manipulation
    //
    
    /**
     * Returns the canonicalized absolute pathname.
     */
    static std::string absolutePath(const std::string& path);

    /**
     * Returns a path relative to dir.
     */
    static std::string relativePath(const std::string& path, const std::string& dir);
    
    /**
     * Return the file name without the path component.
     */
    static std::string fileName(const std::string& filepath);

    /**
     * Return the full directory path without the file name component.
     */
    static std::string dirName(const std::string& filepath);
    
    /**
     * Return the file extension (suffix) from the path.
     * @param lowercase convert result to lower case
     */
    static std::string fileExtension(const std::string& path, bool lowercase = false);

    /**
     * Returns a string containing the current working directory.
     */
    static std::string currentDirectory();

    //
    // file system manipulation/information
    //   
    
    /**
     * Creates the directory of the give name if it does not already exists.
     * 
     * @param   directory   name for the new directory to be created
     * @return  true if the creation was succesful or false otherwise
     */
    static bool createDirectory(const std::string& directory);

    /**
     * Deletes the file with the given filename.
     *
     * @param   filename    name of the file to be deleted
     * @return  true if the file has been deleted successfully, false otherwise.
     */
    static bool deleteFile(const std::string& filename);

    /**
     * Determines whether a file of the given name exists or not.
     *
     * @param   filename   name of the file which is suspected to exist
     * @return  true if the file exists, false otherwise
     */
    static bool fileExists(const std::string& filename);

    /**
     * Returns whether the given directory exists.
     */
    static bool dirExists(const std::string& dirpath);
    
    /**
     * Reads the content of the the directory, ignoring "." and ".."
     *
     * @param   directory   the directory to be read
     * @param   sort    determines whether the returned vector should be sorted in
     *                  alphabetical order
     * @param   recursiveSearch determines whether to search the directory recursively.
     * @return  names of all files contained in the given directory. Files from sub-
     *          directories are inserted like "/subdirectory/file".
     *          If the directory does not exist, the returned vector is empty.
     */
    static std::vector<std::string> readDirectory(const std::string& directory, 
                                                  const bool sort = true,
                                                  const bool recursiveSearch = true);

protected:
    std::map<std::string, FileFactory*> virtualFS_;
    std::vector<FileFactory*> factories_;
    static const std::string loggerCat_;
};

} // namespace

#define FileSys tgt::Singleton<tgt::FileSystem>::getRef()

#endif //TGT_FILESYSTEM_H