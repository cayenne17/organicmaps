#pragma once

#include "coding/file_container.hpp"
#include "coding/mmap_reader.hpp"

#include "defines.hpp"

#include "std/cstdint.hpp"
#include "std/unique_ptr.hpp"
#include "std/vector.hpp"

#include "3party/succinct/elias_fano.hpp"
#include "3party/succinct/mapper.hpp"


namespace platform
{
  class LocalCountryFile;
}

namespace feature
{
  /// This class is a wrapper around elias-fano encoder, which allows
  /// to efficiently encode a sequence of strictly increasing features
  /// offsets in a MWM file and access them by feature's index.
  class FeaturesOffsetsTable
  {
  public:
    /// This class is used to accumulate strictly increasing features
    /// offsets and then build FeaturesOffsetsTable.
    class Builder
    {
    public:
      /// Adds offset to the end of the sequence of already
      /// accumulated offsets. Note that offset must be strictly
      /// greater than all previously added offsets.
      ///
      /// \param offset a feature's offset in a MWM file
      void PushOffset(uint32_t offset);

      /// \return number of already accumulated offsets
      inline size_t size() const { return m_offsets.size(); }

    private:
      friend class FeaturesOffsetsTable;

      vector<uint32_t> m_offsets;
    };

    /// Builds FeaturesOffsetsTable from the strictly increasing
    /// sequence of file offsets.
    ///
    /// \param builder Builder containing sequence of offsets.
    /// \return a pointer to an instance of FeaturesOffsetsTable
    static unique_ptr<FeaturesOffsetsTable> Build(Builder & builder);

    /// Load table by full path to the table file.
    static unique_ptr<FeaturesOffsetsTable> Load(string const & filePath);

    static unique_ptr<FeaturesOffsetsTable> Load(FilesContainerR const & cont);
    static unique_ptr<FeaturesOffsetsTable> Build(FilesContainerR const & cont,
                                                     string const & storePath);

    /// Get table for the MWM map, represented by localFile and cont.
    static unique_ptr<FeaturesOffsetsTable> CreateIfNotExistsAndLoad(
        platform::LocalCountryFile const & localFile, FilesContainerR const & cont);

    /// @todo The easiest solution for now. Need to be removed in future.
    //@{
    static unique_ptr<FeaturesOffsetsTable> CreateIfNotExistsAndLoad(platform::LocalCountryFile const & localFile);
    static unique_ptr<FeaturesOffsetsTable> CreateIfNotExistsAndLoad(FilesContainerR const & cont);
    //@}

    FeaturesOffsetsTable(FeaturesOffsetsTable const &) = delete;
    FeaturesOffsetsTable const & operator=(FeaturesOffsetsTable const &) = delete;

    /// Serializes current instance to a section in container.
    ///
    /// \param filePath a full path of the file to store data
    void Save(string const & filePath);

    /// \param index index of a feature
    /// \return offset a feature
    uint32_t GetFeatureOffset(size_t index) const;

    /// \param offset offset of a feature
    /// \return index of a feature
    size_t GetFeatureIndexbyOffset(uint32_t offset) const;

    /// \return number of features offsets in a table.
    inline size_t size() const { return static_cast<size_t>(m_table.num_ones()); }

    /// \return byte size of a table, may be slightly different from a
    ///         real byte size in memory or on disk due to alignment, but
    ///         can be used in benchmarks, logging, etc.
    //inline size_t byte_size() { return static_cast<size_t>(succinct::mapper::size_of(m_table)); }

  private:
    FeaturesOffsetsTable(succinct::elias_fano::elias_fano_builder & builder);
    FeaturesOffsetsTable(string const & filePath);
    FeaturesOffsetsTable() = default;

    static unique_ptr<FeaturesOffsetsTable> LoadImpl(string const & filePath);
    static unique_ptr<FeaturesOffsetsTable> CreateImpl(platform::LocalCountryFile const & localFile,
                                                       FilesContainerR const & cont,
                                                       string const & storePath);

    succinct::elias_fano m_table;
    unique_ptr<MmapReader> m_pReader;

    detail::MappedFile m_file;
    detail::MappedFile::Handle m_handle;
  };
}  // namespace feature
