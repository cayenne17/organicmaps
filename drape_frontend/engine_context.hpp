#pragma once

#include "threads_commutator.hpp"

#include "../drape/pointers.hpp"

#include "../map/scales_processor.hpp"

namespace df
{
  class Message;
  class MapShape;
  struct TileKey;

  class EngineContext
  {
  public:
    EngineContext(RefPointer<ThreadsCommutator> commutator);

    void BeginReadTile(TileKey const & key);
    /// If you call this method, you may forget about shape.
    /// It will be proccessed and delete later
    void InsertShape(TileKey const & key, TransferPointer<MapShape> shape);
    void EndReadTile(TileKey const & key);

  private:
    void PostMessage(Message * message);

  private:
    RefPointer<ThreadsCommutator> m_commutator;
  };
}
